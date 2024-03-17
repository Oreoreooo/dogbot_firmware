#ifndef __COMMUNICATION_HPP__
#define __COMMUNICATION_HPP__

#include "MotorController.hpp"
#include "Sensor.hpp"
#include <Arduino.h>
#include <MPU6050_light.h>

#define SR Serial  // Serial on USB
#define WR Serial3 // Wireless Module on Serial 3 (D14 & D15)

extern Sensor sensor;
extern MPU6050 mpu;
extern MotorController controller;

inline void setupCommunication()
{
    SR.begin(115200);
    WR.begin(38400);
}

inline char serialControlMotor()
{
    if (SR.available())
    {
        char buffer;
        buffer = SR.read();
        SR.flush();
        controller.command(buffer);
        return buffer;
    }
    return '\0';
}

inline char wirelessControlMotor()
{
    if (WR.available())
    {
        char buffer = WR.read();
        WR.flush();
        controller.command(buffer);
        return buffer;
    }
    return '\0';
}

inline void serialSensorDataTX()
{
    SR.print("LL=");
    SR.print(sensor.getLightLeft());
    SR.print(",LR=");
    SR.print(sensor.getLightRight());
    SR.print(",DT=");
    SR.print(sensor.getDistance());
    SR.print(",AZ=");
    SR.print(mpu.getAngleZ());
    SR.println("");
}

inline void wirelessSensorDataTX()
{
    WR.print("LL=");
    WR.print(sensor.getLightLeft());
    WR.print(",LR=");
    WR.print(sensor.getLightRight());
    WR.print(",DT=");
    WR.print(sensor.getDistance());
    WR.print(",AZ=");
    WR.print(mpu.getAngleZ());
    WR.println("");
}

#endif