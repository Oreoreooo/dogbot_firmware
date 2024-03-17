#ifndef __HPP__
#define __HPP__

#include <Arduino.h>

#define PWMA 12 // Motor A PWM
#define PWMB 8  // Motor B PWM
#define PWMC 6  // Motor C PWM
#define PWMD 5  // Motor D PWM

// motor tuning factor, use measure() to record serial data as .csv to calculate average the value
const float TIME_INTERVAL = 10.00214;
const float MOTOR_TURN_A = 0.299069;
const float MOTOR_TURN_B = 0.317692;
const float MOTOR_TURN_C = 0.303293;
const float MOTOR_TURN_D = 0.251549;

const float MIN_MOTOR_TURN = min(min(MOTOR_TURN_A, MOTOR_TURN_B), min(MOTOR_TURN_C, MOTOR_TURN_D));
const float BALANCE_FACTOR_A = MOTOR_TURN_A / MIN_MOTOR_TURN;
const float BALANCE_FACTOR_B = MOTOR_TURN_B / MIN_MOTOR_TURN;
const float MBALANCE_FACTOR_C = MOTOR_TURN_C / MIN_MOTOR_TURN;
const float BALANCE_FACTOR_D = MOTOR_TURN_D / MIN_MOTOR_TURN;

// PWM Definition
#define MAX_PWM 200
#define MIN_PWM 0

class SpeedController
{
public:
    SpeedController();
    virtual void begin();
    virtual void reset();
    virtual inline void perform();
    virtual void measure();
    virtual void setPWM(int MOTOR_PWM);
    virtual void set_target(int target);

    void off();
    void on();

protected:
    virtual void _setPWM(int PWM_A, int PWM_B, int PWM_C, int PWM_D) final;
    inline void _log();
    bool _control_on;

private:
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

void SpeedController::measure()
{
    _log();
}

void SpeedController::setPWM(int MOTOR_PWM)
{
    if (_control_on)
    {
        _setPWM(MOTOR_PWM / BALANCE_FACTOR_A, MOTOR_PWM / BALANCE_FACTOR_B, MOTOR_PWM / MBALANCE_FACTOR_C, MOTOR_PWM / BALANCE_FACTOR_D);
    }
    else
    {
        _setPWM(MOTOR_PWM, MOTOR_PWM, MOTOR_PWM, MOTOR_PWM);
    }
}

void SpeedController::_setPWM(int PWM_A, int PWM_B, int PWM_C, int PWM_D)
{
    _PWM_A = constrain(_PWM_A, MIN_PWM, MAX_PWM);
    _PWM_B = constrain(_PWM_B, MIN_PWM, MAX_PWM);
    _PWM_C = constrain(_PWM_C, MIN_PWM, MAX_PWM);
    _PWM_D = constrain(_PWM_D, MIN_PWM, MAX_PWM);
    analogWrite(PWMA, _PWM_A);
    analogWrite(PWMB, _PWM_B);
    analogWrite(PWMC, _PWM_C);
    analogWrite(PWMD, _PWM_D);
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

void SpeedController::set_target(int target)
{
}

#endif