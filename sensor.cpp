#include "sensor.h"

#define PTR_L A0 // Phtotransistor L PIN
#define PTR_R A2 // Phtotransistor R PIN

#define LSRP 25 // LaserPING PIN

int lidar_measure_done = 0;
unsigned long lidar_time = 0;
unsigned long pulse_duration;

Sensor::Sensor()
{
  pinMode(PTR_L, INPUT);
  pinMode(PTR_R, INPUT);
}

void Sensor::update()
{
  _light_l = _light_l * 0.5 + 0.5 * analogRead(PTR_L);
  _light_r = _light_r * 0.5 + 0.5 * analogRead(PTR_R);

  if (lidar_measure_done)
  {
    pinMode(LSRP, OUTPUT);
    lidar_measure_done = 0;
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
    pulse_duration = pulseIn(LSRP, HIGH);
    _length = (_length * 0.5) + (pulse_duration * 171.5 * 0.5);
    lidar_measure_done = 1;
  }
}