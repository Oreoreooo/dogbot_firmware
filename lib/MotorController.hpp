#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__

#include <Arduino.h>
#include "SpeedControllerPID.hpp"

// Define motor pins
#define DIRA1 34 // Motor A Direction +
#define DIRA2 35 // Motor A Direction -
#define DIRB1 36 // Motor B Direction +
#define DIRB2 37 // Motor B Direction -
#define DIRC1 43 // Motor C Direction +
#define DIRC2 42 // Motor C Direction -
#define DIRD1 A5 // Motor D Direction +
#define DIRD2 A4 // Motor D Direction -

// Direction Definition
#define MOTOR_FORWARD 1
#define MOTOR_BACKWARD -1
#define MOTOR_STOP 0

class MotorController
{
public:
    MotorController(void);

    void begin();
    void perform();
    void measure();
    void move(uint8_t MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D);

    String command(char command);

    void BACK(uint8_t MOTOR_PWM);
    void ADVANCE(uint8_t MOTOR_PWM);
    void ADVANCE_LEFT(uint8_t MOTOR_PWM);
    void RIGHT(uint8_t MOTOR_PWM);
    void BACK_LEFT(uint8_t MOTOR_PWM);
    void ADVANCE_RIGHT(uint8_t MOTOR_PWM);
    void LEFT(uint8_t MOTOR_PWM);
    void BACK_RIGHT(uint8_t MOTOR_PWM);
    void ROTATE_CW(uint8_t MOTOR_PWM);
    void ROTATE_CCW(uint8_t MOTOR_PWM);
    void STOP();

    void onSpeedControl();
    void offSpeedControl();

private:
    SpeedControllerPID speed_controller;

    int _setDirection(int direction, uint8_t pin_postive, uint8_t pin_negative);

    int _A_DIRECTION;
    int _B_DIRECTION;
    int _C_DIRECTION;
    int _D_DIRECTION;
};

MotorController::MotorController(void) : _A_DIRECTION(MOTOR_STOP), _B_DIRECTION(MOTOR_STOP), _C_DIRECTION(MOTOR_STOP), _D_DIRECTION(MOTOR_STOP), speed_controller(SpeedControllerPID()) {}

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
    speed_controller.begin();
}

int MotorController::_setDirection(int direction, uint8_t pin_positive, uint8_t pin_negative)
{
    if (direction > 0)
    {
        digitalWrite(pin_positive, LOW);
        digitalWrite(pin_negative, HIGH);
        return MOTOR_FORWARD;
    }
    else if (direction < 0)
    {
        digitalWrite(pin_positive, HIGH);
        digitalWrite(pin_negative, LOW);
        return MOTOR_BACKWARD;
    }
    else
    {
        digitalWrite(pin_positive, LOW);
        digitalWrite(pin_negative, LOW);
        return MOTOR_STOP;
    }
}

void MotorController::move(uint8_t MOTOR_PWM, int DIR_A, int DIR_B, int DIR_C, int DIR_D)
{
    _A_DIRECTION = _setDirection(DIR_A, DIRA1, DIRA2);
    _B_DIRECTION = _setDirection(DIR_B, DIRB1, DIRB2);
    _C_DIRECTION = _setDirection(DIR_C, DIRC1, DIRC2);
    _D_DIRECTION = _setDirection(DIR_D, DIRD1, DIRD2);

    speed_controller.setPWM(MOTOR_PWM);
}

String MotorController::command(char command)
{
    static uint8_t _motor_pwm = 125;
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

void MotorController::measure()
{
    _setDirection(MOTOR_FORWARD, DIRA1, DIRA2);
    _setDirection(MOTOR_FORWARD, DIRB1, DIRB2);
    _setDirection(MOTOR_FORWARD, DIRC1, DIRC2);
    _setDirection(MOTOR_FORWARD, DIRD1, DIRD2);
    speed_controller.measure();
}

void MotorController::onSpeedControl()
{
    speed_controller.on();
}

void MotorController::offSpeedControl()
{
    speed_controller.off();
}

void MotorController::perform()
{
    speed_controller.perform();
}

//    ↓A-----B↓
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
void MotorController::BACK(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_BACKWARD);
    speed_controller.reset();
}

//    ↑A-----B↑
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
void MotorController::ADVANCE(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD);
    speed_controller.reset();
}
//    =A-----B↑
//     |   ↖ |
//     | ↖   |
//    ↑C-----D=
void MotorController::ADVANCE_LEFT(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_STOP, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_STOP);
    speed_controller.reset();
}

//    ↓A-----B↑
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
void MotorController::LEFT(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
    speed_controller.reset();
}
//    ↓A-----B=
//     | ↙   |
//     |   ↙ |
//    =C-----D↓
void MotorController::BACK_LEFT(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_BACKWARD);
    speed_controller.reset();
}
//    ↑A-----B=
//     | ↗   |
//     |   ↗ |
//    =C-----D↑
void MotorController::ADVANCE_RIGHT(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_FORWARD, MOTOR_STOP, MOTOR_STOP, MOTOR_FORWARD);
    speed_controller.reset();
}
//    ↑A-----B↓
//     |  →  |
//     |  →  |
//    ↓C-----D↑
void MotorController::RIGHT(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
    speed_controller.reset();
}
//    =A-----B↓
//     |   ↘ |
//     | ↘   |
//    ↓C-----D=
void MotorController::BACK_RIGHT(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_STOP, MOTOR_BACKWARD, MOTOR_BACKWARD, MOTOR_STOP);
    speed_controller.reset();
}

//    ↑A-----B↓
//     | ↗ ↘ |
//     | ↖ ↙ |
//    ↑C-----D↓
void MotorController::ROTATE_CW(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD);
    speed_controller.reset();
}

//    ↓A-----B↑
//     | ↙ ↖ |
//     | ↘ ↗ |
//    ↓C-----D↑
void MotorController::ROTATE_CCW(uint8_t MOTOR_PWM)
{
    move(MOTOR_PWM, MOTOR_BACKWARD, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_FORWARD);
    speed_controller.reset();
}

//    =A-----B=
//     |  =  |
//     |  =  |
//    =C-----D=
void MotorController::STOP()
{
    move(0, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP);
    speed_controller.reset();
}

#endif
