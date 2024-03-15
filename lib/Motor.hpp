#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__

#include <Arduino.h>

// Define motor pins
#define PWMA 12  // Motor A PWM
#define DIRA1 34 // Motor A Direction +
#define DIRA2 35 // Motor A Direction -

#define PWMB 8   // Motor B PWM
#define DIRB1 36 // Motor B Direction +
#define DIRB2 37 // Motor B Direction -

#define PWMC 6   // Motor C PWM
#define DIRC1 43 // Motor C Direction +
#define DIRC2 42 // Motor C Direction -

#define PWMD 5   // Motor D PWM
#define DIRD1 A5 // Motor D Direction +
#define DIRD2 A4 // Motor D Direction -

// PWM Definition
#define MAX_PWM 255
#define MIN_PWM 0

#define MOTOR_RADIUS 1.5 // cm

class Motor
{
public:
    Motor(void);

    inline void setPWM(uint8_t PWM_A, uint8_t PWM_B, uint8_t PWM_C, uint8_t PWM_D);

    inline void BACK(uint8_t motor_pwm);
    inline void ADVANCE(uint8_t motor_pwm);
    inline void ADVANCE_LEFT(uint8_t motor_pwm);
    inline void RIGHT(uint8_t motor_pwm);
    inline void BACK_LEFT(uint8_t motor_pwm);
    inline void ADVANCE_RIGHT(uint8_t motor_pwm);
    inline void LEFT(uint8_t motor_pwm);
    inline void BACK_RIGHT(uint8_t motor_pwm);
    inline void ROTATE_CW(uint8_t motor_pwm);
    inline void ROTATE_CCW(uint8_t motor_pwm);
    inline void STOP();

    inline void MOVE(uint8_t speed_A, uint8_t speed_B, uint8_t speed_C, uint8_t speed_D);

private:
    inline void _setDirection(uint8_t speed, uint8_t dir_pin_pos, uint8_t dir_pin_neg);

    int _A_DIRECTION;
    int _B_DIRECTION;
    int _C_DIRECTION;
    int _D_DIRECTION;
};

Motor::Motor(void) : _A_DIRECTION(1), _B_DIRECTION(-1), _C_DIRECTION(-1), _D_DIRECTION(1) {}

#define MOTORA_FORWARD(pwm)        \
    do                             \
    {                              \
        digitalWrite(DIRA1, LOW);  \
        digitalWrite(DIRA2, HIGH); \
        analogWrite(PWMA, pwm);    \
    } while (0)
#define MOTORA_STOP(x)            \
    do                            \
    {                             \
        digitalWrite(DIRA1, LOW); \
        digitalWrite(DIRA2, LOW); \
        analogWrite(PWMA, 0);     \
    } while (0)
#define MOTORA_BACKOFF(pwm)        \
    do                             \
    {                              \
        digitalWrite(DIRA1, HIGH); \
        digitalWrite(DIRA2, LOW);  \
        analogWrite(PWMA, pwm);    \
    } while (0)

#define MOTORB_FORWARD(pwm)        \
    do                             \
    {                              \
        digitalWrite(DIRB1, LOW);  \
        digitalWrite(DIRB2, HIGH); \
        analogWrite(PWMB, pwm);    \
    } while (0)
#define MOTORB_STOP(x)            \
    do                            \
    {                             \
        digitalWrite(DIRB1, LOW); \
        digitalWrite(DIRB2, LOW); \
        analogWrite(PWMB, 0);     \
    } while (0)
#define MOTORB_BACKOFF(pwm)        \
    do                             \
    {                              \
        digitalWrite(DIRB1, HIGH); \
        digitalWrite(DIRB2, LOW);  \
        analogWrite(PWMB, pwm);    \
    } while (0)

#define MOTORC_FORWARD(pwm)        \
    do                             \
    {                              \
        digitalWrite(DIRC1, LOW);  \
        digitalWrite(DIRC2, HIGH); \
        analogWrite(PWMC, pwm);    \
    } while (0)
#define MOTORC_STOP(x)            \
    do                            \
    {                             \
        digitalWrite(DIRC1, LOW); \
        digitalWrite(DIRC2, LOW); \
        analogWrite(PWMC, 0);     \
    } while (0)
#define MOTORC_BACKOFF(pwm)        \
    do                             \
    {                              \
        digitalWrite(DIRC1, HIGH); \
        digitalWrite(DIRC2, LOW);  \
        analogWrite(PWMC, pwm);    \
    } while (0)

#define MOTORD_FORWARD(pwm)        \
    do                             \
    {                              \
        digitalWrite(DIRD1, LOW);  \
        digitalWrite(DIRD2, HIGH); \
        analogWrite(PWMD, pwm);    \
    } while (0)
#define MOTORD_STOP(x)            \
    do                            \
    {                             \
        digitalWrite(DIRD1, LOW); \
        digitalWrite(DIRD2, LOW); \
        analogWrite(PWMD, 0);     \
    } while (0)
#define MOTORD_BACKOFF(pwm)        \
    do                             \
    {                              \
        digitalWrite(DIRD1, HIGH); \
        digitalWrite(DIRD2, LOW);  \
        analogWrite(PWMD, pwm);    \
    } while (0)

//    ↓A-----B↓
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
inline void Motor::BACK(uint8_t motor_pwm)
{
    MOTORA_BACKOFF(motor_pwm);
    MOTORB_BACKOFF(motor_pwm);
    MOTORC_BACKOFF(motor_pwm);
    MOTORD_BACKOFF(motor_pwm);
}

