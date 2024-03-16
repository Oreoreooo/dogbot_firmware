#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

#define PTR_L A0 // Phtotransistor L PIN
#define PTR_R A2 // Phtotransistor R PIN
#define LSR 48   // LaserPING PIN

class Sensor
{
public:
    Sensor(void);

    void begin();
    void update();

    int getLightLeft();
    int getLightRight();

    int getSonarLeft();
    int getSonarRight();

    float getDistance();

    float getAngleX();
    float getAngleY();
    float getAngleZ();

private:
    MPU6050 _mpu;
    void _updatePulse();
    unsigned long _pulse_duration;
    int _light_L;
    int _light_R;

    int _sonar_L;
    int _sonar_R;
};

Sensor::Sensor(void) : _mpu(MPU6050(Wire)) {}

void Sensor::begin()
{
    pinMode(PTR_L, INPUT);
    pinMode(PTR_R, INPUT);
    _mpu.begin();
    delay(500);
    _mpu.calcGyroOffsets();
    _mpu.setFilterGyroCoef(0.965);
}

void Sensor::_updatePulse()
{
    static unsigned long pulse_start_time;
    static unsigned long measure_start_time;
    static bool measure_start = false;
    static bool measure_done = false;

    if (millis() > (measure_start_time + 65))
    {
        measure_start_time = millis();
        measure_start = true;
    }

    if (measure_start && measure_done)
    {
        measure_done = false;
        pulse_start_time = millis();
        digitalWrite(LSR, LOW);
    }

    if (millis() > pulse_start_time + 1)
    {
        digitalWrite(LSR, HIGH);
    }

    if (millis() > pulse_start_time + 3)
    {
        digitalWrite(LSR, LOW);
        pinMode(LSR, INPUT);
        _pulse_duration = 0.9 * _pulse_duration + 0.1 * pulseInLong(LSR, HIGH, 12000);
        pinMode(LSR, OUTPUT);
        measure_done = true;
        measure_start = false;
    }
}

void Sensor::update()
{
    _mpu.update();

    _light_L = 0.9 * _light_L + 0.1 * analogRead(PTR_L);
    _light_R = 0.9 * _light_R + 0.1 * analogRead(PTR_R);

    _updatePulse();
}

int Sensor::getLightLeft()
{
    return _light_L;
}

int Sensor::getLightRight()
{
    return _light_R;
}

int Sensor::getSonarLeft()
{
    return _sonar_L;
}

int Sensor::getSonarRight()
{
    return _sonar_R;
}

// Distance is updated in every 65ms
float Sensor::getDistance()
{
    return _pulse_duration * 0.01715;
}

float Sensor::getAngleX()
{
    return _mpu.getAngleX();
}

float Sensor::getAngleY()
{
    return _mpu.getAngleY();
}

float Sensor::getAngleZ()
{
    return _mpu.getAngleZ();
}

#endif
