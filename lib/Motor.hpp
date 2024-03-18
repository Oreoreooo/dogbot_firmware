#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__

#include <Arduino.h>
#include <MPU6050_light.h>

#define DIRA1 34 // Motor A Direction +
#define DIRA2 35 // Motor A Direction -
#define DIRB1 36 // Motor B Direction +
#define DIRB2 37 // Motor B Direction -
#define DIRC1 43 // Motor C Direction +
#define DIRC2 42 // Motor C Direction -
#define DIRD1 A5 // Motor D Direction +
#define DIRD2 A4 // Motor D Direction -
#define PWMA 12  // Motor A PWM
#define PWMB 8   // Motor B PWM
#define PWMC 6   // Motor C PWM
#define PWMD 5   // Motor D PWM

#define MOTOR_FORWARD 1
#define MOTOR_BACKWARD -1
#define MOTOR_STOP 0

// motor tuning factor, use measure.ino to record serial data as .csv to calculate average the value
#define MOTOR_TURN_A 7.928667
#define MOTOR_TURN_B 8.404883
#define MOTOR_TURN_C 8.049408
#define MOTOR_TURN_D 6.803542
#define MIN_MOTOR_TURN 6.803542
#define BALANCE_FACTOR_A (MIN_MOTOR_TURN / MOTOR_TURN_A)
#define BALANCE_FACTOR_B (MIN_MOTOR_TURN / MOTOR_TURN_B)
#define BALANCE_FACTOR_C (MIN_MOTOR_TURN / MOTOR_TURN_C)
#define BALANCE_FACTOR_D (MIN_MOTOR_TURN / MOTOR_TURN_D)

#define MAX_PWM 191
#define MIN_PWM 0

#define TIME_STEP 25

#define MAX_PWM_ROTATE 60

const float _kP = 1.0;

float _init_angle = 0.0;
float _target_angle = 0.0;
const float _angle_threshold = 1.0;

int _PWM_A = 0;
int _PWM_B = 0;
int _PWM_C = 0;
int _PWM_D = 0;

bool _is_driving = false;
bool _is_stopped = true;

unsigned long _prev_time = 0;

extern MPU6050 mpu;

inline void setupMotor()
{
    pinMode(DIRA1, OUTPUT);
    pinMode(DIRA2, OUTPUT);
    pinMode(DIRB1, OUTPUT);
    pinMode(DIRB2, OUTPUT);
    pinMode(DIRC1, OUTPUT);
    pinMode(DIRC2, OUTPUT);
    pinMode(DIRD1, OUTPUT);
    pinMode(DIRD2, OUTPUT);
    _prev_time = millis();
    _init_angle = mpu.getAngleZ();
    _target_angle = _init_angle;
}

inline void _setWheelDirection(int direction, int postive, int negative)
{
    if (direction > 0)
    {
        digitalWrite(postive, LOW);
        digitalWrite(negative, HIGH);
    }
    else if (direction < 0)
    {
        digitalWrite(postive, HIGH);
        digitalWrite(negative, LOW);
    }
    else
    {
        digitalWrite(postive, LOW);
        digitalWrite(negative, LOW);
    }
}

inline void _setDirection(int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _setWheelDirection(DIR_A, DIRA1, DIRA2);
    _setWheelDirection(DIR_B, DIRB1, DIRB2);
    _setWheelDirection(DIR_C, DIRC1, DIRC2);
    _setWheelDirection(DIR_D, DIRD1, DIRD2);
}

inline void _writePWM(int PWM_A, int PWM_B, int PWM_C, int PWM_D)
{
    analogWrite(PWMA, _PWM_A);
    analogWrite(PWMB, _PWM_B);
    analogWrite(PWMC, _PWM_C);
    analogWrite(PWMD, _PWM_D);
}

inline void _adjustPWM(int i_a, int i_b, int i_c, int i_d)
{
    _PWM_A = constrain(_PWM_A + i_a, MIN_PWM, MAX_PWM);
    _PWM_B = constrain(_PWM_B + i_b, MIN_PWM, MAX_PWM);
    _PWM_C = constrain(_PWM_C + i_c, MIN_PWM, MAX_PWM);
    _PWM_D = constrain(_PWM_D + i_d, MIN_PWM, MAX_PWM);
    _writePWM(_PWM_A, _PWM_B, _PWM_C, _PWM_D);
}

inline void _drive(int MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _is_stopped = false;
    _is_driving = true;

    _PWM_A = constrain(BALANCE_FACTOR_A * MOTOR_PWM, MIN_PWM, MAX_PWM);
    _PWM_B = constrain(BALANCE_FACTOR_B * MOTOR_PWM, MIN_PWM, MAX_PWM);
    _PWM_C = constrain(BALANCE_FACTOR_C * MOTOR_PWM, MIN_PWM, MAX_PWM);
    _PWM_D = constrain(BALANCE_FACTOR_D * MOTOR_PWM, MIN_PWM, MAX_PWM);
    _setDirection(DIR_A, DIR_B, DIR_C, DIR_D);
}

//    ↓A-----B↓
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
inline void BACK(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD);
}

