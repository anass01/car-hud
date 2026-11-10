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
        delay(20);

        // Release Reset: Pull EXIO0 & EXIO1 HIGH
        Wire.beginTransmission(TCA9554_ADDR);
        Wire.write(0x01);
        Wire.write(0x03); // HIGH
        Wire.endTransmission();
        delay(50);

        // 3. Setup Backlight PWM on GPIO 5
        ledcSetup(0, 5000, 8); // Channel 0, 5kHz, 8-bit resolution
        ledcAttachPin(LCD_BL, 0);
        setBrightness(128);

        // 4. Setup Touch Interrupt Pin
        pinMode(TP_INT, INPUT_PULLUP);

        // Initialize SPD2010 Touch controller
        static const uint8_t cpu_start[2]  = { 0x01, 0x00 };
        static const uint8_t point_mode[2] = { 0x00, 0x00 };
        static const uint8_t start[2]      = { 0x00, 0x00 };

        clearInt();
        delay(5);
        writeCommand(0x0004, cpu_start, 2);
        delay(5);
        writeCommand(0x0050, point_mode, 2);
        delay(5);
        writeCommand(0x0046, start, 2);
        delay(5);
        clearInt();

        // Drain any pending boot interrupt
        for (int i = 0; i < 5 && digitalRead(TP_INT) == LOW; i++)
        {
            uint8_t dummy[4];
            readRegister(0x0020, dummy, 4);
            clearInt();
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
        ledcWrite(0, brightness);
    }

    void writePixel(int32_t x, int32_t y, const uint16_t color)
    {
        gfx->writePixel(x, y, color);
    }

    bool getTouch(uint16_t *x, uint16_t *y)
    {
        // INT is active LOW when touched
        if (digitalRead(TP_INT) != LOW)
        {
            return false;
        }

        uint8_t status[4] = {0};
        if (!readRegister(0x0020, status, 4))
        {
            clearInt();
            return false;
        }

        // If I2C bus returned idle pullup values (0xFF), no valid device response
        if (status[0] == 0xFF && status[1] == 0xFF && status[2] == 0xFF && status[3] == 0xFF)
        {
            return false;
        }

        // Check if chip is in BIOS or CPU transition state
        static const uint8_t cpu_start[2]  = { 0x01, 0x00 };
        static const uint8_t point_mode[2] = { 0x00, 0x00 };
        static const uint8_t start[2]      = { 0x00, 0x00 };

        bool tic_in_bios = (status[0] & 0x02) != 0;
        bool tic_in_cpu  = (status[0] & 0x04) != 0;

        if (tic_in_bios)
        {
            clearInt();
            writeCommand(0x0004, cpu_start, 2);
            return false;
        }
        if (tic_in_cpu)
        {
            writeCommand(0x0050, point_mode, 2);
            writeCommand(0x0046, start, 2);
            clearInt();
            return false;
        }

        bool pt_exist = (status[0] & 0x01) != 0;
        uint16_t read_len = (uint16_t)status[2] | ((uint16_t)status[3] << 8);

        if (!pt_exist || read_len < 10)
        {
            clearInt();
            return false;
        }

        uint8_t buf[16] = {0};
        uint8_t to_read = (read_len > 16) ? 16 : (uint8_t)read_len;
        if (!readRegister(0x0300, buf, to_read))
        {
            clearInt();
            return false;
        }

        clearInt(); // Acknowledge and release INT

        uint8_t weight = buf[8];
        if (weight == 0 || weight == 0xFF)
        {
            return false;
        }

        uint16_t raw_x = (((uint16_t)(buf[7] & 0xF0)) << 4) | (uint16_t)buf[5];
        uint16_t raw_y = (((uint16_t)(buf[7] & 0x0F)) << 8) | (uint16_t)buf[6];

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

    bool writeCommand(uint16_t reg, const uint8_t *data, size_t len)
    {
        Wire.beginTransmission(SPD2010_TOUCH_ADDR);
        Wire.write((uint8_t)(reg & 0xFF));
        Wire.write((uint8_t)(reg >> 8));
        for (size_t i = 0; i < len; i++)
        {
            Wire.write(data[i]);
        }
        return (Wire.endTransmission() == 0);
    }

    bool readRegister(uint16_t reg, uint8_t *data, size_t len)
    {
        Wire.beginTransmission(SPD2010_TOUCH_ADDR);
        Wire.write((uint8_t)(reg & 0xFF));
        Wire.write((uint8_t)(reg >> 8));
        if (Wire.endTransmission() != 0)
        {
            return false;
        }

        uint8_t read_bytes = Wire.requestFrom((uint8_t)SPD2010_TOUCH_ADDR, (size_t)len);
        if (read_bytes < len)
        {
            while (Wire.available()) Wire.read();
            return false;
        }

        for (size_t i = 0; i < len; i++)
        {
            data[i] = Wire.read();
        }
        return true;
    }

    void clearInt()
    {
        static const uint8_t ack[2]  = { 0x01, 0x00 };
        static const uint8_t rear[2] = { 0x00, 0x00 };
        writeCommand(0x0002, ack, 2);
        delayMicroseconds(200);
        writeCommand(0x0002, rear, 2);
    }
};

DisplayWrapper tft;

