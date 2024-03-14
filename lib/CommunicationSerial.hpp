#ifndef __COMMUNICATION_SERIAL_HPP__
#define __COMMUNICATION_SERIAL_HPP__

#include "Display_SSD1306.hpp"
#include "Motor.hpp"
#include "Sensor.hpp"
#include <Arduino.h>

#define SR Serial  // Serial on USB
#define WR Serial3 // Wireless Module on Serial 3 (D14 & D15)

class CommunicationSerial
{
public:
    CommunicationSerial();
    void begin();

    inline char serialControlMotor(Motor *motor);
    inline char wirelessControlMotor(Motor *motor);

    inline void serialSensorDataTX(Sensor *sensor);
    inline void wirelessSensorDataTX(Sensor *sensor);

private:
    inline void _motorControl(char data);
    // inline String _commandToString(char data);
};

CommunicationSerial::CommunicationSerial() {}

void CommunicationSerial::begin()
{
    SR.begin(115200);
    WR.begin(9600);
}

inline char CommunicationSerial::serialControlMotor(Motor *motor)
{
    static char buffer;
    if (SR.available())
    {
        buffer = SR.read();
        SR.flush();
        motor->motorControl(buffer);
    }
    return buffer;
}

inline char CommunicationSerial::wirelessControlMotor(Motor *motor)
{
    static char buffer;
    if (WR.available())
    {
        buffer = WR.read();
        WR.flush();
        motor->motorControl(buffer);
    }
    return buffer;
}

inline void CommunicationSerial::serialSensorDataTX(Sensor *sensor)
{
    SR.print("LL=");
    SR.print(sensor->getLightL());
    SR.print(",LR=");
    SR.print(sensor->getLightR());
    SR.print(",DT=");
    SR.print(sensor->getDistance());
    SR.print(",AX=");
    SR.print(sensor->getAngleX());
    SR.print(",AZ=");
    SR.print(sensor->getAngleZ());
    SR.println("");
}

inline void CommunicationSerial::wirelessSensorDataTX(Sensor *sensor)
{
    WR.print("LL=");
    WR.print(sensor->getLightL());
    WR.print(",LR=");
    WR.print(sensor->getLightR());
    WR.print(",DT=");
    WR.print(sensor->getDistance());
    WR.print(",AX=");
    WR.print(sensor->getAngleX());
    WR.print(",AZ=");
    WR.print(sensor->getAngleZ());
    WR.println("");
}


// inline String CommunicationSerial::_commandToString(char data)
// {
//     switch (data)
//     {
//     case 'A':
//         return "Forward";
//     case 'B':
//         return "Forward Right";
//     case 'C':
//         return "Rotate Right";
//     case 'D':
//         return "Backward Right";
//     case 'E':
//         return "Backward";
//     case 'F':
//         return "Backward Left";
//     case 'G':
//         return "Rotate Left";
//     case 'H':
//         return "Forward Left";
//     case 'L':
//         return "+170 PWM";
//     case 'M':
//         return "-170 PWM";
//     case 'Z':
//         return "Stop";
//     case 'z':
//         return "Stop";
//     case 'd':
//         return "Left";
//     case 'b':
//         return "Right";
//     default:
//         return "Invalid Command";
//     }
// }

#endif