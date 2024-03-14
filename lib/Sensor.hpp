#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

#define PTR_L A0 // Phtotransistor L PIN
#define PTR_R A2 // Phtotransistor R PIN
#define LSRP 25  // LaserPING PIN

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

  long getDepth();

  float getAngleX();
  float getAngleY();
  float getAngleZ();

private:
  MPU6050 _mpu;

  int _light_l;
  int _light_r;
  long _depth;
  int _sonar_l;
  int _sonar_r;
};

Sensor::Sensor(void) : _mpu(MPU6050(Wire)), _depth(0), _light_l(0), _light_r(0), _sonar_l(0), _sonar_r(0)
{
  pinMode(PTR_L, INPUT);
  pinMode(PTR_R, INPUT);
}

void Sensor::begin()
{
  _mpu.begin();
  delay(500);
  _mpu.calcGyroOffsets();
}

void Sensor::update()
{
  static bool lidar_done;
  static unsigned long lidar_time;
  static unsigned long lidar_duration;

  _mpu.update();

  _light_l = _light_l * 0.5 + 0.5 * analogRead(PTR_L);
  _light_r = _light_r * 0.5 + 0.5 * analogRead(PTR_R);

  if (lidar_done)
  {
    pinMode(LSRP, OUTPUT);
    lidar_done = false;
    lidar_time = millis();
    digitalWrite(LSRP, LOW);
  }

  if (millis() > lidar_time + 2 && millis() < lidar_time + 5)
  {
    digitalWrite(LSRP, HIGH);
  }

  if (millis() > lidar_time + 5)
  {
    digitalWrite(LSRP, LOW);
  }

  if (millis() > lidar_time + 700)
  {
    pinMode(LSRP, INPUT);
    lidar_duration = pulseIn(LSRP, HIGH);
    _depth = (_depth * 0.5) + (lidar_duration * 171.5 * 0.5);
    lidar_done = true;
  }
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

long Sensor::getDepth()
{
  return _depth;
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
