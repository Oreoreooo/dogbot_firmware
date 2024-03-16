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

    void serialSensorDataTX(Sensor *sensor);
    void wirelessSensorDataTX(Sensor *sensor);

private:
    String _commandToString(char data);
};

CommunicationSerial::CommunicationSerial(void) {}

void CommunicationSerial::begin()
{
    SR.begin(115200);
    WR.begin(38400);
}

char CommunicationSerial::serialControlMotor(MotorController *motor_controller)
{
    static char buffer;
    if (SR.available())
    {
        buffer = SR.read();
        SR.flush();
        motor_controller->command(buffer);
    }
    return buffer;
}

char CommunicationSerial::wirelessControlMotor(MotorController *motor_controller)
{
    static char buffer;
    if (WR.available())
    {
        buffer = WR.read();
        WR.flush();
        motor_controller->command(buffer);
    }
    return buffer;
}

void CommunicationSerial::serialSensorDataTX(Sensor *sensor)
{
    SR.print("LL=");
    SR.print(sensor->getLightLeft());
    SR.print(",LR=");
    SR.print(sensor->getLightRight());
    SR.print(",DT=");
    SR.print(sensor->getDistance());
    SR.print(",AX=");
    SR.print(sensor->getAngleX());
    SR.print(",AZ=");
    SR.print(sensor->getAngleZ());
    SR.println("");
}

void CommunicationSerial::wirelessSensorDataTX(Sensor *sensor)
{
    WR.print("LL=");
    WR.print(sensor->getLightLeft());
    WR.print(",LR=");
    WR.print(sensor->getLightRight());
    WR.print(",DT=");
    WR.print(sensor->getDistance());
    WR.print(",AX=");
    WR.print(sensor->getAngleX());
    WR.print(",AZ=");
    WR.print(sensor->getAngleZ());
    WR.println("");
}

#endif