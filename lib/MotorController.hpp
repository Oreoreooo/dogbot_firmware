#ifndef __MOTOR_CONTROLLER_HPP__
#define __MOTOR_CONTROLLER_HPP__

#include <Arduino.h>
#include <MPU6050_light.h>
#include "Sensor.hpp"

#define MOTOR_TURN_A 7.928667
#define MOTOR_TURN_B 8.404883
#define MOTOR_TURN_C 8.049408
#define MOTOR_TURN_D 6.803542
#define MIN_MOTOR_TURN 6.803542
#define BALANCE_FACTOR_A (MIN_MOTOR_TURN / MOTOR_TURN_A)
#define BALANCE_FACTOR_B (MIN_MOTOR_TURN / MOTOR_TURN_B)
#define BALANCE_FACTOR_C (MIN_MOTOR_TURN / MOTOR_TURN_C)
#define BALANCE_FACTOR_D (MIN_MOTOR_TURN / MOTOR_TURN_D)

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

#define ROTATE_STEADEY_GOAL 20

#define MAX_PWM 75
#define MIN_PWM 5

#define MIN_ANGULAR_SPEED 0.00
#define MAX_ANGULAR_SPEED 50.0

#define ANGLE_DELTA_THRESHOLD 0.75   // degree
#define DISTANCE_DELTA_THRESHOLD 1.0 // cm

#define TIME_STEP 20

extern MPU6050 mpu;
extern Sensor sensor;

class MotorController
{
public:
    MotorController();
    void begin();

    inline void BACK(int MOTOR_PWM, bool rectify = false);
    inline void ADVANCE(int MOTOR_PWM, bool rectify = false);
    inline void ADVANCE_LEFT(int MOTOR_PWM, bool rectify = false);
    inline void LEFT(int MOTOR_PWM, bool rectify = false);
    inline void BACK_LEFT(int MOTOR_PWM, bool rectify = false);
    inline void ADVANCE_RIGHT(int MOTOR_PWM, bool rectify = false);
    inline void RIGHT(int MOTOR_PWM, bool rectify = false);
    inline void BACK_RIGHT(int MOTOR_PWM, bool rectify = false);
    inline void ROTATE_CW(int MOTOR_PWM);
    inline void ROTATE_CCW(int MOTOR_PWM);
    inline void ROTATE_TO(float angle);
    inline void STOP();
    inline void ROTATE_TO_ABSOLUTE(float angle);

    inline void balance();

    inline bool hasStopped() { return _is_stopped; }
    inline void setInitAngle(float angle) { _init_angle = angle; }

    inline String command(char command);

private:
    inline void _adjustDrive();
    inline void _adjustRotate();

    inline void _setWheelDirection(int direction, int positive, int negative);
    inline void _setDirection(int DIR_A, int DIR_B, int DIR_C, int DIR_D);
    inline void _writePWM(int pwm_a, int pwm_b, int pwm_c, int pwm_D);
    inline void _adjustRotatePWM(int pwm_a, int pwm_b, int pwm_c, int pwm_d);
    inline void _setupDrive(bool rectify, int MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D);
    inline void _setupRotate(float target_angle);

    int _PWM_A;
    int _PWM_B;
    int _PWM_C;
    int _PWM_D;
    int _rotate_steady_counter;

    bool _is_driving;
    bool _is_stopped;

    unsigned long _prev_time;

    const float _kP = 1.25;
    const float _kI = 0.001;

    float _integral;

    float _init_angle;
    float _target_angle;

    bool _rectify;
};

MotorController::MotorController()
{
    _PWM_A = 0;
    _PWM_B = 0;
    _PWM_C = 0;
    _PWM_D = 0;
    _rotate_steady_counter = 0;
    _is_driving = false;
    _is_stopped = true;
    _prev_time = 0;
    _integral = 0.0;
    _init_angle = 0.0;
    _target_angle = 0.0;
}

inline void MotorController::begin()
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

inline void MotorController::_setWheelDirection(int direction, int positive, int negative)
{
    digitalWrite(positive, direction < 0 ? HIGH : LOW);
    digitalWrite(negative, direction > 0 ? HIGH : LOW);
}

inline void MotorController::_setDirection(int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _setWheelDirection(DIR_A, DIRA1, DIRA2);
    _setWheelDirection(DIR_B, DIRB1, DIRB2);
    _setWheelDirection(DIR_C, DIRC1, DIRC2);
    _setWheelDirection(DIR_D, DIRD1, DIRD2);
}

