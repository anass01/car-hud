#pragma once
#include <Arduino.h>
#include <Wire.h>

#include "Arduino_GFX_Library.h"
#include "pins.h"

#define TFT_BLACK 0x0000

#define FLIP_NONE 0
#define FLIP_X 1
#define FLIP_Y 2
#define FLIP_XY 3

class DisplayWrapper
{
public:
    Arduino_DataBus *bus;
    Arduino_GFX *gfx;

    DisplayWrapper()
    {
        bus = new Arduino_ESP32QSPI(
            LCD_CS /* CS */, LCD_SCK /* SCK */, LCD_SD0 /* SDIO0 */, LCD_SD1 /* SDIO1 */,
            LCD_SD2 /* SDIO2 */, LCD_SD3 /* SDIO3 */);

        gfx = new Arduino_SPD2010(bus, -1 /* RST via TCA9554 */);
    }

    ~DisplayWrapper()
    {
        delete gfx;
        delete bus;
    }

    bool init(void)
    {
        // 1. Initialize I2C bus for TCA9554 and Touch
        Wire.begin(I2C_SDA, I2C_SCL, 400000);

        // 2. Configure TCA9554 (IO Expander)
        // EXIO0 = TP_RST, EXIO1 = LCD_RST
        // Register 0x03 is Configuration Register (0 = Output, 1 = Input)
        Wire.beginTransmission(TCA9554_ADDR);
        Wire.write(0x03);
        Wire.write(0xFC); // Pin 0 and 1 as outputs (0), others as inputs (1)
        Wire.endTransmission();

        // Hardware Reset: Pull EXIO0 & EXIO1 LOW
        Wire.beginTransmission(TCA9554_ADDR);
        Wire.write(0x01); // Output Port Register
        Wire.write(0x00); // LOW
        Wire.endTransmission();
        delay(50);

        // Release Reset: Pull EXIO0 & EXIO1 HIGH
        Wire.beginTransmission(TCA9554_ADDR);
        Wire.write(0x01);
        Wire.write(0x03); // HIGH
        Wire.endTransmission();
        delay(50);

        // 3. Setup Backlight PWM on GPIO 5 (5 kHz, 8-bit resolution)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
        ledcAttach(LCD_BL, 5000, 8);
#else
        ledcSetup(0, 5000, 8); // Channel 0, 5kHz, 8-bit resolution
        ledcAttachPin(LCD_BL, 0);
#endif
        setBrightness(128);

        // 4. Setup Touch Interrupt Pin
        pinMode(TP_INT, INPUT_PULLUP);

        // Check if SPD2010 responds on I2C bus
        Wire.beginTransmission(SPD2010_TOUCH_ADDR);
        if (Wire.endTransmission() != 0)
        {
            Serial.println("[TOUCH] SPD2010 not detected at 0x53");
        }
        else
        {
            Serial.println("[TOUCH] SPD2010 detected at 0x53");
        }

        read_fw_version();

        // Step through SPD2010 BIOS -> CPU -> RUN initialization
        for (int i = 0; i < 20; i++)
        {
            SPD2010_Touch dummy = {0};
            tp_read_data(&dummy);
            delay(10);
        }

        // 5. Initialize Display
        bool state = gfx->begin();
        return state;
    }

    void initDMA(void) {}

    void fillScreen(uint16_t color)
    {
        gfx->fillScreen(color);
    }

    void setRotation(uint8_t rotation)
    {
        // Not used with round display
    }

