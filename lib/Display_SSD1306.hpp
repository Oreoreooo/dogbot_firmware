#ifndef __OLED_SSD1306_HPP__
#define __OLED_SSD1306_HPP__

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "Sensor.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 28    // 4 // Reset pin # (or -1 if sharing Arduino reset pin)

class Display_SSD1306
{

public:
    Display_SSD1306();
    void begin();
    void clear();
    void show(String data);
    void show(const char *data);

    void displaySensorData(Sensor *sensor);
    void displayMeasured(Sensor *sensor);

    void setTextSize(uint8_t size);

private:
    Adafruit_SSD1306 _ssd1306;
};

Display_SSD1306::Display_SSD1306() : _ssd1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

void Display_SSD1306::begin()
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

void Display_SSD1306::clear()
{
    _ssd1306.clearDisplay();
    _ssd1306.setCursor(0, 0);
}

void Display_SSD1306::show(String data)
{
    clear();
    _ssd1306.println(data);
    _ssd1306.display();
}

void Display_SSD1306::show(const char *data)
{
    clear();
    _ssd1306.println(data);
    _ssd1306.display();
}

void Display_SSD1306::displaySensorData(Sensor *sensor)
{
    clear();
    _ssd1306.print("L: ");
    _ssd1306.print(sensor->getLightLeft());
    _ssd1306.print("\nR: ");
    _ssd1306.print(sensor->getLightRight());
    _ssd1306.print("\nDT: ");
    _ssd1306.print(sensor->getDistance(), 3);
    _ssd1306.print(" cm\n");
    _ssd1306.print("AZ: ");
    _ssd1306.print(sensor->getAngleZ(), 3);
    _ssd1306.print(" deg\n");
    _ssd1306.display();
}

void Display_SSD1306::displayMeasured(Sensor *sensor)
{
    clear();
    _ssd1306.print("Distance: ");
    _ssd1306.print(sensor->getDistance(), 3);
    _ssd1306.print(" cm\n\n");
    _ssd1306.print("Angle: ");
    _ssd1306.print(sensor->getAngleZ() + 90, 3);
    _ssd1306.print(" deg\n");
    _ssd1306.display();
}

void Display_SSD1306::setTextSize(uint8_t size)
{
    _ssd1306.setTextSize(size);
}

#endif