#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

#define PTR_L A0 // Phtotransistor L PIN
#define PTR_R A2 // Phtotransistor R PIN

#define LSR 48 // LaserPING PIN

class Sensor
{
public:
  Sensor(void);

  void begin();
  inline void update();

  inline int getLightL();
  inline int getLightR();

  inline int getSonarL();
  inline int getSonarR();

  inline float getDistance();

  inline float getAngleX();
  inline float getAngleY();
  inline float getAngleZ();

private:
  MPU6050 _mpu;
  inline void _updateDistance();
  unsigned long _pulse_duration;
  int _light_l;
  int _light_r;
  int _sonar_l;
  int _sonar_r;
};

Sensor::Sensor(void) : _mpu(MPU6050(Wire)), _pulse_duration(0), _light_l(0), _light_r(0), _sonar_l(0), _sonar_r(0)
{
  pinMode(PTR_L, INPUT);
  pinMode(PTR_R, INPUT);
}

void Sensor::begin()
{
  _mpu.begin();
  delay(500);
  _mpu.calcGyroOffsets();
  _mpu.setFilterGyroCoef(0.965);
}

inline void Sensor::_updateDistance()
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
    pulse_start_time = micros();
    digitalWrite(LSR, LOW);
  }
  
  if (micros() > pulse_start_time + 8)
  {
    digitalWrite(LSR, HIGH);
  }


  if (micros() > pulse_start_time + 16)
  {
    pinMode(LSR, INPUT);
    _pulse_duration = 0.9 * _pulse_duration + 0.1 * pulseInLong(LSR, HIGH, 12000);
    pinMode(LSR, OUTPUT);
    measure_done = true;
    measure_start = false;
  }
}

inline void Sensor::update()
{
  _mpu.update();
  _light_l = 0.9 * _light_l + 0.1 * analogRead(PTR_L);
  _light_r = 0.9 * _light_r + 0.1 * analogRead(PTR_R);
  _updateDistance();
}

inline int Sensor::getLightL()
{
  return _light_l;
}

inline int Sensor::getLightR()
{
  return _light_r;
}

inline int Sensor::getSonarL()
{
  return _sonar_l;
}

inline int Sensor::getSonarR()
{
  return _sonar_r;
}

// Distance is updated in every 65ms
inline float Sensor::getDistance()
{
  return _pulse_duration * 0.01715;
}

inline float Sensor::getAngleX()
{
  return _mpu.getAngleX();
}

float Sensor::getAngleY()
{
  return _mpu.getAngleY();
}

inline float Sensor::getAngleZ()
{
  return _mpu.getAngleZ();
}

#endif
