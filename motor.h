#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <Arduino.h>
#include <Encoder.h>

// Define motor pins
#define PWMA 12  // Motor A PWM
#define DIRA1 34 // Motor A Direction +
#define DIRA2 35 // Motor A Direction -
#define ECDAA 18 // Motor A Encoder PIN A
#define ECDAB 31 // Motor A Encoder PIN B

#define PWMB 8   // Motor B PWM
#define DIRB1 37 // Motor C Direction +
#define DIRB2 36 // Motor B Direction -
#define ECDBA 19 // Motor B Encoder PIN A
#define ECDBB 38 // Motor B Encoder PIN B

#define PWMC 6   // Motor C PWM
#define DIRC1 43 // Motor C Direction +
#define DIRC2 42 // Motor C Direction -
#define ECDCA 3  // Motor C Encoder PIN A
#define ECDCB 49 // Motor C Encoder PIN B

#define PWMD 5   // Motor D PWM
#define DIRD1 A4 // Motor D Direction +
#define DIRD2 A5 // Motor D Direction -
#define ECDDA 2  // Motor D Encoder PIN A
#define ECDDB A1 // Motor D Encoder PIN B

class Motor
{

public:
    Motor(void);

    void BACK(uint8_t motor_pwm);
    void ADVANCE(uint8_t motor_pwm);
    void ADVANCE_LEFT(uint8_t motor_pwm);
    void RIGHT(uint8_t motor_pwm);
    void BACK_LEFT(uint8_t motor_pwm);
    void ADVANCE_RIGHT(uint8_t motor_pwm);
    void LEFT(uint8_t motor_pwm);
    void BACK_RIGHT(uint8_t motor_pwm);
    void ROTATE_CW(uint8_t motor_pwm);
    void ROTATE_CCW(uint8_t motor_pwm);

    void BACK(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void ADVANCE(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void ADVANCE_LEFT(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void RIGHT(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void BACK_LEFT(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void ADVANCE_RIGHT(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void LEFT(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void BACK_RIGHT(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void ROTATE_CW(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);
    void ROTATE_CCW(uint8_t pwm_A, uint8_t pwm_B, uint8_t pwm_C, uint8_t pwm_D);

    void STOP();



    long getTurnsA();
    long getTurnsB();
    long getTurnsC();
    long getTurnsD();

    void update();
    void reset();

private:
    long turnsA = 0;
    long turnsB = 0;
    long turnsC = 0;
    long turnsD = 0;
};

#endif