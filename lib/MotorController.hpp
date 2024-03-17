#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__
#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Arduino.h>
#include <Encoder.h>

// Define motor pins
#define PWMA 12  // Motor A PWM
#define DIRA1 34 // Motor A Direction +
#define DIRA2 35 // Motor A Direction -
#define ECDAA 18 // Motor A Encoder PIN A
#define ECDAB 31 // Motor A Encoder PIN B

#define PWMB 8   // Motor B PWM
#define DIRB1 36 // Motor B Direction +
#define DIRB2 37 // Motor B Direction -
#define ECDBA 19 // Motor B Encoder PIN A
#define ECDBB 38 // Motor B Encoder PIN B

#define PWMC 6   // Motor C PWM
#define DIRC1 43 // Motor C Direction +
#define DIRC2 42 // Motor C Direction -
#define ECDCA 3  // Motor C Encoder PIN A
#define ECDCB 49 // Motor C Encoder PIN B

#define PWMD 5   // Motor D PWM
#define DIRD1 A5 // Motor D Direction +
#define DIRD2 A4 // Motor D Direction -
#define ECDDA 2  // Motor D Encoder PIN A
#define ECDDB A1 // Motor D Encoder PIN B

// PWM Definition
#define MAX_PWM 255
#define MIN_PWM 0

// Direction Definition
#define MOTOR_FORWARD 1
#define MOTOR_BACKWARD -1
#define MOTOR_STOP 0

// Motor tuning factor, use measure() and record serial data as .csv to calculate average the value
#define MOTOR_A_RPS 0.007922329950478557
#define MOTOR_B_RPS 0.008408508633843398
#define MOTOR_C_RPS 0.008045855179921036
#define MOTOR_D_RPS 0.006797039616074927

class MotorController
{
public:
    MotorController(void);

    inline void controlByCommand(char command);
    inline void measureRotationPerSecond();

    inline void performPID();
    inline void resetPID();

    inline void BACK(uint8_t MOTOR_PWM);
    inline void ADVANCE(uint8_t MOTOR_PWM);
    inline void ADVANCE_LEFT(uint8_t MOTOR_PWM);
    inline void RIGHT(uint8_t MOTOR_PWM);
    inline void BACK_LEFT(uint8_t MOTOR_PWM);
    inline void ADVANCE_RIGHT(uint8_t MOTOR_PWM);
    inline void LEFT(uint8_t MOTOR_PWM);
    inline void BACK_RIGHT(uint8_t MOTOR_PWM);
    inline void ROTATE_CW(uint8_t MOTOR_PWM);
    inline void ROTATE_CCW(uint8_t MOTOR_PWM);
    inline void STOP();

    inline void MOVE(uint8_t MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D);
    inline void MOVE(int PWM_A, int PWM_B, int PWM_C, int PWM_D);

private:
    inline int _setDirection(int direction, uint8_t dir_pin_positive, uint8_t dir_pin_negative);
    inline void _setPWM(uint8_t PWM_A, uint8_t PWM_B, uint8_t PWM_C, uint8_t PWM_D);
    inline void _readECDs();
    inline void _computePID();
    inline void _logPWM();

    int _A_DIRECTION;
    int _B_DIRECTION;
    int _C_DIRECTION;
    int _D_DIRECTION;

    Encoder _ECDA{ECDAA, ECDAB};
    Encoder _ECDB{ECDBA, ECDBB};
    Encoder _ECDC{ECDCA, ECDCB};
    Encoder _ECDD{ECDDA, ECDDB};

    int32_t _ECD_A;
    int32_t _ECD_B;
    int32_t _ECD_C;
    int32_t _ECD_D;

    uint8_t _PWM_A;
    uint8_t _PWM_B;
    uint8_t _PWM_C;
    uint8_t _PWM_D;

    unsigned long _prev_time = 0;
    const unsigned long _time_step = 10;

    float _kP = 0.20;
    float _kI = 0.000005;

    int32_t _prev_error_A = 0;
    int32_t _prev_error_B = 0;
    int32_t _prev_error_C = 0;

    int32_t _integral_A = 0;
    int32_t _integral_B = 0;
    int32_t _integral_C = 0;

    const float _MOTOR_MIN_RPS = min(min(MOTOR_A_RPS, MOTOR_B_RPS), min(MOTOR_C_RPS, MOTOR_D_RPS));
    const float _MOTOR_A_BALANCE_FACTOR = MOTOR_A_RPS / _MOTOR_MIN_RPS;
    const float _MOTOR_B_BALANCE_FACTOR = MOTOR_B_RPS / _MOTOR_MIN_RPS;
    const float _MOTOR_C_BALANCE_FACTOR = MOTOR_C_RPS / _MOTOR_MIN_RPS;
    const float _MOTOR_D_BALANCE_FACTOR = MOTOR_D_RPS / _MOTOR_MIN_RPS;
};

MotorController::MotorController(void){};

//    ↓A-----B↓
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
inline void MotorController::BACK(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD);
    resetPID();
}

