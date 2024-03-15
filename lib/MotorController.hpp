#ifndef __MOTOR_ENCODER_HPP__
#define __MOTOR_ENCODER_HPP__

#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Arduino.h>
#include <Encoder.h>
#include "Motor.hpp"

#define ECDAA 18 // Motor A Encoder PIN A
#define ECDAB 31 // Motor A Encoder PIN B
#define ECDBA 19 // Motor B Encoder PIN A
#define ECDBB 38 // Motor B Encoder PIN B
#define ECDCA 3  // Motor C Encoder PIN A
#define ECDCB 49 // Motor C Encoder PIN B
#define ECDDA 2  // Motor D Encoder PIN A
#define ECDDB A1 // Motor D Encoder PIN B

#define PWM_MAX 255
#define PWM_MIN 0

class MotorController
{
public:
    MotorController(Motor *motor);

    inline void controlByCommand(char command);
    inline void run();

private:
    Motor *_motor;

    Encoder _ECDA{ECDAA, ECDAB};
    Encoder _ECDB{ECDBA, ECDBB};
    Encoder _ECDC{ECDCA, ECDCB};
    Encoder _ECDD{ECDDA, ECDDB};

    double _kP = 1;
    double _kI = 0;
    double _kD = 0;

    double _lastStep = 0;
    double _timeStep = 0;

    double _ECD_A;
    double _ECD_B;
    double _ECD_C;
    double _ECD_D;

    double _PWM_A;
    double _PWM_B;
    double _PWM_C;
    double _PWM_D;

    double _integral_A;
    double _integral_B;
    double _integral_C;
    double _integral_D;

    double _prev_error_A;
    double _prev_error_B;
    double _prev_error_C;
    double _prev_error_D;

    inline void _update();
};

MotorController::MotorController(Motor *motor) : _motor(motor)
{
    _PWM_D = 127.0;
    _timeStep = 10;
}

inline void MotorController::run()
{
    unsigned long _dT = millis() - _lastStep;
    if (_dT >= _timeStep)
    {
        _lastStep = millis();
        _update();

        double _error_A = _ECD_D - _ECD_A;
        double _error_B = _ECD_D - _ECD_B;
        double _error_C = _ECD_D - _ECD_C;

        _integral_A += (_error_A + _prev_error_A) / 2 * _dT;
        _integral_B += (_error_B + _prev_error_B) / 2 * _dT;
        _integral_C += (_error_C + _prev_error_C) / 2 * _dT;

        double _derivative_A = (_error_A - _prev_error_A) / _dT;
        double _derivative_B = (_error_B - _prev_error_B) / _dT;
        double _derivative_C = (_error_C - _prev_error_C) / _dT;

        _prev_error_A = _error_A;
        _prev_error_B = _error_B;
        _prev_error_C = _error_C;

        _PWM_A = constrain(_kP * _error_A + _kI * _integral_A + _kD * _derivative_A, PWM_MIN, PWM_MAX);
        _PWM_B = constrain(_kP * _error_B + _kI * _integral_B + _kD * _derivative_B, PWM_MIN, PWM_MAX);
        _PWM_C = constrain(_kP * _error_C + _kI * _integral_C + _kD * _derivative_C, PWM_MIN, PWM_MAX);

        Serial.print(_PWM_A);
        Serial.print(",");
        Serial.print(_PWM_B);
        Serial.print(",");
        Serial.print(_PWM_C);
        Serial.print(",");
        Serial.println(_PWM_D);

        Serial3.print(_kP);
        Serial3.print(",");
        Serial3.print(_kI);
        Serial3.print(",");
        Serial3.println(_kD);

        _motor->MOVE(_PWM_A, _PWM_B, _PWM_C, _PWM_D);
    }
}

inline void MotorController::_update()
{
    _ECD_A = -_ECDA.read();
    _ECD_B = _ECDB.read();
    _ECD_C = -_ECDC.read();
    _ECD_D = _ECDD.read();
}

inline void MotorController::controlByCommand(char command)
{
    switch (command)
    {
    case 'Q':
        _kP = _kP + 0.01;
    case 'W':
        _kI = _kI + 0.01;
    case 'E':
        _kD = _kD + 0.01;
    case 'A':
        _kP = _kP - 0.01;
    case 'S':
        _kI = _kI - 0.01;
    case 'D':
        _kD = _kD - 0.01;
    case '+':
        _PWM_D = constrain(_PWM_D + 5, 0, 255);
        break;
    case '-':
        _PWM_D = constrain(_PWM_D - 5, 0, 255);
        break;
    case 'Z':
        _motor->STOP();
        break;
    case 'z':
        _motor->STOP();
        break;
    default:
        break;
    }
}

#endif