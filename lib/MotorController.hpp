#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__

#include <Arduino.h>
#include <MPU6050_light.h>

// Define motor pins
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

// Direction Definition
#define MOTOR_FORWARD 1
#define MOTOR_BACKWARD -1
#define MOTOR_STOP 0

// motor tuning factor, use measure() to record serial data as .csv to calculate average the value
const float MOTOR_RPPWM_A = 7.928667;
const float MOTOR_RPPWM_B = 8.404883;
const float MOTOR_RPPWM_C = 8.049408;
const float MOTOR_RPPWM_D = 6.803542;
const float MIN_MOTOR_RPPWM = min(min(MOTOR_RPPWM_A, MOTOR_RPPWM_B), min(MOTOR_RPPWM_C, MOTOR_RPPWM_D));
const float BALANCE_FACTOR_A = MIN_MOTOR_RPPWM / MOTOR_RPPWM_A;
const float BALANCE_FACTOR_B = MIN_MOTOR_RPPWM / MOTOR_RPPWM_B;
const float MBALANCE_FACTOR_C = MIN_MOTOR_RPPWM / MOTOR_RPPWM_C;
const float BALANCE_FACTOR_D = MIN_MOTOR_RPPWM / MOTOR_RPPWM_D;

// PWM Definition
#define MAX_PWM 200
#define MIN_PWM 0

const float kP = 1.0;
const float kI = 0.0;

const float threshold = 1.0;

extern MPU6050 mpu;

class MotorController
{
public:
    MotorController(void);

    void begin();
    void balance();
    // void measure();

    void STOP();
    void BACK(int MOTOR_PWM);
    void ADVANCE(int MOTOR_PWM);
    void ADVANCE_LEFT(int MOTOR_PWM);
    void RIGHT(int MOTOR_PWM);
    void BACK_LEFT(int MOTOR_PWM);
    void ADVANCE_RIGHT(int MOTOR_PWM);
    void LEFT(int MOTOR_PWM);
    void BACK_RIGHT(int MOTOR_PWM);
    void ROTATE_BY(int MOTOR_PWM, int angle);
    void ROTATE_TO(int MOTOR_PWM, int target_angle);
    void ROTATE_CW(int MOTOR_PWM);
    void ROTATE_CCW(int MOTOR_PWM);

    String command(char command);

private:
    inline void _driveSetup();
    inline void _rotateSetup();

    inline void _drive(int MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D);
    inline void _adjust();
    inline void _rotate();

    inline void _setWheelDirection(int DIR, int positive, int negative);
    inline void _setDirection(int DIR_A, int DIR_B, int DIR_C, int DIR_D);

    inline void _setTargetPWM(int PWM_A, int PWM_B, int PWM_C, int PWM_D);
    inline void _setPWM(int PWM_A, int PWM_B, int PWM_C, int PWM_D);

    int _PWM_A_set;
    int _PWM_B_set;
    int _PWM_C_set;
    int _PWM_D_set;
    int _PWM_A_target;
    int _PWM_B_target;
    int _PWM_C_target;
    int _PWM_D_target;

    bool _is_driving;
    bool _is_stopped;

    float _integral;

    float _target_angle;

    unsigned long _prev_time;
    unsigned long _time_step;
};

MotorController::MotorController(void) {}

void MotorController::begin()
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
    _time_step = 10;
    STOP();
}