//    ↑A-----B↑
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
inline void MotorController::ADVANCE(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD);
    resetPID();
}
//    =A-----B↑
//     |   ↖ |
//     | ↖   |
//    ↑C-----D=
inline void MotorController::ADVANCE_LEFT(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_STOP, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_STOP);
    resetPID();
}

//    ↓A-----B↑
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
inline void MotorController::LEFT(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
    resetPID();
}
//    ↓A-----B=
//     | ↙   |
//     |   ↙ |
//    =C-----D↓
inline void MotorController::BACK_LEFT(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_BACKWARD);
    resetPID();
}
//    ↑A-----B=
//     | ↗   |
//     |   ↗ |
//    =C-----D↑
inline void MotorController::ADVANCE_RIGHT(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_FORWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_FORWARD);
    resetPID();
}
//    ↑A-----B↓
//     |  →  |
//     |  →  |
//    ↓C-----D↑
inline void MotorController::RIGHT(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
    resetPID();
}
//    =A-----B↓
//     |   ↘ |
//     | ↘   |
//    ↓C-----D=
inline void MotorController::BACK_RIGHT(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_STOP, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_STOP);
    resetPID();
}

//    ↑A-----B↓
//     | ↗ ↘ |
//     | ↖ ↙ |
//    ↑C-----D↓
inline void MotorController::ROTATE_CW(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
    resetPID();
}

