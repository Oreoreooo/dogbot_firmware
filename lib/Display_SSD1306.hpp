#ifndef __OLED_SSD1306_HPP__
#define __OLED_SSD1306_HPP__

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

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

private:
    Adafruit_SSD1306 _sdd1306;
    int window_size;
};

Display_SSD1306::Display_SSD1306() : _sdd1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), window_size(0)
{
}

void Display_SSD1306::begin()
{
    if (!_sdd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
    }
    _sdd1306.clearDisplay();
    _sdd1306.setTextSize(1);              // Normal 1:1 pixel scale
    _sdd1306.setTextColor(SSD1306_WHITE); // Draw white text
    _sdd1306.cp437(true);                 // Use full 256 char 'Code Page 437' font
}

void Display_SSD1306::clear()
{
    _sdd1306.clearDisplay();
}

void Display_SSD1306::show(String data)
{
    _sdd1306.clearDisplay();
    _sdd1306.setCursor(0, 0); // Start at top-left corner
    _sdd1306.println(data);
    _sdd1306.display();
}

void Display_SSD1306::show(const char *data)
{
    _sdd1306.clearDisplay();
    _sdd1306.setCursor(0, 0); // Start at top-left corner
    _sdd1306.println(data);
    _sdd1306.display();
}

#endif