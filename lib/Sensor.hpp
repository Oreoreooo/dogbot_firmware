#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__
#include <Arduino.h>

#define PTR_L A0 // Phtotransistor L PIN
#define PTR_R A2 // Phtotransistor R PIN
#define LSR_L 48 // LaserPING L PIN
#define LSR_R 22 // LaserPING R PIN

class Sensor
{
public:
    Sensor() {}

    inline void begin()
    {
        pinMode(PTR_L, INPUT);
        pinMode(PTR_R, INPUT);
    }

    inline void update()
    {
        int light_val_L = analogRead(PTR_L);
        int light_val_R = analogRead(PTR_R);
        _light_L = 0.5 * _light_L + 0.5 * map(light_val_L, 0, 400, 0, 255);
        _light_R = 0.5 * _light_R + 0.5 * map(light_val_R, 0, 300, 0, 255);
        if (!_measure_done)
        {
            if (_measure_done_l)
            {
                _measure_done_l = false;
                _pulse_start_time_l = millis();
                digitalWrite(LSR_L, LOW);
            }

            if (millis() > _pulse_start_time_l + 65)
            {
                digitalWrite(LSR_L, HIGH);
            }

            if (millis() > _pulse_start_time_l + 66)
            {
                digitalWrite(LSR_L, LOW);
                pinMode(LSR_L, INPUT);
                _pulse_duration_l = pulseIn(LSR_L, HIGH, 12000);
                pinMode(LSR_L, OUTPUT);
                _measure_done_l = true;
                _measure_done = true;
            }
        }
        else
        {
            if (_measure_done_r)
            {
                _measure_done_r = false;
                _pulse_start_time_r = millis();
                digitalWrite(LSR_R, LOW);
            }

            if (millis() > _pulse_start_time_r + 1)
            {
                digitalWrite(LSR_R, HIGH);
            }

            if (millis() > _pulse_start_time_r + 2)
            {
                digitalWrite(LSR_R, LOW);
                pinMode(LSR_R, INPUT);
                _pulse_duration_r = pulseIn(LSR_R, HIGH, 12000);
                pinMode(LSR_R, OUTPUT);
                _measure_done_r = true;
                _measure_done = false;
            }
        }
    }

    inline float getDistanceL()
    {
        return _pulse_duration_l * 0.01715;
    }

    inline float getDistanceR()
    {
        return _pulse_duration_r * 0.01715;
    }

    inline float getDistanceError()
    {
        return getDistanceL() - getDistanceR();
    }

    inline float getAverageDistance()
    {
        return (getDistanceL() + getDistanceR()) / 2;
    }

    inline int getLightL()
    {
        return _light_L;
    }

    inline int getLightR()
    {
        return _light_R;
    }

private:
    int _light_L;
    int _light_R;
    bool _measure_start_l = false;
    bool _measure_start_r = false;
    bool _measure_done_l = false;
    bool _measure_done_r = false;
    bool _measure_done = false;
    unsigned long _pulse_duration_l;
    unsigned long _pulse_duration_r;
    unsigned long _pulse_start_time_l;
    unsigned long _pulse_start_time_r;
};

#endif
