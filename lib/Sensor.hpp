#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__
#include <Arduino.h>

#define PTR_L A0 // Phtotransistor L PIN
#define PTR_R A2 // Phtotransistor R PIN
#define LSR 48   // LaserPING PIN

class Sensor
{
public:
    Sensor(void);

    void begin();
    void update();

    int getLightLeft();  // voltage in 0-1023 (0-5V)
    int getLightRight(); // voltage in 0-1023 (0-5V)

    float getDistance(); // cm

private:
    unsigned long _pulse_duration;
    unsigned long _pulse_start_time;
    unsigned long _measure_start_time;
    bool _measure_start = false;
    bool _measure_done = false;

    int _light_L;
    int _light_R;
};

Sensor::Sensor(void) : _pulse_duration(0), _light_L(0), _light_R(0) {}

void Sensor::begin()
{
    pinMode(PTR_L, INPUT);
    pinMode(PTR_R, INPUT);
}

void Sensor::update()
{
    _light_L = 0.9 * _light_L + 0.1 * analogRead(PTR_L);
    _light_R = 0.9 * _light_R + 0.1 * analogRead(PTR_R);

    if (millis() > (_measure_start_time + 65))
    {
        _measure_start_time = millis();
        _measure_start = true;
    }

    if (_measure_start && _measure_done)
    {
        _measure_done = false;
        _pulse_start_time = millis();
        digitalWrite(LSR, LOW);
    }

    if (millis() > _pulse_start_time + 1)
    {
        digitalWrite(LSR, HIGH);
    }

    if (millis() > _pulse_start_time + 3)
    {
        digitalWrite(LSR, LOW);
        pinMode(LSR, INPUT);
        _pulse_duration = 0.9 * _pulse_duration + 0.1 * pulseInLong(LSR, HIGH, 12000);
        pinMode(LSR, OUTPUT);
        _measure_done = true;
        _measure_start = false;
    }
}

int Sensor::getLightLeft()
{
    return _light_L;
}

int Sensor::getLightRight()
{
    return _light_R;
}

// Distance is updated in every 65ms
float Sensor::getDistance()
{
    return _pulse_duration * 0.01715;
}

#endif
