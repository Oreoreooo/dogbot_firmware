#ifndef __Display_HPP__
#define __Display_HPP__

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050_light.h>
#include <Wire.h>

#include "Sensor.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 28    // 4 // Reset pin # (or -1 if sharing Arduino reset pin)

extern Sensor sensor;
extern MPU6050 mpu;

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

inline void displayClear()
{
    _ssd1306.clearDisplay();
    _ssd1306.setCursor(0, 0);
}

inline void displayShow(String data)
{
    displayClear();
    _ssd1306.println(data);
    _ssd1306.display();
}

inline void displayShow(const char *data)
{
    displayClear();
    _ssd1306.println(data);
    _ssd1306.display();
}

inline void displaySensorData()
{
    displayClear();
    _ssd1306.print("L: ");
    _ssd1306.print(sensor.getLightLeft());
    _ssd1306.print("\nR: ");
    _ssd1306.print(sensor.getLightRight());
    _ssd1306.print("\nDT: ");
    _ssd1306.print(sensor.getDistance(), 3);
    _ssd1306.print(" cm\n");
    _ssd1306.print("AZ: ");
    _ssd1306.print(mpu.getAngleZ(), 3);
    _ssd1306.print(" deg\n");
    _ssd1306.display();
}

inline void displayMeasured()
{
    displayClear();
    _ssd1306.print("Distance: ");
    _ssd1306.print(sensor.getDistance(), 3);
    _ssd1306.print(" cm\n\n");
    _ssd1306.print("Angle: ");
    _ssd1306.print(mpu.getAngleZ() + 90, 3);
    _ssd1306.print(" deg\n");
    _ssd1306.display();
}

#endif