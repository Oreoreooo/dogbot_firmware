#ifndef __MOTOR_ENCODER_HPP__
#define __MOTOR_ENCODER_HPP__

#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Arduino.h>
#include <Encoder.h>
#include "Motor.hpp"
#include <AutoPID.h>

#define ECDAA 18 // Motor A Encoder PIN A
#define ECDAB 31 // Motor A Encoder PIN B
#define ECDBA 19 // Motor B Encoder PIN A
#define ECDBB 38 // Motor B Encoder PIN B
#define ECDCA 3  // Motor C Encoder PIN A
#define ECDCB 49 // Motor C Encoder PIN B
#define ECDDA 2  // Motor D Encoder PIN A
#define ECDDB A1 // Motor D Encoder PIN B

#define SPEED_MAX 255
#define SPEED_MIN -255

#define AMPLIFIER 1

class MotorController
{
public:
    MotorController(Motor *motor);

    inline void controlByCommand(char command);
    inline void run();

private:
    Motor *_motor;
    uint8_t _motor_pwm = 255;

    Encoder _ECDA{ECDAA, ECDAB};
    Encoder _ECDB{ECDBA, ECDBB};
    Encoder _ECDC{ECDCA, ECDCB};
    Encoder _ECDD{ECDDA, ECDDB};

    volatile int32_t _ECD_A;
    volatile int32_t _prev_ECD_A;
    volatile int32_t _ECD_B;
    volatile int32_t _prev_ECD_B;
    volatile int32_t _ECD_C;
    volatile int32_t _prev_ECD_C;
    volatile int32_t _ECD_D;
    volatile int32_t _prev_ECD_D;

    double _curr_time = 0;
    double _prev_time = 0;

    double _v_A;
    double _v_B;
    double _v_C;
    double _v_D;

    double _SP_A;
    double _SP_B;
    double _SP_C;
    double _SP_D;

    double _PWM_A;
    double _PWM_B;
    double _PWM_C;
    double _PWM_D;

    double _kP = 50;
    double _kI = 30.0;
    double _kD = 20.0;

    double _interval = 0;

    AutoPID _PID_A{&_v_A, &_PWM_A, &_SP_A, SPEED_MAX, SPEED_MIN, _kP, _kI, _kD};
    AutoPID _PID_B{&_v_B, &_PWM_B, &_SP_B, SPEED_MAX, SPEED_MIN, _kP, _kI, _kD};
    AutoPID _PID_C{&_v_C, &_PWM_C, &_SP_C, SPEED_MAX, SPEED_MIN, _kP, _kI, _kD};
    AutoPID _PID_D{&_v_D, &_PWM_D, &_SP_D, SPEED_MAX, SPEED_MIN, _kP, _kI, _kD};

    inline void _updateFeedback();
};

MotorController::MotorController(Motor *motor) : _motor(motor) {}

inline void MotorController::run()
{
    _updateFeedback();

    _curr_time = micros();
    _interval = _curr_time - _prev_time;

    _v_A = (_ECD_A - _prev_ECD_A) ;
    _v_B = (_ECD_B - _prev_ECD_B) ;
    _v_C = (_ECD_C - _prev_ECD_C) ;
    _v_D = (_ECD_D - _prev_ECD_D) ;

    _prev_time = _curr_time;
    _prev_ECD_A = _ECD_A;
    _prev_ECD_B = _ECD_B;
    _prev_ECD_C = _ECD_C;
    _prev_ECD_D = _ECD_D;

    Serial.print(_interval);
    Serial.print(",");
    Serial.print(_v_A);
    Serial.print(",");
    Serial.print(_v_B);
    Serial.print(",");
    Serial.print(_v_C);
    Serial.print(",");
    Serial.println(_v_D);

    // _PID_A.run();
    // _PID_B.run();
    // _PID_C.run();
    // _PID_D.run();
    // _motor->MOVE(_pwm[0], _pwm[1], _pwm[2], _pwm[3]);
}

inline void MotorController::_updateFeedback()
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
        _motor_pwm = constrain(_motor_pwm + 5, MIN_PWM, MAX_PWM);
        _motor->setPWM(_motor_pwm, _motor_pwm, _motor_pwm, _motor_pwm);
        break;
    case 'M':
        _motor_pwm = constrain(_motor_pwm - 5, MIN_PWM, MAX_PWM);
        _motor->setPWM(_motor_pwm, _motor_pwm, _motor_pwm, _motor_pwm);
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

#endif