void MotorController::balance()
{
    if (millis() - _prev_time >= _time_step)
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

inline void MotorController::_adjust()
{
    float _error = _target_angle - mpu.getAngleZ();
    _integral += _error * kI;
    int _output = round(kP * _error + _integral);

    if (_error > threshold) // Left -> Right Slower
    {
        _setPWM(_PWM_A_target, _PWM_B_target - round(_output), _PWM_C_target, _PWM_D_target);
    }
    else if (_error < -threshold) // Right -> Left Slower
    {
        _setPWM(_PWM_A_target + round(_output), _PWM_B_target, _PWM_C_target, _PWM_D_target);
    }
}

inline void MotorController::_rotate()
{
    int _error = round(_target_angle - mpu.getAngleZ());

    if (abs(_error) <= threshold)
    {
        _setDirection(MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP);
        return;
    }

    int _angular_speed = abs(_error) > 30 ? 60 : abs(_error) * 2;

    if (mpu.getGyroZ() > _angular_speed)
    {
        _setPWM(--_PWM_A_target, --_PWM_B_target, --_PWM_C_target, --_PWM_D_target);
    }
    else
    {
        _setPWM(++_PWM_A_target, ++_PWM_B_target, ++_PWM_C_target, ++_PWM_D_target);
    }
    if (_error > threshold) // Turn Left
    {
        _setDirection(MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
    }
    else if (_error < -threshold) // Turn Right
    {
        _setDirection(MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
    }
}

inline void MotorController::_setWheelDirection(int direction, int postive, int negative)
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

inline void MotorController::_setDirection(int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _setWheelDirection(DIR_A, DIRA1, DIRA2);
    _setWheelDirection(DIR_B, DIRB1, DIRB2);
    _setWheelDirection(DIR_C, DIRC1, DIRC2);
    _setWheelDirection(DIR_D, DIRD1, DIRD2);
}

inline void MotorController::_setPWM(int PWM_A, int PWM_B, int PWM_C, int PWM_D)
{
    if (PWM_A == _PWM_A_set && PWM_B == _PWM_B_set && PWM_C == _PWM_C_set && PWM_D == _PWM_D_set)
    {
        return;
    }
    _PWM_A_set = constrain(PWM_A, MIN_PWM, MAX_PWM);
    _PWM_B_set = constrain(PWM_B, MIN_PWM, MAX_PWM);
    _PWM_C_set = constrain(PWM_C, MIN_PWM, MAX_PWM);
    _PWM_D_set = constrain(PWM_D, MIN_PWM, MAX_PWM);
    analogWrite(PWMA, PWM_A);
    analogWrite(PWMB, PWM_B);
    analogWrite(PWMC, PWM_C);
    analogWrite(PWMD, PWM_D);
}

inline void MotorController::_setTargetPWM(int PWM_A, int PWM_B, int PWM_C, int PWM_D)
{
    _PWM_A_target = constrain(PWM_A, MIN_PWM, MAX_PWM);
    _PWM_B_target = constrain(PWM_B, MIN_PWM, MAX_PWM);
    _PWM_C_target = constrain(PWM_C, MIN_PWM, MAX_PWM);
    _PWM_D_target = constrain(PWM_D, MIN_PWM, MAX_PWM);
}

// Speed is weighed PWM based on the slowest motor
inline void MotorController::_drive(int MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _setDirection(DIR_A, DIR_B, DIR_C, DIR_D);
    _setTargetPWM(round(MOTOR_PWM * BALANCE_FACTOR_A), round(MOTOR_PWM * BALANCE_FACTOR_B), round(MOTOR_PWM * MBALANCE_FACTOR_C), round(MOTOR_PWM * BALANCE_FACTOR_D));
}

inline void MotorController::_driveSetup()
{
    _is_stopped = false;
    _is_driving = true;
    _integral = 0;
    _target_angle = mpu.getAngleZ();
}

inline void MotorController::_rotateSetup()
{
    _setPWM(0, 0, 0, 0);
    _is_stopped = false;
    _is_driving = false;
}

//    ↓A-----B↓
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
void MotorController::BACK(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD);
}

//    ↑A-----B↑
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
void MotorController::ADVANCE(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD);
}
//    =A-----B↑
//     |   ↖ |
//     | ↖   |
//    ↑C-----D=
void MotorController::ADVANCE_LEFT(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_STOP, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_STOP);
}

//    ↓A-----B↑
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
void MotorController::LEFT(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
}
//    ↓A-----B=
//     | ↙   |
//     |   ↙ |
//    =C-----D↓
void MotorController::BACK_LEFT(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_BACKWARD);
}
//    ↑A-----B=
//     | ↗   |
//     |   ↗ |
//    =C-----D↑
void MotorController::ADVANCE_RIGHT(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_FORWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_FORWARD);
}
//    ↑A-----B↓
//     |  →  |
//     |  →  |
//    ↓C-----D↑
void MotorController::RIGHT(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
}
//    =A-----B↓
//     |   ↘ |
//     | ↘   |
//    ↓C-----D=
void MotorController::BACK_RIGHT(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_STOP, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_STOP);
}

//    ↑A-----B↓
//     | ↗ ↘ |
//     | ↖ ↙ |
//    ↑C-----D↓
void MotorController::ROTATE_CW(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
}

//    ↓A-----B↑
//     | ↙ ↖ |
//     | ↘ ↗ |
//    ↓C-----D↑
void MotorController::ROTATE_CCW(int MOTOR_PWM)
{
    _driveSetup();
    _drive(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
}

//    =A-----B=
//     |  =  |
//     |  =  |
//    =C-----D=
void MotorController::STOP()
{
    _is_stopped = true;
    _drive(0, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP);
}

//    ↓A-----B↑  angle > _target_angle  ↑A-----B↓
//     | ↙ ↖ |                           | ↗ ↘ |
//     | ↘ ↗ |                           | ↖ ↙ |
//    ↓C-----D↑                         ↑C-----D↓  angle < _target_angle
void MotorController::ROTATE_BY(int MOTOR_PWM, int increment_angle)
{
    _rotateSetup();
    _target_angle += increment_angle;
}

//    ↓A-----B↑  angle > _target_angle  ↑A-----B↓
//     | ↙ ↖ |                           | ↗ ↘ |
//     | ↘ ↗ |                           | ↖ ↙ |
//    ↓C-----D↑                         ↑C-----D↓  angle < _target_angle
void MotorController::ROTATE_TO(int MOTOR_PWM, int target_angle)
{
    _rotateSetup();
    _target_angle = target_angle;
}

String MotorController::command(char command)
{
    static int _motor_pwm = 125;
    switch (command)
    {
    case 'A': // OK
        ADVANCE(_motor_pwm);
        return "Forward";
    case 'B': // OK
        ADVANCE_RIGHT(_motor_pwm);
        return "Forward Right";
    case 'b': // -10 degree
        RIGHT(_motor_pwm);
        return "Right";
        break;
    case 'C': // SOMEWHAT OK
        ROTATE_CW(_motor_pwm);
        return "Rotate Right";
    case 'D': // OK
        BACK_RIGHT(_motor_pwm);
        return "Backward Right";
    case 'd': //
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
