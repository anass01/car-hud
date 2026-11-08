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

        // Request 7 bytes touch report from SPD2010 (address 0x53)
        // Reg pointer 0x0001
        Wire.beginTransmission(SPD2010_TOUCH_ADDR);
        Wire.write(0x00);
        Wire.write(0x01);
        if (Wire.endTransmission(false) != 0)
        {
            // Direct read fallback if command phase not used
            Wire.requestFrom((uint8_t)SPD2010_TOUCH_ADDR, (size_t)7);
        }
        else
        {
            Wire.requestFrom((uint8_t)SPD2010_TOUCH_ADDR, (size_t)7);
        }

        if (Wire.available() < 7)
        {
            while (Wire.available()) Wire.read();
            return false;
        }

        uint8_t count   = Wire.read();
        uint8_t gesture = Wire.read();
        uint8_t b2      = Wire.read();
        uint8_t b3      = Wire.read();
        uint8_t b4      = Wire.read();
        uint8_t b5      = Wire.read();
        uint8_t weight  = Wire.read();

        if (count == 0 || weight == 0)
        {
            return false;
        }

        uint16_t raw_x = ((uint16_t)b3 << 8) | b2;
        uint16_t raw_y = ((uint16_t)b5 << 8) | b4;

        if (raw_x >= SCREEN_WIDTH && (((uint16_t)b2 << 8) | b3) < SCREEN_WIDTH)
        {
            raw_x = ((uint16_t)b2 << 8) | b3;
            raw_y = ((uint16_t)b4 << 8) | b5;
        }

        if (raw_x >= SCREEN_WIDTH) raw_x = SCREEN_WIDTH - 1;
        if (raw_y >= SCREEN_HEIGHT) raw_y = SCREEN_HEIGHT - 1;

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
};

DisplayWrapper tft;

