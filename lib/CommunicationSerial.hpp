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
    CommunicationSerial(Display_SSD1306 *display, Motor *motor, Sensor *sensor);
    void begin();

    char serialControlRX();
    char wirelessControlRX();

    void displaySensorData();

    void serialSensorDataTX();
    // void wirelessSensorDataTX();

private:
    void _wheelControl(char data);
    String _commandToString(char data);

    Display_SSD1306 *_display;
    Motor *_motor;
    Sensor *_sensor;

    int _motor_pwm;
};

CommunicationSerial::CommunicationSerial(Display_SSD1306 *display, Motor *motor, Sensor *sensor) : _display(display), _motor(motor), _sensor(sensor), _motor_pwm(255) {}

void CommunicationSerial::begin()
{
    SR.begin(115200);
    WR.begin(9600);
}

char CommunicationSerial::serialControlRX()
{
    static char data;
    if (SR.available())
    {
        data = SR.read();
        SR.flush();
        // _display->show(_commandToString(data));
        _wheelControl(data);
    }
    return data;
}

char CommunicationSerial::wirelessControlRX()
{
    static char data;
    if (WR.available())
    {
        data = WR.read();
        WR.flush();
        // _display->show(_commandToString(data));
        _wheelControl(data);
    }
    return data;
}

void CommunicationSerial::displaySensorData()
{
    static char data[50];
    sprintf(data, "L: %-4d  R: %-4d\nD: %-6.1f\nX: %-5.2f  Z: %-5.2f", _sensor->getLightL(), _sensor->getLightR(), _sensor->getDistance(), _sensor->getAngleX(), _sensor->getAngleZ());
    _display->show(data);
}

void CommunicationSerial::serialSensorDataTX()
{
    SR.print("LL=");
    SR.print(_sensor->getLightL());
    SR.print(",LR=");
    SR.print(_sensor->getLightR());
    SR.print(",DT=");
    SR.print(_sensor->getDistance());
    SR.print(",AX=");
    SR.print(_sensor->getAngleX());
    SR.print(",AZ=");
    SR.print(_sensor->getAngleZ());
    SR.println("");
}

// void CommunicationSerial::wirelessSensorDataTX()
// {
//     WR.print("LL=");
//     WR.print(_sensor->getLightL());
//     WR.print(",LR=");
//     WR.print(_sensor->getLightR());
//     WR.print(",DT=");
//     WR.print(_sensor->getDistance());
//     WR.print(",AX=");
//     WR.print(_sensor->getAngleX());
//     WR.print(",AZ=");
//     WR.print(_sensor->getAngleZ());
//     WR.println("");
// }

void CommunicationSerial::_wheelControl(char data)
{
    switch (data)
    {
    case 'A':
        _motor->ADVANCE(_motor_pwm);
        break;
    case 'B':
        _motor->ADVANCE_RIGHT(_motor_pwm);
        break;
    case 'C':
        _motor->ROTATE_CW(_motor_pwm);
        break;
    case 'D':
        _motor->BACK_RIGHT(_motor_pwm);
        break;
    case 'E':
        _motor->BACK(_motor_pwm);
        break;
    case 'F':
        _motor->BACK_LEFT(_motor_pwm);
        break;
    case 'G':
        _motor->ROTATE_CCW(_motor_pwm);
        break;
    case 'H':
        _motor->ADVANCE_LEFT(_motor_pwm);
        break;
    case 'L':
        _motor_pwm = constrain(_motor_pwm + 25, MAX_PWM, MIN_PWM);
        break;
    case 'M':
        _motor_pwm = constrain(_motor_pwm - 25, MAX_PWM, MIN_PWM);
        break;
    case 'Z':
        _motor->STOP();
        break;
    case 'z':
        _motor->STOP();
        break;
    case 'd':
        _motor->LEFT(_motor_pwm);
        break;
    case 'b':
        _motor->RIGHT(_motor_pwm);
        break;
    default:
        break;
    }
}

String CommunicationSerial::_commandToString(char data)
{
    switch (data)
    {
    case 'A':
        return "Forward";
    case 'B':
        return "Forward Right";
    case 'C':
        return "Rotate Right";
    case 'D':
        return "Backward Right";
    case 'E':
        return "Backward";
    case 'F':
        return "Backward Left";
    case 'G':
        return "Rotate Left";
    case 'H':
        return "Forward Left";
    case 'L':
        return "+170 PWM";
    case 'M':
        return "-170 PWM";
    case 'Z':
        return "Stop";
    case 'z':
        return "Stop";
    case 'd':
        return "Left";
    case 'b':
        return "Right";
    default:
        return "Invalid Command";
    }
}

#endif