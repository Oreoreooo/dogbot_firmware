#ifndef __COMMUNICATION_SERIAL_HPP__
#define __COMMUNICATION_SERIAL_HPP__

#include "Display_SSD1306.hpp"
#include "MotorController.hpp"
#include "Sensor.hpp"
#include <Arduino.h>

#define SR Serial  // Serial on USB
#define WR Serial3 // Wireless Module on Serial 3 (D14 & D15)

class CommunicationSerial
{
public:
    CommunicationSerial(void);
    void begin();

    char serialControlMotor(MotorController *motor_controller);
    char wirelessControlMotor(MotorController *motor_controller);

    inline void serialSensorDataTX(Sensor *sensor, MPU6050 *mpu);
    inline void wirelessSensorDataTX(Sensor *sensor, MPU6050 *mpu);
};

CommunicationSerial::CommunicationSerial(void) {}

void CommunicationSerial::begin()
{
    SR.begin(115200);
    WR.begin(38400);
}

char CommunicationSerial::serialControlMotor(MotorController *motor_controller)
{
    if (SR.available())
    {
        char buffer;
        buffer = SR.read();
        SR.flush();
        motor_controller->command(buffer);
        return buffer;
    }
    return '\0';
}

char CommunicationSerial::wirelessControlMotor(MotorController *motor_controller)
{
    if (WR.available())
    {
        char buffer = WR.read();
        WR.flush();
        motor_controller->command(buffer);
        return buffer;
    }
    return '\0';
}

void CommunicationSerial::serialSensorDataTX(Sensor *sensor, MPU6050 *mpu)
{
    SR.print("LL=");
    SR.print(sensor->getLightLeft());
    SR.print(",LR=");
    SR.print(sensor->getLightRight());
    SR.print(",DT=");
    SR.print(sensor->getDistance());
    SR.print(",AZ=");
    SR.print(mpu->getAngleZ());
    SR.println("");
}

void CommunicationSerial::wirelessSensorDataTX(Sensor *sensor, MPU6050 *mpu)
{
    WR.print("LL=");
    WR.print(sensor->getLightLeft());
    WR.print(",LR=");
    WR.print(sensor->getLightRight());
    WR.print(",DT=");
    WR.print(sensor->getDistance());
    WR.print(",AZ=");
    WR.print(mpu->getAngleZ());
    WR.println("");
}

#endif