//    ↑A-----B↑
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
inline void Motor::ADVANCE(uint8_t motor_pwm)
{
    MOTORA_FORWARD(motor_pwm);
    MOTORB_FORWARD(motor_pwm);
    MOTORC_FORWARD(motor_pwm);
    MOTORD_FORWARD(motor_pwm);
}
//    =A-----B↑
//     |   ↖ |
//     | ↖   |
//    ↑C-----D=
inline void Motor::ADVANCE_LEFT(uint8_t motor_pwm)
{
    MOTORA_STOP(motor_pwm);
    MOTORB_FORWARD(motor_pwm);
    MOTORC_FORWARD(motor_pwm);
    MOTORD_STOP(motor_pwm);
}

//    ↓A-----B↑
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
inline void Motor::LEFT(uint8_t motor_pwm)
{
    MOTORA_BACKOFF(motor_pwm);
    MOTORB_FORWARD(motor_pwm);
    MOTORC_FORWARD(motor_pwm);
    MOTORD_BACKOFF(motor_pwm);
}
//    ↓A-----B=
//     | ↙   |
//     |   ↙ |
//    =C-----D↓
inline void Motor::BACK_LEFT(uint8_t motor_pwm)
{
    MOTORA_BACKOFF(motor_pwm);
    MOTORB_STOP(motor_pwm);
    MOTORC_STOP(motor_pwm);
    MOTORD_BACKOFF(motor_pwm);
}
//    ↑A-----B=
//     | ↗   |
//     |   ↗ |
//    =C-----D↑
inline void Motor::ADVANCE_RIGHT(uint8_t motor_pwm)
{
    MOTORA_FORWARD(motor_pwm);
    MOTORB_STOP(motor_pwm);
    MOTORC_STOP(motor_pwm);
    MOTORD_FORWARD(motor_pwm);
}
//    ↑A-----B↓
//     |  →  |
//     |  →  |
//    ↓C-----D↑
inline void Motor::RIGHT(uint8_t motor_pwm)
{
    MOTORA_FORWARD(motor_pwm);
    MOTORB_BACKOFF(motor_pwm);
    MOTORC_BACKOFF(motor_pwm);
    MOTORD_FORWARD(motor_pwm);
}
//    =A-----B↓
//     |   ↘ |
//     | ↘   |
//    ↓C-----D=
inline void Motor::BACK_RIGHT(uint8_t motor_pwm)
{
    MOTORA_STOP(motor_pwm);
    MOTORB_BACKOFF(motor_pwm);
    MOTORC_BACKOFF(motor_pwm);
    MOTORD_STOP(motor_pwm);
}

//    ↑A-----B↓
//     | ↗ ↘ |
//     | ↖ ↙ |
//    ↑C-----D↓
inline void Motor::ROTATE_CW(uint8_t motor_pwm)
{
    MOTORA_FORWARD(motor_pwm);
    MOTORB_BACKOFF(motor_pwm);
    MOTORC_FORWARD(motor_pwm);
    MOTORD_BACKOFF(motor_pwm);
}

//    ↓A-----B↑
//     | ↙ ↖ |
//     | ↘ ↗ |
//    ↓C-----D↑
inline void Motor::ROTATE_CCW(uint8_t motor_pwm)
{
    MOTORA_BACKOFF(motor_pwm);
    MOTORB_FORWARD(motor_pwm);
    MOTORC_BACKOFF(motor_pwm);
    MOTORD_FORWARD(motor_pwm);
}

//    =A-----B=
//     |  =  |
//     |  =  |
//    =C-----D=
inline void Motor::STOP()
{
    MOTORA_STOP(0);
    MOTORB_STOP(0);
    MOTORC_STOP(0);
    MOTORD_STOP(0);
}

// Speed is PWM value ranged from -255 to 255, -255 is full speed backward, 255 is full speed forward, 0 is stop.
inline void Motor::_setDirection(uint8_t speed, uint8_t dir_pin_pos, uint8_t dir_pin_neg)
{
    if (speed > 0)
    {
        digitalWrite(dir_pin_pos, LOW);
        digitalWrite(dir_pin_neg, HIGH);
    }
    else if (speed < 0)
    {
        digitalWrite(dir_pin_pos, HIGH);
        digitalWrite(dir_pin_neg, LOW);
    }
    else
    {
        digitalWrite(dir_pin_pos, LOW);
        digitalWrite(dir_pin_neg, LOW);
    }
}

inline void Motor::setPWM(uint8_t PWM_A, uint8_t PWM_B, uint8_t PWM_C, uint8_t PWM_D)
{
    analogWrite(PWMA, PWM_A);
    analogWrite(PWMB, PWM_B);
    analogWrite(PWMC, PWM_C);
    analogWrite(PWMD, PWM_D);
}

inline void Motor::MOVE(uint8_t speed_A, uint8_t speed_B, uint8_t speed_C, uint8_t speed_D)
{
    _setDirection(speed_A, DIRA1, DIRA2);
    _setDirection(speed_B, DIRB1, DIRB2);
    _setDirection(speed_C, DIRC1, DIRC2);
    _setDirection(speed_D, DIRD1, DIRD2);

    setPWM(abs(speed_A), abs(speed_B), abs(speed_C), abs(speed_D));
}

#endif