//    ↓A-----B↑
//     | ↙ ↖ |
//     | ↘ ↗ |
//    ↓C-----D↑
inline void MotorController::ROTATE_CCW(uint8_t MOTOR_PWM)
{
    MOVE(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
    resetPID();
}

//    =A-----B=
//     |  =  |
//     |  =  |
//    =C-----D=
inline void MotorController::STOP()
{
    MOVE(0, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP);
    resetPID();
}

inline int MotorController::_setDirection(int direction, uint8_t dir_pin_positive, uint8_t dir_pin_negative)
{
    if (direction > 0)
    {
        digitalWrite(dir_pin_positive, LOW);
        digitalWrite(dir_pin_negative, HIGH);
        return MOTOR_FORWARD;
    }
    else if (direction < 0)
    {
        digitalWrite(dir_pin_positive, HIGH);
        digitalWrite(dir_pin_negative, LOW);
        return MOTOR_BACKWARD;
    }
    else
    {
        digitalWrite(dir_pin_positive, LOW);
        digitalWrite(dir_pin_negative, LOW);
        return MOTOR_STOP;
    }
}

inline void MotorController::_setPWM(uint8_t PWM_A, uint8_t PWM_B, uint8_t PWM_C, uint8_t PWM_D)
{
    _PWM_A = PWM_A;
    _PWM_B = PWM_B;
    _PWM_C = PWM_C;
    _PWM_D = PWM_D;
    analogWrite(PWMA, PWM_A);
    analogWrite(PWMB, PWM_B);
    analogWrite(PWMC, PWM_C);
    analogWrite(PWMD, PWM_D);
}

inline void MotorController::MOVE(uint8_t MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _A_DIRECTION = _setDirection(DIR_A, DIRA1, DIRA2);
    _B_DIRECTION = _setDirection(DIR_B, DIRB1, DIRB2);
    _C_DIRECTION = _setDirection(DIR_C, DIRC1, DIRC2);
    _D_DIRECTION = _setDirection(DIR_D, DIRD1, DIRD2);

    _setPWM(constrain(MOTOR_PWM / _MOTOR_A_BALANCE_FACTOR, MIN_PWM, MAX_PWM), constrain(MOTOR_PWM / _MOTOR_B_BALANCE_FACTOR, MIN_PWM, MAX_PWM), constrain(MOTOR_PWM / _MOTOR_C_BALANCE_FACTOR, MIN_PWM, MAX_PWM), constrain(MOTOR_PWM / _MOTOR_D_BALANCE_FACTOR, MIN_PWM, MAX_PWM));
}

inline void MotorController::MOVE(int PWM_A, int PWM_B, int PWM_C, int PWM_D)
{
    _A_DIRECTION = _setDirection(PWM_A, DIRA1, DIRA2);
    _B_DIRECTION = _setDirection(PWM_B, DIRB1, DIRB2);
    _C_DIRECTION = _setDirection(PWM_C, DIRC1, DIRC2);
    _D_DIRECTION = _setDirection(PWM_D, DIRD1, DIRD2);

    _setPWM(constrain(abs(PWM_A), MIN_PWM, MAX_PWM), constrain(abs(PWM_B), MIN_PWM, MAX_PWM), constrain(abs(PWM_C), MIN_PWM, MAX_PWM), constrain(abs(PWM_D), MIN_PWM, MAX_PWM));
}

inline void MotorController::resetPID()
{
    _integral_A = 0;
    _integral_B = 0;
    _integral_C = 0;
    _prev_error_A = 0;
    _prev_error_B = 0;
    _prev_error_C = 0;
}

inline void MotorController::_computePID()
{
    _readECDs();
    if (_D_DIRECTION != MOTOR_STOP)
    {
        int32_t _error_A = _ECD_D - _ECD_A;
        int32_t _error_B = _ECD_D - _ECD_B;
        int32_t _error_C = _ECD_D - _ECD_C;

        _integral_A += (_error_A + _prev_error_A);
        _integral_B += (_error_B + _prev_error_B);
        _integral_C += (_error_C + _prev_error_C);

        // int32_t _derivative_A = (_error_A - _prev_error_A);
        // int32_t _derivative_B = (_error_B - _prev_error_B);
        // int32_t _derivative_C = (_error_C - _prev_error_C);

        _prev_error_A = _error_A;
        _prev_error_B = _error_B;
        _prev_error_C = _error_C;

        _PWM_A = constrain(_kP * _error_A + _kI * _integral_A, MIN_PWM, MAX_PWM);
        _PWM_B = constrain(_kP * _error_B + _kI * _integral_B, MIN_PWM, MAX_PWM);
        _PWM_C = constrain(_kP * _error_C + _kI * _integral_C, MIN_PWM, MAX_PWM);
    }
    else
    {
        int32_t _error_A = _ECD_C - _ECD_A;
        int32_t _error_B = _ECD_C - _ECD_B;

        _integral_A += (_error_A + _prev_error_A);
        _integral_B += (_error_B + _prev_error_B);

        // int32_t _derivative_A = (_error_A - _prev_error_A);
        // int32_t _derivative_B = (_error_B - _prev_error_B);

        _prev_error_A = _error_A;
        _prev_error_B = _error_B;

        _PWM_A = constrain(_kP * _error_A + _kI * _integral_A, MIN_PWM, MAX_PWM);
        _PWM_B = constrain(_kP * _error_B + _kI * _integral_B, MIN_PWM, MAX_PWM);
    }
}

inline void MotorController::_logPWM()
{
    Serial.print(_PWM_A);
    Serial.print(",");
    Serial.print(_PWM_B);
    Serial.print(",");
    Serial.print(_PWM_C);
    Serial.print(",");
    Serial.println(_PWM_D);
}

// PID Control Based on Encoder
inline void MotorController::performPID()
{
    if (millis() - _prev_time >= _time_step)
    {
        _prev_time = millis();

        _computePID();
        _writePWM(_PWM_A, _PWM_B, _PWM_C, _PWM_D);
    }
}

inline void MotorController::_readECDs()
{
    _ECD_A = -_ECDA.read();
    _ECD_B = _ECDB.read();
    _ECD_C = -_ECDC.read();
    _ECD_D = _ECDD.read();
}

inline void MotorController::controlByCommand(char command)
{
    static uint8_t _motor_pwm = 50;
    switch (command)
    {
    case 'A':
        ADVANCE(_motor_pwm);
        break;
    case 'B':
        ADVANCE_RIGHT(_motor_pwm);
    case 'b':
        RIGHT(_motor_pwm);
        break;
    case 'C':
        ROTATE_CW(_motor_pwm);
        break;
    case 'D':
        BACK_RIGHT(_motor_pwm);
        break;
    case 'd':
        LEFT(_motor_pwm);
        break;
    case 'E':
        BACK(_motor_pwm);
        break;
    case 'F':
        BACK_LEFT(_motor_pwm);
        break;
    case 'G':
        ROTATE_CCW(_motor_pwm);
        break;
    case 'H':
        ADVANCE_LEFT(_motor_pwm);
        break;
    case 'L':
        _motor_pwm = constrain(_motor_pwm + 5, MAX_PWM, MIN_PWM);
        break;
    case 'M':
        _motor_pwm = constrain(_motor_pwm - 5, MAX_PWM, MIN_PWM);
        break;
    case 'Z':
        STOP();
        break;
    case 'z':
        STOP();
        break;
    default:
        break;
    }
}

// WARNING: Leave the wheel in the air before run this function.
inline void MotorController::measureRotationPerSecond()
{
    MOVE(255, 255, 255, 255, true);
    unsigned long start_time = micros();
    unsigned long curr_time = start_time;
    unsigned long prev_time = start_time;

    while (curr_time - start_time < 500000)
    {
        curr_time = micros();
    }
    while (curr_time - start_time < 30500000)
    {
        curr_time = micros();
        unsigned long interval = curr_time - prev_time;
        if (interval >= 9000)
        {
            prev_time = curr_time;

            int32_t prev_ECD_A = _ECD_A;
            int32_t prev_ECD_B = _ECD_B;
            int32_t prev_ECD_C = _ECD_C;
            int32_t prev_ECD_D = _ECD_D;
            _readECDs();
            int32_t sample_ECD_A = _ECD_A - prev_ECD_A;
            int32_t sample_ECD_B = _ECD_B - prev_ECD_B;
            int32_t sample_ECD_C = _ECD_C - prev_ECD_C;
            int32_t sample_ECD_D = _ECD_D - prev_ECD_D;

            Serial.print(interval);
            Serial.print(",");
            Serial.print(sample_ECD_A);
            Serial.print(",");
            Serial.print(sample_ECD_B);
            Serial.print(",");
            Serial.print(sample_ECD_C);
            Serial.print(",");
            Serial.println(sample_ECD_D);
        }
    }
}

#endif
