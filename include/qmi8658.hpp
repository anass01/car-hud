#pragma once
#include <Arduino.h>
#include <Wire.h>

class QMI8658
{
public:
    static const uint8_t ADDR1 = 0x6B;
    static const uint8_t ADDR2 = 0x6A;

    bool detected = false;
    uint8_t i2c_addr = ADDR1;
    float ax = 0.0f, ay = 0.0f, az = 0.0f;
    float gx = 0.0f, gy = 0.0f, gz = 1.0f; // Baseline gravity vector
    bool gravity_calibrated = false;
    int calib_samples = 0;
    float g_total = 0.0f;
    float g_lat = 0.0f;
    float g_lon = 0.0f;

    bool begin()
    {
        if (checkId(ADDR1))
        {
            i2c_addr = ADDR1;
            detected = true;
        }
        else if (checkId(ADDR2))
        {
            i2c_addr = ADDR2;
            detected = true;
        }
        else
        {
            return false;
        }

        // CTRL1 (0x02): Enable address auto-increment (bit 6 = 1 -> 0x60)
        writeReg(0x02, 0x60);
        // CTRL2 (0x03): Accelerometer +/-8g, 100Hz ODR (0x23)
        writeReg(0x03, 0x23);
        // CTRL7 (0x08): Enable Accelerometer and Gyroscope (0x03)
        writeReg(0x08, 0x03);

        gravity_calibrated = false;
        calib_samples = 0;

        return true;
    }

    bool update(bool is_stationary = false)
    {
        if (!detected)
            return false;

        Wire.beginTransmission(i2c_addr);
        Wire.write(0x35); // AX_L register
        if (Wire.endTransmission(false) != 0)
            return false;

        if (Wire.requestFrom((uint8_t)i2c_addr, (uint8_t)6) != 6)
            return false;

        uint8_t b0 = Wire.read();
        uint8_t b1 = Wire.read();
        uint8_t b2 = Wire.read();
        uint8_t b3 = Wire.read();
        uint8_t b4 = Wire.read();
        uint8_t b5 = Wire.read();

        int16_t raw_x = (int16_t)(b0 | (b1 << 8));
        int16_t raw_y = (int16_t)(b2 | (b3 << 8));
        int16_t raw_z = (int16_t)(b4 | (b5 << 8));

        // Scale +/-8g -> 4096 LSB/g (QMI8658 16-bit 2's complement)
        ax = (float)raw_x / 4096.0f;
        ay = (float)raw_y / 4096.0f;
        az = (float)raw_z / 4096.0f;

        // Initial gravity calibration over the first 25 samples
        if (!gravity_calibrated)
        {
            gx = gx * 0.8f + ax * 0.2f;
            gy = gy * 0.8f + ay * 0.2f;
            gz = gz * 0.8f + az * 0.2f;
            calib_samples++;
            if (calib_samples > 25)
                gravity_calibrated = true;
            g_total = 0.0f;
            return true;
        }

        // When stationary, adapt gravity vector faster to eliminate mounting tilt
        if (is_stationary)
        {
            gx = gx * 0.95f + ax * 0.05f;
            gy = gy * 0.95f + ay * 0.05f;
            gz = gz * 0.95f + az * 0.05f;
        }
        else
        {
            // Slow adaptation during driving to account for road gradient
            gx = gx * 0.998f + ax * 0.002f;
            gy = gy * 0.998f + ay * 0.002f;
            gz = gz * 0.998f + az * 0.002f;
        }

        // Compute unit gravity vector (vertical reference)
        float glen = sqrtf(gx * gx + gy * gy + gz * gz);
        float ux = (glen > 0.01f) ? (gx / glen) : 0.0f;
        float uy = (glen > 0.01f) ? (gy / glen) : 0.0f;
        float uz = (glen > 0.01f) ? (gz / glen) : 1.0f;

        // Dynamic acceleration: total measured minus static earth gravity
        float dax = ax - gx;
        float day = ay - gy;
        float daz = az - gz;

        // Component along gravity (vertical bumps/potholes)
        float a_vert = dax * ux + day * uy + daz * uz;

        // Horizontal vehicle component (braking, acceleration, cornering)
        float hx = dax - a_vert * ux;
        float hy = day - a_vert * uy;
        float hz = daz - a_vert * uz;

        float g_dyn = sqrtf(hx * hx + hy * hy + hz * hz);

        // Deadband filter for sensor noise
        if (g_dyn < 0.03f)
            g_dyn = 0.0f;

        g_total = g_dyn;
        return true;
    }

private:
    bool checkId(uint8_t addr)
    {
        Wire.beginTransmission(addr);
        Wire.write(0x00); // WHO_AM_I
        if (Wire.endTransmission(false) != 0)
            return false;
        if (Wire.requestFrom((uint8_t)addr, (uint8_t)1) != 1)
            return false;
        uint8_t id = Wire.read();
        return (id == 0x05);
    }

    void writeReg(uint8_t reg, uint8_t val)
    {
        Wire.beginTransmission(i2c_addr);
        Wire.write(reg);
        Wire.write(val);
        Wire.endTransmission();
    }
};

