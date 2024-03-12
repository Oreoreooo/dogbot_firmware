#ifndef __SENSOR_H__
#define __SENSOR_H__
#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_light.h>

class Sensor
{
public:
    Sensor(void);

    void update();

    int getLight() { return this->light; }  // 0: Left, 1: Right
    int getSonar() { return this->sonar; }  // 0: Left, 1: Right
    int getAngle() { return this->angle; }  // 0: X, 1: Y, 2: Z
    float getDistance() { return this->distance; }  // cm

private:
    float distance = 0;
    int light[2] = {0, 0};
    int sonar[2] = {0, 0};
    float angle[3] = {0, 0, 0};
    float acc[3] = {0, 0, 0};
};

#endif