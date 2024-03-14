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
  void update();

  int getLightL();
  int getLightR();

  int getSonarL();
  int getSonarR();

  unsigned long getDistance();

  float getAngleX();
  // float getAngleY();
  float getAngleZ();

private:
  MPU6050 _mpu;

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
  _light_l = analogRead(PTR_L);
  _light_r = analogRead(PTR_R);
}

void Sensor::update()
{
  static unsigned long pulse_duration;
  static unsigned long start_time;
  static unsigned long delay_time;
  static bool measure_start;
  static bool measure_done;

  if (millis() > (delay_time + 65))
  {
    delay_time = millis();
    measure_start = true;
  }

  if (measure_start && measure_done)
  {
    measure_done = 0;
    start_time = micros();
    digitalWrite(LSR, LOW);
  }

  if (micros() > start_time + 16)
  {
    digitalWrite(LSR, HIGH);
  }

  if (micros() > start_time + 32)
  {
    digitalWrite(LSR, LOW);
    pinMode(LSR, INPUT);
    pulse_duration = pulseInLong(LSR, HIGH, 12000);
    _pulse_duration = _pulse_duration * 0.9 + 0.1 * pulse_duration;
    pinMode(LSR, OUTPUT);
    measure_done = true;
    measure_start = false;
  }

  _mpu.update();

  _light_l = _light_l * 0.9 + 0.1 * analogRead(PTR_L);
  _light_r = _light_r * 0.9 + 0.1 * analogRead(PTR_R);
}

int Sensor::getLightL()
{
  return _light_l;
}

int Sensor::getLightR()
{
  return _light_r;
}

int Sensor::getSonarL()
{
  return _sonar_l;
}

int Sensor::getSonarR()
{
  return _sonar_r;
}

unsigned long Sensor::getDistance() // Measure per 65ms
{
  return _pulse_duration * 0.1715;
}

float Sensor::getAngleX()
{
  return _mpu.getAngleX();
}

// float Sensor::getAngleY()
// {
//   return _mpu.getAngleY();
// }

float Sensor::getAngleZ()
{
  return _mpu.getAngleZ();
}

#endif
