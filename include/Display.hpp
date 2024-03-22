#ifndef __DISPLAY_HPP__
#define __DISPLAY_HPP__

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050_light.h>
#include <Wire.h>

#include "Sensor.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 28    // 4 // Reset pin # (or -1 if sharing Arduino reset pin)

extern MPU6050 mpu;
extern Sensor sensor;

Adafruit_SSD1306 _ssd1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

inline void setupDisplay()
{
    if (!_ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
    }
    _ssd1306.clearDisplay();
    _ssd1306.setTextSize(1);              // Normal 1:1 pixel scale
    _ssd1306.setTextColor(SSD1306_WHITE); // Draw white text
    _ssd1306.cp437(true);                 // Use full 256 char 'Code Page 437' font
}

inline void clearDisplay()
{
    _ssd1306.clearDisplay();
    _ssd1306.setCursor(0, 0);
}

inline void textDisplay(String data)
{
    clearDisplay();
    _ssd1306.println(data);
    _ssd1306.display();
}

inline void textDisplay(const char *data)
{
    clearDisplay();
    _ssd1306.println(data);
    _ssd1306.display();
}

inline void textDisplay(int data)
{
    clearDisplay();
    _ssd1306.println(data);
    _ssd1306.display();
}

inline void sensorDataDisplay()
{
    clearDisplay();
    _ssd1306.print("LL: ");
    _ssd1306.print(sensor.getLightL());
    _ssd1306.print("    LR: ");
    _ssd1306.print(sensor.getLightR());
    _ssd1306.print("\nDL: ");
    _ssd1306.print(sensor.getDistanceL(), 3);
    _ssd1306.print("\nDR: ");
    _ssd1306.print(sensor.getDistanceR(), 3);
    _ssd1306.print("\nAZ: ");
    _ssd1306.print(mpu.getAngleZ(), 3);
    _ssd1306.print(" deg\n");
    _ssd1306.display();
}
#endif