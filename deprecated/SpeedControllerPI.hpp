#ifndef __PID_HPP__
#define __PID_HPP__

#define ENCODER_OPTIMIZE_INTERRUPTS

#include "SpeedController.hpp"
#include <Arduino.h>
#include <Encoder.h>

#define ECDAA 18 // Motor A Encoder PIN A
#define ECDAB 31 // Motor A Encoder PIN B
#define ECDBA 19 // Motor B Encoder PIN A
#define ECDBB 38 // Motor B Encoder PIN B
#define ECDCA 3  // Motor C Encoder PIN A
#define ECDCB 49 // Motor C Encoder PIN B
#define ECDDA 2  // Motor D Encoder PIN A
#define ECDDB A1 // Motor D Encoder PIN B

const float kP = 0.1;
const float kI = 0.0001;

struct MotorValue {
    int base_pwm;
    float integral;
    int32_t ecd_val;
    int32_t ecd_val_prev;
    int32_t ecd_diff;
};

class SpeedControllerPI : public SpeedController {
public:
    SpeedControllerPI(void);
    virtual void begin() override;
    virtual void reset() override;
    virtual inline void perform() override;
    virtual void setPWM(int MOTOR_PWM) override;
    virtual void measure() override;

private:
    void _read();
    unsigned long _dT;
    unsigned long _prev_time;
    unsigned long _time_step;

    int _startup_count;

    struct MotorValue _MTRA;
    struct MotorValue _MTRB;
    struct MotorValue _MTRC;
    struct MotorValue _MTRD;

    Encoder _ECDA;
    Encoder _ECDB;
    Encoder _ECDC;
    Encoder _ECDD;

    float _set_point;
};

SpeedControllerPI::SpeedControllerPI(void) : SpeedController(), _ECDA(Encoder(ECDAA, ECDAB)), _ECDB(Encoder(ECDBA, ECDBB)), _ECDC(Encoder(ECDCA, ECDCB)), _ECDD(Encoder(ECDDA, ECDDB)) {
    _time_step = 10;
}

void SpeedControllerPI::begin() {
    SpeedController::begin();
    pinMode(ECDAA, INPUT);
    pinMode(ECDAB, INPUT);
    pinMode(ECDBA, INPUT);
    pinMode(ECDBB, INPUT);
    pinMode(ECDCA, INPUT);
    pinMode(ECDCB, INPUT);
    pinMode(ECDDA, INPUT);
    pinMode(ECDDB, INPUT);
    _prev_time = millis();
}

void SpeedControllerPI::reset() {
    SpeedController::reset();
    _MTRA.integral = 0;
    _MTRB.integral = 0;
    _MTRC.integral = 0;
    _MTRD.integral = 0;
    _prev_time = millis();
}

inline void SpeedControllerPI::perform() {
    _dT = millis() - _prev_time;
    if (_dT >= _time_step && _control_on)
    {
        if (_startup_count < 250)
        {
            _startup_count++;
            return;
        }
        _prev_time = millis();
        _read();

        // convert to revolution per step
        float _scale = (float)_dT / TIME_INTERVAL;
        float _error_a = _set_point - (float)_MTRA.ecd_diff / _scale;
        float _error_b = _set_point - (float)_MTRB.ecd_diff / _scale;
        float _error_c = _set_point - (float)_MTRC.ecd_diff / _scale;
        float _error_d = _set_point - (float)_MTRD.ecd_diff / _scale;

        _MTRA.integral += kI * _error_a;
        _MTRB.integral += kI * _error_b;
        _MTRC.integral += kI * _error_c;
        _MTRD.integral += kI * _error_d;

        _setPWM(kP * _error_a + _MTRA.integral + _MTRA.base_pwm, kP * _error_b + _MTRB.integral + _MTRB.base_pwm, kP * _error_c + _MTRC.integral + _MTRC.base_pwm, kP * _error_d + _MTRD.integral + _MTRD.base_pwm);
    }
}

inline void SpeedControllerPI::_read() {
    _MTRA.ecd_val_prev = _MTRA.ecd_val;
    _MTRB.ecd_val_prev = _MTRB.ecd_val;
    _MTRC.ecd_val_prev = _MTRC.ecd_val;
    _MTRD.ecd_val_prev = _MTRD.ecd_val;

    _MTRA.ecd_val = -_ECDA.read();
    _MTRB.ecd_val = _ECDB.read();
    _MTRC.ecd_val = -_ECDC.read();
    _MTRD.ecd_val = _ECDD.read();

    _MTRA.ecd_diff = _MTRA.ecd_val - _MTRA.ecd_val_prev;
    _MTRB.ecd_diff = _MTRB.ecd_val - _MTRB.ecd_val_prev;
    _MTRC.ecd_diff = _MTRC.ecd_val - _MTRC.ecd_val_prev;
    _MTRD.ecd_diff = _MTRD.ecd_val - _MTRD.ecd_val_prev;
}

void SpeedControllerPI::setPWM(int MOTOR_PWM) {
    _startup_count = 0;

    _MTRA.base_pwm = MOTOR_PWM / BALANCE_FACTOR_A;
    _MTRB.base_pwm = MOTOR_PWM / BALANCE_FACTOR_B;
    _MTRC.base_pwm = MOTOR_PWM / MBALANCE_FACTOR_C;
    _MTRD.base_pwm = MOTOR_PWM / BALANCE_FACTOR_D;

    if (_control_on)
    {
        _set_point = (float)MOTOR_PWM * MIN_MOTOR_TURN;
    }
    else
    {
        _setPWM(_MTRA.base_pwm, _MTRB.base_pwm, _MTRC.base_pwm, _MTRD.base_pwm);
    }
}

// WARNING: leaving the wheels in the air before run this function.


#endif