//    ↑A-----B↑
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
inline void ADVANCE(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD);
}
//    =A-----B↑
//     |   ↖ |
//     | ↖   |
//    ↑C-----D=
inline void ADVANCE_LEFT(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_STOP, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_STOP);
}

//    ↓A-----B↑
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
inline void LEFT(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
}
//    ↓A-----B=
//     | ↙   |
//     |   ↙ |
//    =C-----D↓
inline void BACK_LEFT(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_BACKWARD);
}
//    ↑A-----B=
//     | ↗   |
//     |   ↗ |
//    =C-----D↑
inline void ADVANCE_RIGHT(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_FORWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_FORWARD);
}
//    ↑A-----B↓
//     |  →  |
//     |  →  |
//    ↓C-----D↑
inline void RIGHT(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
}
//    =A-----B↓
//     |   ↘ |
//     | ↘   |
//    ↓C-----D=
inline void BACK_RIGHT(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_STOP, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_STOP);
}

//    ↑A-----B↓
//     | ↗ ↘ |
//     | ↖ ↙ |
//    ↑C-----D↓
inline void ROTATE_CW(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
}

//    ↓A-----B↑
//     | ↙ ↖ |
//     | ↘ ↗ |
//    ↓C-----D↑
inline void ROTATE_CCW(int MOTOR_PWM)
{
    _drive(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
}

//    =A-----B=
//     |  =  |
//     |  =  |
//    =C-----D=
inline void STOP()
{
    _drive(0, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP);
    _is_stopped = true;
    _is_driving = true;
}

//    ↓A-----B↑  angle > _target_angle  ↑A-----B↓
//     | ↙ ↖ |                           | ↗ ↘ |
//     | ↘ ↗ |                           | ↖ ↙ |
//    ↓C-----D↑                         ↑C-----D↓  angle < _target_angle
inline void ROTATE_BY(int MOTOR_PWM, int increment_angle)
{
    _is_stopped = false;
    _is_driving = false;
    _target_angle += increment_angle;
}

//    ↓A-----B↑  angle > _target_angle  ↑A-----B↓
//     | ↙ ↖ |                           | ↗ ↘ |
//     | ↘ ↗ |                           | ↖ ↙ |
//    ↓C-----D↑                         ↑C-----D↓  angle < _target_angle
inline void ROTATE_TO(int MOTOR_PWM, int target_angle)
{
    _is_stopped = false;
    _is_driving = false;
    _target_angle = _init_angle + target_angle;
}

inline void _adjust()
{
    float error = _target_angle - mpu.getAngleZ();
    if (error > -_angle_threshold && error < _angle_threshold)
    {
        _writePWM(_PWM_A, _PWM_B, _PWM_C, _PWM_D);
        return;
    }
    int output = round(_kP * error);
    _writePWM(_PWM_A - output, _PWM_B + output, _PWM_C - output, _PWM_D + output);
}

void _rotate()
{
    float angleZ = mpu.getAngleZ();
    float gyroZ = mpu.getGyroZ();
    float error = _target_angle - angleZ;
    float delta = abs(error);

    if (delta <= _angle_threshold)
    {
        STOP();
        return;
    }

    if (_target_angle < angleZ)
    {
        _setDirection(MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
    }
    else
    {
        _setDirection(MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
    }

    int output = round(_kP * delta);

    if (output - gyroZ == 0)
    {
        return;
    }

    if (output < gyroZ)
    {
        _adjustPWM(-1, -1, -1, -1);
    }
    else
    {
        _adjustPWM(1, 1, 1, 1);
    }
}

inline void balance()
{
    if (millis() - _prev_time >= TIME_STEP && !_is_stopped)
    {
        _prev_time = millis();
        if (_is_driving)
        {
            _adjust();
        }
        else
        {
            _rotate();
        }
    }
}

inline String command(char command)
{
    static int _motor_pwm = 100;
    switch (command)
    {
    case 'A':
        ADVANCE(_motor_pwm);
        return "Forward";
    case 'B':
        ADVANCE_RIGHT(_motor_pwm);
        return "Forward Right";
    case 'b':
        RIGHT(_motor_pwm);
        return "Right";
        break;
    case 'C':
        ROTATE_CW(_motor_pwm);
        return "Rotate Right";
    case 'D':
        BACK_RIGHT(_motor_pwm);
        return "Backward Right";
    case 'd':
        LEFT(_motor_pwm);
        return "Left";
    case 'E':
        BACK(_motor_pwm);
        return "Back";
    case 'F':
        BACK_LEFT(_motor_pwm);
        return "Backward Left";
    case 'G':
        ROTATE_CCW(_motor_pwm);
        return "Rotate Left";
    case 'H':
        ADVANCE_LEFT(_motor_pwm);
        return "Forward Left";
    case 'L':
        _motor_pwm = constrain(_motor_pwm + 5, MIN_PWM, MAX_PWM);
        return "+5 PWM";
    case 'M':
        _motor_pwm = constrain(_motor_pwm - 5, MIN_PWM, MAX_PWM);
        return "-5 PWM";
    case 'Z':
        STOP();
        return "Stop";
    case 'z':
        STOP();
        return "Stop";
    default:
        return "Invalid Command";
    }
}

#endif
