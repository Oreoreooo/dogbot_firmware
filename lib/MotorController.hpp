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
const float MOTOR_TURN_A = 7.928667;
const float MOTOR_TURN_B = 8.404883;
const float MOTOR_TURN_C = 8.049408;
const float MOTOR_TURN_D = 6.803542;
const float MIN_MOTOR_TURN = min(min(MOTOR_TURN_A, MOTOR_TURN_B), min(MOTOR_TURN_C, MOTOR_TURN_D));
const float BALANCE_FACTOR_A = MIN_MOTOR_TURN / MOTOR_TURN_A;
const float BALANCE_FACTOR_B = MIN_MOTOR_TURN / MOTOR_TURN_B;
const float BALANCE_FACTOR_C = MIN_MOTOR_TURN / MOTOR_TURN_C;
const float BALANCE_FACTOR_D = MIN_MOTOR_TURN / MOTOR_TURN_D;

#define MAX_PWM 255
#define MIN_PWM 50

const float threshold = 1.0;
const float kP = 1.0;
const float kI = 0.005;

extern MPU6050 mpu;

class MotorController
{
public:
    MotorController(void);

    void begin();
    void balance();

    String command(char command);

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

private:
    inline void _driveSetup();
    inline void _rotateSetup();

    inline void _drive(int MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D);

    inline void _adjust();
    inline void _rotate();

    inline void _setWheelDirection(int DIR, int positive, int negative);
    inline void _setDirection(int DIR_A, int DIR_B, int DIR_C, int DIR_D);
    inline void _setTargetPWM(int MOTOR_PWM);
    inline void _setPWM(int PWM_A, int PWM_B, int PWM_C, int PWM_D);

    inline bool _is_steady();

    int _PWM_A;
    int _PWM_B;
    int _PWM_C;
    int _PWM_D;

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
    _time_step = 25;
    STOP();
}

void MotorController::balance()
{
    if (millis() - _prev_time >= _time_step && !_is_stopped)
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
    float error = _target_angle - mpu.getAngleZ();

    if (abs(round(error)) <= threshold)
    {
        _setPWM(_PWM_A_target, _PWM_B_target, _PWM_C_target, _PWM_D_target);
        return;
    }

    _integral += error * kI;
    float output = kP * error + _integral;

    float gyro_z = mpu.getGyroZ();

    Serial.print("E: ");
    Serial.print(error);
    Serial.print(", O: ");
    Serial.print(output);
    Serial.print(", G: ");
    Serial.println(gyro_z);

    _setPWM(_PWM_A_target - output, _PWM_B + output, _PWM_C_target - output, _PWM_D + output);
}

inline void MotorController::_rotate()
{
    float _error = _target_angle - mpu.getAngleZ();

    if (_error > threshold) // CW
    {
        _setDirection(MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
    }
    else if (_error < -threshold) // CCW
    {
        _setDirection(MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
    }
    else
    {
        _setDirection(MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP);
        _setPWM(0, 0, 0, 0);
        return;
    }

    _integral += _error * kI;
    float output = abs(kP * _error + _integral);
    float gyro_z = abs(mpu.getGyroZ());

    if (gyro_z > output)
    {
        _setPWM(_PWM_B - 1, _PWM_B - 1, _PWM_B - 1, _PWM_D - 1);
    }
    else if (gyro_z < output)
    {
        _setPWM(_PWM_A + 1, _PWM_A + 1, _PWM_C - 1, _PWM_A + 1);
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
    _PWM_A = constrain(PWM_A, MIN_PWM, MAX_PWM);
    _PWM_B = constrain(PWM_B, MIN_PWM, MAX_PWM);
    _PWM_C = constrain(PWM_C, MIN_PWM, MAX_PWM);
    _PWM_D = constrain(PWM_D, MIN_PWM, MAX_PWM);
    analogWrite(PWMA, _PWM_A);
    analogWrite(PWMB, _PWM_B);
    analogWrite(PWMC, _PWM_C);
    analogWrite(PWMD, _PWM_D);
}

inline void MotorController::_setTargetPWM(int MOTOR_PWM)
{
    _PWM_A_target = constrain(BALANCE_FACTOR_A * MOTOR_PWM, MIN_PWM, MAX_PWM);
    _PWM_B_target = constrain(BALANCE_FACTOR_B * MOTOR_PWM, MIN_PWM, MAX_PWM);
    _PWM_C_target = constrain(BALANCE_FACTOR_C * MOTOR_PWM, MIN_PWM, MAX_PWM);
    _PWM_D_target = constrain(BALANCE_FACTOR_D * MOTOR_PWM, MIN_PWM, MAX_PWM);
}

// Speed is weighed PWM based on the slowest motor
inline void MotorController::_drive(int MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _setDirection(DIR_A, DIR_B, DIR_C, DIR_D);
    _setTargetPWM(MOTOR_PWM);
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
    _setTargetPWM(0);
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
    _driveSetup();
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
