#include "sensor.h"

Sensor::Sensor()
{
  pinMode(PTR_L, INPUT);
  pinMode(PTR_R, INPUT);
}

void Sensor::update()
{
  this->lightL = this->lightL * 0.5 + 0.5 * analogRead(PTR_L);
  this->lightR = this->lightR * 0.5 + 0.5 * analogRead(PTR_R);

  pinMode(LSR, OUTPUT);
  digitalWrite(LSR, LOW);
  delayMicroseconds(2);
  digitalWrite(LSR, HIGH);
  delayMicroseconds(5);
  digitalWrite(LSR, LOW);
  pinMode(LSR, INPUT);

  this->distance = (this->distance * 0.5) + (pulseIn(LSR, HIGH) * 171.5 * 0.5);
}