inline void MotorController::_writePWM(int pwm_a, int pwm_b, int pwm_c, int pwm_D)
{
    int PWM_A = constrain(pwm_a, MIN_PWM, MAX_PWM);
    int PWM_B = constrain(pwm_b, MIN_PWM, MAX_PWM);
    int PWM_C = constrain(pwm_c, MIN_PWM, MAX_PWM);
    int PWM_D = constrain(pwm_D, MIN_PWM, MAX_PWM);
    analogWrite(PWMA, PWM_A);
    analogWrite(PWMB, PWM_B);
    analogWrite(PWMC, PWM_C);
    analogWrite(PWMD, PWM_D);
}

inline void MotorController::_adjustRotatePWM(int pwm_a, int pwm_b, int pwm_c, int pwm_d)
{
    _PWM_A = constrain(pwm_a, MIN_PWM, MAX_PWM);
    _PWM_B = constrain(pwm_b, MIN_PWM, MAX_PWM);
    _PWM_C = constrain(pwm_c, MIN_PWM, MAX_PWM);
    _PWM_D = constrain(pwm_d, MIN_PWM, MAX_PWM);
    _writePWM(pwm_a, pwm_b, pwm_c, pwm_d);
}

inline void MotorController::_setupDrive(bool rectify, int MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _rectify = rectify;
    _is_stopped = false;
    _is_driving = true;
    _integral = 0.0;
    _PWM_A = constrain(MOTOR_PWM * BALANCE_FACTOR_A, MIN_PWM, MAX_PWM);
    _PWM_B = constrain(MOTOR_PWM * BALANCE_FACTOR_B, MIN_PWM, MAX_PWM);
    _PWM_C = constrain(MOTOR_PWM * BALANCE_FACTOR_C, MIN_PWM, MAX_PWM);
    _PWM_D = constrain(MOTOR_PWM * BALANCE_FACTOR_D, MIN_PWM, MAX_PWM);
    _setDirection(DIR_A, DIR_B, DIR_C, DIR_D);
}

inline void MotorController::_setupRotate(float angle)
{
    _target_angle = angle;
    _integral = 0.0;
    _is_stopped = false;
    _is_driving = false;
    _PWM_A = 0;
    _PWM_B = 0;
    _PWM_C = 0;
    _PWM_D = 0;
    _writePWM(0, 0, 0, 0);
}

//    ↓A-----B↓
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
inline void MotorController::BACK(int MOTOR_PWM, bool rectify = false)
{
    _setupDrive(rectify, MOTOR_PWM, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD);
}

//    ↑A-----B↑
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
inline void MotorController::ADVANCE(int MOTOR_PWM, bool rectify = false)
{
    _setupDrive(rectify, MOTOR_PWM, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD);
}
//    =A-----B↑
//     |   ↖ |
//     | ↖   |
//    ↑C-----D=
inline void MotorController::ADVANCE_LEFT(int MOTOR_PWM, bool rectify = false)
{
    _setupDrive(false, MOTOR_PWM, MOTOR_STOP, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_STOP);
}

