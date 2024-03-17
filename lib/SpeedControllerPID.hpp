#ifndef __SPEED_CONTROLLER_PID_HPP__
#define __SPEED_CONTROLLER_PID_HPP__

#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Arduino.h>
#include <Encoder.h>
#include "SpeedController.hpp"

#define ECDAA 18 // Motor A Encoder PIN A
#define ECDAB 31 // Motor A Encoder PIN B
#define ECDBA 19 // Motor B Encoder PIN A
#define ECDBB 38 // Motor B Encoder PIN B
#define ECDCA 3  // Motor C Encoder PIN A
#define ECDCB 49 // Motor C Encoder PIN B
#define ECDDA 2  // Motor D Encoder PIN A
#define ECDDB A1 // Motor D Encoder PIN B

const float kP = 5.0;
const float kI = 0.0;
const float kD = 0.0;

struct MotorPID
{
    float integral;
    float error;
    float derivative;

    int32_t ECD_val;
    int32_t ECD_val_prev;
    int32_t ECD_diff;
    int32_t ECD_diff_prev;
};

class SpeedControllerPID : public SpeedController
{
public:
    SpeedControllerPID(void);
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
    struct MotorPID _MTRA;
    struct MotorPID _MTRB;
    struct MotorPID _MTRC;
    struct MotorPID _MTRD;
    Encoder _ECDA;
    Encoder _ECDB;
    Encoder _ECDC;
    Encoder _ECDD;
    float _set_point;
};

SpeedControllerPID::SpeedControllerPID(void) : SpeedController(), _ECDA(Encoder(ECDAA, ECDAB)), _ECDB(Encoder(ECDBA, ECDBB)), _ECDC(Encoder(ECDCA, ECDCB)), _ECDD(Encoder(ECDDA, ECDDB))
{
    _time_step = 10;
}

void SpeedControllerPID::begin()
{
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

void SpeedControllerPID::reset()
{
    SpeedController::reset();
    _MTRA.integral = 0;
    _MTRB.integral = 0;
    _MTRC.integral = 0;
    _MTRD.integral = 0;
    _prev_time = millis();
}

inline void SpeedControllerPID::perform()
{
    _dT = millis() - _prev_time;
    if (_dT >= _time_step && _control_on)
    {
        _prev_time = millis();
        _read();

        // convert to revolution per step
        _MTRA.error = _set_point - (float)_MTRA.ECD_diff / (float)_dT * TIME_INTERVAL;
        _MTRB.error = _set_point - (float)_MTRB.ECD_diff / (float)_dT * TIME_INTERVAL;
        _MTRC.error = _set_point - (float)_MTRC.ECD_diff / (float)_dT * TIME_INTERVAL;
        _MTRD.error = _set_point - (float)_MTRD.ECD_diff / (float)_dT * TIME_INTERVAL;

        _MTRA.integral = _MTRA.integral + kI * _MTRA.error;
        _MTRB.integral = _MTRB.integral + kI * _MTRB.error;
        _MTRC.integral = _MTRC.integral + kI * _MTRC.error;
        _MTRD.integral = _MTRD.integral + kI * _MTRD.error;

        _MTRA.derivative = _MTRA.ECD_diff - _MTRA.ECD_diff_prev;
        _MTRB.derivative = _MTRB.ECD_diff - _MTRB.ECD_diff_prev;
        _MTRC.derivative = _MTRC.ECD_diff - _MTRC.ECD_diff_prev;
        _MTRD.derivative = _MTRD.ECD_diff - _MTRD.ECD_diff_prev;

        _PWM_A = constrain(kP * _MTRA.error + _MTRA.integral + kD * _MTRA.derivative + _PWM_A, MIN_PWM, MAX_PWM);
        _PWM_B = constrain(kP * _MTRB.error + _MTRB.integral + kD * _MTRB.derivative + _PWM_B, MIN_PWM, MAX_PWM);
        _PWM_C = constrain(kP * _MTRC.error + _MTRC.integral + kD * _MTRC.derivative + _PWM_C, MIN_PWM, MAX_PWM);
        _PWM_D = constrain(kP * _MTRD.error + _MTRD.integral + kD * _MTRD.derivative + _PWM_D, MIN_PWM, MAX_PWM);

        _MTRA.ECD_diff_prev = _MTRA.ECD_diff;
        _MTRB.ECD_diff_prev = _MTRB.ECD_diff;
        _MTRC.ECD_diff_prev = _MTRC.ECD_diff;
        _MTRD.ECD_diff_prev = _MTRD.ECD_diff;

        _writePWM();
        // _log();
    }
}

inline void SpeedControllerPID::_read()
{
    _MTRA.ECD_val_prev = _MTRA.ECD_val;
    _MTRB.ECD_val_prev = _MTRB.ECD_val;
    _MTRC.ECD_val_prev = _MTRC.ECD_val;
    _MTRD.ECD_val_prev = _MTRD.ECD_val;

    _MTRA.ECD_val = -_ECDA.read();
    _MTRB.ECD_val = _ECDB.read();
    _MTRC.ECD_val = -_ECDC.read();
    _MTRD.ECD_val = _ECDD.read();

    _MTRA.ECD_diff = _MTRA.ECD_val - _MTRA.ECD_val_prev;
    _MTRB.ECD_diff = _MTRB.ECD_val - _MTRB.ECD_val_prev;
    _MTRC.ECD_diff = _MTRC.ECD_val - _MTRC.ECD_val_prev;
    _MTRD.ECD_diff = _MTRD.ECD_val - _MTRD.ECD_val_prev;
}

void SpeedControllerPID::setPWM(int MOTOR_PWM)
{
    SpeedController::setPWM(MOTOR_PWM);
    _set_point = (float)MOTOR_PWM * MIN_MOTOR_RPPWM;
}

// WARNING: leaving the wheels in the air before run this function.
void SpeedControllerPID::measure()
{
    _off();
    setPWM(255);
    unsigned long curr_time;
    unsigned long prev_time;
    unsigned long interval;
    while (true)
    {
        curr_time = micros();
        interval = curr_time - prev_time;
        if (interval >= 10000)
        {
            prev_time = curr_time;
            _read();
            Serial.print(interval);
            Serial.print(",");
            Serial.print(_MTRA.ECD_diff);
            Serial.print(",");
            Serial.print(_MTRB.ECD_diff);
            Serial.print(",");
            Serial.print(_MTRC.ECD_diff);
            Serial.print(",");
            Serial.println(_MTRD.ECD_diff);
        }
    }
}

#endif