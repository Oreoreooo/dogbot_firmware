#include "sensor.h"
#include <Wire.h>
#include <MPU6050_light.h>

#define PTR_L A0 // Phtotransistor Left PIN
#define PTR_R A2 // Phtotransistor Right PIN
#define LSR A3   // LaserPING PIN

Sensor::Sensor()
{
    Wire.begin();
    mpu.begin(Wire);
    pinMode(PTR_L, INPUT);
    pinMode(PTR_R, INPUT);
    delay(1000);
    mpu.calcGyroOffsets();
}

void Sensor::update()
{
    this->light[0] = this->light[0] * 0.5 + 0.5 * analogRead(PTR_L);
    this->light[1] = this->light[1] * 0.5 + 0.5 * analogRead(PTR_R);

    pinMode(LSR, OUTPUT);
    digitalWrite(LSR, LOW);
    delayMicroseconds(2);
    digitalWrite(LSR, HIGH);
    delayMicroseconds(5);
    digitalWrite(LSR, LOW);
    pinMode(LSR, INPUT);

    this->distance = (this->distance * 0.5) + (pulseIn(LSR, HIGH) * 171.5 * 0.5);

    mpu.update();
    this->angle[0] = mpu.getAngleX();
    this->angle[1] = mpu.getAngleY();
    this->angle[2] = mpu.getAngleZ();
    this->acc[0] = mpu.getAccX();
    this->acc[1] = mpu.getAccY();
    this->acc[2] = mpu.getAccZ();
}