//    ↓A-----B↑
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
inline void MotorController::LEFT(int MOTOR_PWM, bool rectify = false)
{
    _setupDrive(rectify, MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
}
//    ↓A-----B=
//     | ↙   |
//     |   ↙ |
//    =C-----D↓
inline void MotorController::BACK_LEFT(int MOTOR_PWM, bool rectify = false)
{
    _setupDrive(rectify, MOTOR_PWM, MOTOR_BACKWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_BACKWARD);
}
//    ↑A-----B=
//     | ↗   |
//     |   ↗ |
//    =C-----D↑
inline void MotorController::ADVANCE_RIGHT(int MOTOR_PWM, bool rectify = false)
{
    _setupDrive(rectify, MOTOR_PWM, MOTOR_FORWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_FORWARD);
}
//    ↑A-----B↓
//     |  →  |
//     |  →  |
//    ↓C-----D↑
inline void MotorController::RIGHT(int MOTOR_PWM, bool rectify = false)
{
    _setupDrive(rectify, MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
}
//    =A-----B↓
//     |   ↘ |
//     | ↘   |
//    ↓C-----D=
inline void MotorController::BACK_RIGHT(int MOTOR_PWM, bool rectify = false)
{
    _setupDrive(rectify, MOTOR_PWM, MOTOR_STOP, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_STOP);
}

//    ↑A-----B↓
//     | ↗ ↘ |
//     | ↖ ↙ |
//    ↑C-----D↓
inline void MotorController::ROTATE_CW(int MOTOR_PWM)
{
    _setupDrive(false, MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
}

//    ↓A-----B↑
//     | ↙ ↖ |
//     | ↘ ↗ |
//    ↓C-----D↑
inline void MotorController::ROTATE_CCW(int MOTOR_PWM)
{
    _setupDrive(false, MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
}

//    ↓A-----B↑  angle > _target_angle  ↑A-----B↓
//     | ↙ ↖ |                           | ↗ ↘ |
//     | ↘ ↗ |                           | ↖ ↙ |
//    ↓C-----D↑                         ↑C-----D↓  angle < _target_angle
inline void MotorController::ROTATE_TO(float angle)
{
    _setupRotate(angle + _init_angle);
}

//    =A-----B=
//     |  =  |
//     |  =  |
//    =C-----D=
inline void MotorController::STOP()
{
    _setDirection(MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP);
    _PWM_A = 0;
    _PWM_B = 0;
    _PWM_C = 0;
    _PWM_D = 0;
    _writePWM(0, 0, 0, 0);
    _is_stopped = true;
}

void MotorController::_adjustDrive()
{
    if (_rectify)
    {
        float error = sensor.getDistanceError();
        _integral += error * _kI;
        if (fabs(error) <= DISTANCE_DELTA_THRESHOLD)
        {
            _writePWM(_PWM_A, _PWM_B, _PWM_C, _PWM_D);
            return;
        }
        int output = round(_kP * error + _integral);
        if (error > 0)
        {
            _writePWM(_PWM_A + output, _PWM_B, _PWM_C + output, _PWM_D);
        }
        else
        {
            _writePWM(_PWM_A, _PWM_B - output, _PWM_C, _PWM_D - output);
        }
    }
    else
    {
    float error = _target_angle - mpu.getAngleZ();
    _integral += error * _kI;
    if (fabs(error) <= ANGLE_DELTA_THRESHOLD)
    {
        _writePWM(_PWM_A, _PWM_B, _PWM_C, _PWM_D);
        return;
    }
    int output = round(_kP * error + _integral);
    _writePWM(_PWM_A - output, _PWM_B + output, _PWM_C - output, _PWM_D + output);
}

// https://github.com/bz36912/driving_straight_robot_car/blob/main/driving_straight_MPU6050_v1.ino
inline void MotorController::_adjustRotate()
{
    float angle = mpu.getAngleZ();
    float angular_speed = fabs(mpu.getGyroZ());
    float delta = fabs(_target_angle - angle);

    if (delta <= ANGLE_DELTA_THRESHOLD)
    {
        if (++_rotate_steady_counter > ROTATE_STEADEY_GOAL)
        {
            STOP();
        }
        return;
    }

    _rotate_steady_counter = 0;

    if (_target_angle < angle)
    {
        _setDirection(MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
    }
    else
    {
        _setDirection(MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
    }

    float output = constrain(round(_kP * delta), MIN_ANGULAR_SPEED, MAX_ANGULAR_SPEED);

    if (round(output - angular_speed) == 0)
    {
        return;
    }

    if (output < angular_speed)
    {
        _adjustRotatePWM(_PWM_A - 1, _PWM_A - 1, _PWM_B - 1, _PWM_B - 1);
    }
    else
    {
        _adjustRotatePWM(_PWM_A + 1, _PWM_A + 1, _PWM_B + 1, _PWM_B + 1);
    }
}

inline void MotorController::balance()
{
    if (millis() - _prev_time < TIME_STEP || _is_stopped)
    {
        return;
    }
    _prev_time = millis();
    _is_driving ? _adjustDrive() : _adjustRotate();
}

inline String MotorController::command(char command)
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
        _motor_pwm = _motor_pwm;
        if (_motor_pwm > MAX_PWM)
        {
            _motor_pwm = MAX_PWM;
        }
        return "+5 PWM";
    case 'M':
        _motor_pwm = _motor_pwm - 5;
        if (_motor_pwm < MIN_PWM)
        {
            _motor_pwm = MIN_PWM;
        }
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
