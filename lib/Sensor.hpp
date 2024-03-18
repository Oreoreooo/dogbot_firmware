#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__
#include <Arduino.h>

#define PTR_L A0 // Phtotransistor L PIN
#define PTR_R A2 // Phtotransistor R PIN
#define LSR 48   // LaserPING PIN

int _light_L;
int _light_R;
bool _measure_start = false;
bool _measure_done = false;
unsigned long _pulse_duration;
unsigned long _pulse_start_time;

inline void updateSensor()
{
    _light_L = 0.9 * _light_L + 0.1 * analogRead(PTR_L);
    _light_R = 0.9 * _light_R + 0.1 * analogRead(PTR_R);

    // if (_measure_done)
    // {
    //     _measure_done = false;
    //     _pulse_start_time = millis();
    //     digitalWrite(LSR, LOW);
    // }

    // if (millis() > _pulse_start_time + 65)
    // {
    //     digitalWrite(LSR, HIGH);
    // }

    // if (millis() > _pulse_start_time + 66)
    // {
    //     digitalWrite(LSR, LOW);
    //     pinMode(LSR, INPUT);
    //     _pulse_duration = 0.9 * _pulse_duration + 0.1 * pulseIn(LSR, HIGH, 12000);
    //     pinMode(LSR, OUTPUT);
    //     _measure_done = true;
    // }
}

inline void setupSensor()
{
    pinMode(PTR_L, INPUT);
    pinMode(PTR_R, INPUT);
    for (int i = 0; i < 500; i++)
    {
        updateSensor();
        delay(1);
    }
}

// Distance is updated in every 65ms
inline float getDistance()
{
    return _pulse_duration * 0.01715;
}

#endif