    void setFlipMode(uint8_t mode)
    {
        switch (mode)
        {
        case FLIP_NONE:
            flip_x = false;
            flip_y = false;
            break;
        case FLIP_X:
            flip_x = true;
            flip_y = false;
            break;
        case FLIP_Y:
            flip_x = false;
            flip_y = true;
            break;
        case FLIP_XY:
            flip_x = true;
            flip_y = true;
            break;
        default:
            break;
        }
    }

    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data)
    {
        gfx->draw16bitBeRGBBitmap(x, y, data, w, h);
    }

    void pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data)
    {
        gfx->draw16bitBeRGBBitmap(x, y, data, w, h);
    }

    void startWrite(void) {}

    uint32_t getStartCount(void)
    {
        return 0;
    }

    void endWrite(void) {}

    void setBrightness(uint8_t brightness)
    {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
        ledcWrite(LCD_BL, brightness);
#else
        ledcWrite(0, brightness);
#endif
    }

    void writePixel(int32_t x, int32_t y, const uint16_t color)
    {
        gfx->writePixel(x, y, color);
    }

    struct tp_report_t {
        uint8_t id;
        uint16_t x;
        uint16_t y;
        uint8_t weight;
    };

    struct SPD2010_Touch {
        tp_report_t rpt[5];
        uint8_t touch_num;
        uint8_t pack_code;
        uint8_t down;
        uint8_t up;
        uint8_t gesture;
        uint16_t down_x;
        uint16_t down_y;
        uint16_t up_x;
        uint16_t up_y;
    };

    struct tp_status_high_t {
        uint8_t none0;
        uint8_t none1;
        uint8_t none2;
        uint8_t cpu_run;
        uint8_t tint_low;
        uint8_t tic_in_cpu;
        uint8_t tic_in_bios;
        uint8_t tic_busy;
    };

    struct tp_status_low_t {
        uint8_t pt_exist;
        uint8_t gesture;
        uint8_t key;
        uint8_t aux;
        uint8_t keep;
        uint8_t raw_or_pt;
        uint8_t none6;
        uint8_t none7;
    };

    struct tp_status_t {
        tp_status_low_t status_low;
        tp_status_high_t status_high;
        uint16_t read_len;
    };

    struct tp_hdp_status_t {
        uint8_t status;
        uint16_t next_packet_len;
    };

    bool getTouch(uint16_t *x, uint16_t *y)
    {
        SPD2010_Touch touch = {0};
        if (!tp_read_data(&touch))
        {
            return false;
        }

        if (touch.touch_num == 0)
        {
            return false;
        }

        uint8_t weight = touch.rpt[0].weight;
        if (weight == 0 || weight == 0xFF)
        {
            return false;
        }

        uint16_t raw_x = touch.rpt[0].x;
        uint16_t raw_y = touch.rpt[0].y;

        if (raw_x >= SCREEN_WIDTH || raw_y >= SCREEN_HEIGHT)
        {
            return false;
        }

        if (flip_x)
        {
            raw_x = SCREEN_WIDTH - 1 - raw_x;
        }
        if (flip_y)
        {
            raw_y = SCREEN_HEIGHT - 1 - raw_y;
        }

        *x = raw_x;
        *y = raw_y;
        return true;
    }

