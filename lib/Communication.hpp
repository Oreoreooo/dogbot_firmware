#ifndef __COMMUNICATION_HPP__
#define __COMMUNICATION_HPP__

#include "Motor.hpp"
#include "Sensor.hpp"
#include <Arduino.h>
#include <MPU6050_light.h>

#define SR Serial  // Serial on USB
#define WR Serial3 // Wireless Module on Serial 3 (D14 & D15)

extern MPU6050 mpu;

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
        command(buffer);
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
        command(buffer);
        return buffer;
    }
    return '\0';
}

inline void serialSensorDataTX()
{
    SR.print("LL=");
    SR.print(_light_L);
    SR.print(",LR=");
    SR.print(_light_R);
    SR.print(",DT=");
    SR.print(getDistance());
    SR.print(",AZ=");
    SR.print(mpu.getAngleZ());
    SR.println("");
}

void wirelessSensorDataTX()
{
    WR.print("LL=");
    WR.print(_light_L);
    WR.print(",LR=");
    WR.print(_light_R);
    WR.print(",DT=");
    WR.print(getDistance());
    WR.print(",AZ=");
    WR.print(mpu.getAngleZ());
    WR.println("");
}

#endif