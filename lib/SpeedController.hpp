#ifndef __SPEED_CONTROLLER_HPP__
#define __SPEED_CONTROLLER_HPP__

#include <Arduino.h>

#define PWMA 12 // Motor A PWM
#define PWMB 8  // Motor B PWM
#define PWMC 6  // Motor C PWM
#define PWMD 5  // Motor D PWM

// motor tuning factor, use measure() to record serial data as .csv to calculate average the value
const float TIME_INTERVAL = 10.00214;
const float MOTOR_RPPWM_A = 0.299069;
const float MOTOR_RPPWM_B = 0.317692;
const float MOTOR_RPPWM_C = 0.303293;
const float MOTOR_RPPWM_D = 0.251549;

const float MIN_MOTOR_RPPWM = min(min(MOTOR_RPPWM_A, MOTOR_RPPWM_B), min(MOTOR_RPPWM_C, MOTOR_RPPWM_D));
const float MOTOR_BALANCE_FACTOR_A = MOTOR_RPPWM_A / MIN_MOTOR_RPPWM;
const float MOTOR_BALANCE_FACTOR_B = MOTOR_RPPWM_B / MIN_MOTOR_RPPWM;
const float MOTOR_BALANCE_FACTOR_C = MOTOR_RPPWM_C / MIN_MOTOR_RPPWM;
const float MOTOR_BALANCE_FACTOR_D = MOTOR_RPPWM_D / MIN_MOTOR_RPPWM;

// PWM Definition
#define MAX_PWM 255
#define MIN_PWM 0
#define MAX_PWM_f 255.0
#define MIN_PWM_f 0.0

class SpeedController
{
public:
    SpeedController();
    virtual void begin();
    virtual void reset();
    virtual inline void perform();
    virtual void measure();
    virtual void setPWM(int MOTOR_PWM);
    void off();
    void on();

protected:
    inline void _log();
    inline void _writePWM();
    bool _control_on;
    int _PWM_A;
    int _PWM_B;
    int _PWM_C;
    int _PWM_D;
};

SpeedController::SpeedController() : _PWM_A(0), _PWM_B(0), _PWM_C(0), _PWM_D(0) {}

void SpeedController::begin()
{
    _control_on = true;
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(PWMC, OUTPUT);
    pinMode(PWMD, OUTPUT);
}

void SpeedController::reset()
{
    _control_on = true;
}

// PID Control Based on Encoder
inline void SpeedController::perform()
{
}

inline void SpeedController::_writePWM()
{
    analogWrite(PWMA, _PWM_A);
    analogWrite(PWMB, _PWM_B);
    analogWrite(PWMC, _PWM_C);
    analogWrite(PWMD, _PWM_D);
}

void SpeedController::measure()
{
    _log();
}

void SpeedController::setPWM(int MOTOR_PWM)
{
    _PWM_A = _control_on ? constrain((int)(MOTOR_PWM / MOTOR_BALANCE_FACTOR_A), MIN_PWM, MAX_PWM) : MOTOR_PWM;
    _PWM_B = _control_on ? constrain((int)(MOTOR_PWM / MOTOR_BALANCE_FACTOR_B), MIN_PWM, MAX_PWM) : MOTOR_PWM;
    _PWM_C = _control_on ? constrain((int)(MOTOR_PWM / MOTOR_BALANCE_FACTOR_C), MIN_PWM, MAX_PWM) : MOTOR_PWM;
    _PWM_D = _control_on ? constrain((int)(MOTOR_PWM / MOTOR_BALANCE_FACTOR_D), MIN_PWM, MAX_PWM) : MOTOR_PWM;
    _writePWM();
}
void SpeedController::off()
{
    _control_on = false;
}

void SpeedController::on()
{
    _control_on = true;
}

inline void SpeedController::_log()
{
    Serial.print(_PWM_A);
    Serial.print(",");
    Serial.print(_PWM_B);
    Serial.print(",");
    Serial.print(_PWM_C);
    Serial.print(",");
    Serial.println(_PWM_D);
}

#endif