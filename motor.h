#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <Arduino.h>

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

private:
};

#endif