private:
    bool flip_x = false;
    bool flip_y = false;

    static bool I2C_Read_Touch(uint8_t Driver_addr, uint16_t Reg_addr, uint8_t *Reg_data, uint32_t Length)
    {
        Wire.beginTransmission(Driver_addr);
        Wire.write((uint8_t)(Reg_addr >> 8));
        Wire.write((uint8_t)Reg_addr);
        if (Wire.endTransmission(true) != 0)
        {
            return false;
        }
        uint8_t read_bytes = Wire.requestFrom((uint8_t)Driver_addr, (size_t)Length);
        if (read_bytes < Length)
        {
            while (Wire.available()) Wire.read();
            return false;
        }
        for (uint32_t i = 0; i < Length; i++)
        {
            Reg_data[i] = Wire.read();
        }
        return true;
    }

    static bool I2C_Write_Touch(uint8_t Driver_addr, uint16_t Reg_addr, const uint8_t *Reg_data, uint32_t Length)
    {
        Wire.beginTransmission(Driver_addr);
        Wire.write((uint8_t)(Reg_addr >> 8));
        Wire.write((uint8_t)Reg_addr);
        for (uint32_t i = 0; i < Length; i++)
        {
            Wire.write(Reg_data[i]);
        }
        return (Wire.endTransmission(true) == 0);
    }

    static void write_tp_point_mode_cmd()
    {
        static const uint8_t data[2] = { 0x00, 0x00 };
        I2C_Write_Touch(SPD2010_TOUCH_ADDR, 0x5000, data, 2);
        delayMicroseconds(200);
    }

    static void write_tp_start_cmd()
    {
        static const uint8_t data[2] = { 0x00, 0x00 };
        I2C_Write_Touch(SPD2010_TOUCH_ADDR, 0x4600, data, 2);
        delayMicroseconds(200);
    }

    static void write_tp_cpu_start_cmd()
    {
        static const uint8_t data[2] = { 0x01, 0x00 };
        I2C_Write_Touch(SPD2010_TOUCH_ADDR, 0x0400, data, 2);
        delayMicroseconds(200);
    }

    static void write_tp_clear_int_cmd()
    {
        static const uint8_t data[2] = { 0x01, 0x00 };
        I2C_Write_Touch(SPD2010_TOUCH_ADDR, 0x0200, data, 2);
        delayMicroseconds(200);
    }

    static bool read_tp_status_length(tp_status_t *tp_status)
    {
        uint8_t sample_data[4] = {0};
        if (!I2C_Read_Touch(SPD2010_TOUCH_ADDR, 0x2000, sample_data, 4))
        {
            return false;
        }
        delayMicroseconds(200);
        tp_status->status_low.pt_exist = (sample_data[0] & 0x01);
        tp_status->status_low.gesture = (sample_data[0] & 0x02);
        tp_status->status_low.aux = (sample_data[0] & 0x08);
        tp_status->status_high.tic_busy = ((sample_data[1] & 0x80) >> 7);
        tp_status->status_high.tic_in_bios = ((sample_data[1] & 0x40) >> 6);
        tp_status->status_high.tic_in_cpu = ((sample_data[1] & 0x20) >> 5);
        tp_status->status_high.tint_low = ((sample_data[1] & 0x10) >> 4);
        tp_status->status_high.cpu_run = ((sample_data[1] & 0x08) >> 3);
        tp_status->read_len = ((uint16_t)sample_data[3] << 8) | sample_data[2];
        return true;
    }

    static bool read_tp_hdp_status(tp_hdp_status_t *tp_hdp_status)
    {
        uint8_t sample_data[8] = {0};
        if (!I2C_Read_Touch(SPD2010_TOUCH_ADDR, 0xFC02, sample_data, 8))
        {
            return false;
        }
        tp_hdp_status->status = sample_data[5];
        tp_hdp_status->next_packet_len = ((uint16_t)sample_data[2]) | (((uint16_t)sample_data[3]) << 8);
        return true;
    }

    static bool Read_HDP_REMAIN_DATA(tp_hdp_status_t *tp_hdp_status)
    {
        uint8_t sample_data[64] = {0};
        uint16_t len = tp_hdp_status->next_packet_len;
        if (len > sizeof(sample_data)) len = sizeof(sample_data);
        return I2C_Read_Touch(SPD2010_TOUCH_ADDR, 0x0003, sample_data, len);
    }

    static bool read_tp_hdp(tp_status_t *tp_status, SPD2010_Touch *touch)
    {
        uint8_t sample_data[64] = {0};
        uint16_t len = tp_status->read_len;
        if (len > sizeof(sample_data)) len = sizeof(sample_data);
        if (!I2C_Read_Touch(SPD2010_TOUCH_ADDR, 0x0003, sample_data, len))
        {
            return false;
        }

        uint8_t check_id = sample_data[4];
        if ((check_id <= 0x0A) && tp_status->status_low.pt_exist)
        {
            touch->touch_num = ((tp_status->read_len - 4) / 6);
            if (touch->touch_num > 5) touch->touch_num = 5;
            touch->gesture = 0x00;
            for (uint8_t i = 0; i < touch->touch_num; i++)
            {
                uint8_t offset = i * 6;
                touch->rpt[i].id = sample_data[4 + offset];
                touch->rpt[i].x = (((uint16_t)(sample_data[7 + offset] & 0xF0)) << 4) | sample_data[5 + offset];
                touch->rpt[i].y = (((uint16_t)(sample_data[7 + offset] & 0x0F)) << 8) | sample_data[6 + offset];
                touch->rpt[i].weight = sample_data[8 + offset];
            }
        }
        else if ((check_id == 0xF6) && tp_status->status_low.gesture)
        {
            touch->touch_num = 0x00;
            touch->gesture = sample_data[6] & 0x07;
        }
        else
        {
            touch->touch_num = 0x00;
            touch->gesture = 0x00;
        }
        return true;
    }

    static bool tp_read_data(SPD2010_Touch *touch)
    {
        tp_status_t tp_status = {0};
        tp_hdp_status_t tp_hdp_status = {0};
        if (!read_tp_status_length(&tp_status))
        {
            return false;
        }

        if (tp_status.status_high.tic_in_bios)
        {
            write_tp_clear_int_cmd();
            write_tp_cpu_start_cmd();
        }
        else if (tp_status.status_high.tic_in_cpu)
        {
            write_tp_point_mode_cmd();
            write_tp_start_cmd();
            write_tp_clear_int_cmd();
        }
        else if (tp_status.status_high.cpu_run && tp_status.read_len == 0)
        {
            write_tp_clear_int_cmd();
        }
        else if (tp_status.status_low.pt_exist || tp_status.status_low.gesture)
        {
            read_tp_hdp(&tp_status, touch);
            int retry = 5;
            while (retry-- > 0)
            {
                if (!read_tp_hdp_status(&tp_hdp_status)) break;
                if (tp_hdp_status.status == 0x82)
                {
                    write_tp_clear_int_cmd();
                    break;
                }
                else if (tp_hdp_status.status == 0x00)
                {
                    Read_HDP_REMAIN_DATA(&tp_hdp_status);
                }
                else
                {
                    break;
                }
            }
        }
        else if (tp_status.status_high.cpu_run && tp_status.status_low.aux)
        {
            write_tp_clear_int_cmd();
        }

        return true;
    }

    static void read_fw_version()
    {
        uint8_t sample_data[18] = {0};
        if (I2C_Read_Touch(SPD2010_TOUCH_ADDR, 0x2600, sample_data, 18))
        {
            uint16_t DVer = ((uint16_t)sample_data[5] << 8) | sample_data[4];
            uint32_t PID = (((uint32_t)sample_data[9]) << 24) | (((uint32_t)sample_data[8]) << 16) | (((uint32_t)sample_data[7]) << 8) | sample_data[6];
            Serial.printf("[TOUCH] SPD2010 FW read OK: DVer=%d, PID=%lu\r\n", DVer, PID);
        }
        else
        {
            Serial.println("[TOUCH] SPD2010 FW read failed");
        }
    }
};

DisplayWrapper tft;

