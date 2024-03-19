#ifndef __COMMUNICATION_HPP__
#define __COMMUNICATION_HPP__

#include "MotorController.hpp"
#include "Sensor.hpp"
#include <Arduino.h>
#include <MPU6050_light.h>

#define SR Serial  // Serial on USB
#define WR Serial3 // Wireless Module on Serial 3 (D14 & D15)

extern MPU6050 mpu;
extern Sensor sensor;
extern MotorController motor;

inline void setupCommunication()
{
    SR.begin(115200);
    WR.begin(38400); // 00:23:10:34:F0:E1
}

inline void serialControlMotor()
{
    if (SR.available())
    {
        char buffer;
        buffer = SR.read();
        SR.flush();
        motor.command(buffer);
    }
}

inline void wirelessControlMotor()
{
    if (WR.available())
    {
        char buffer = WR.read();
        WR.flush();
        motor.command(buffer);
    }
}

inline void serialSensorDataTX()
{
    SR.print("LL=");
    SR.print(sensor.getLightL());
    SR.print(",LR=");
    SR.print(sensor.getLightR());
    SR.print(",DT=");
    SR.print(sensor.getDistanceL());
    SR.print(",AZ=");
    SR.print(mpu.getAngleZ());
    SR.println();
}

inline void wirelessSensorDataTX()
{
    WR.print("LL=");
    WR.print(sensor.getLightL());
    WR.print(",LR=");
    WR.print(sensor.getLightR());
    WR.print(",DT=");
    WR.print(sensor.getDistanceL());
    WR.print(",AZ=");
    WR.print(mpu.getAngleZ());
    SR.println();
}

#endif