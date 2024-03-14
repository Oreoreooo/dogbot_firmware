#ifndef __MOTOR_ENCODER_HPP__
#define __MOTOR_ENCODER_HPP__

#define ENCODER_OPTIMIZE_INTERRUPTS

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

class MotorFeedback
{
public:
    MotorFeedback(void);

    long *read();

private:
    Encoder ECDA;
    Encoder ECDB;
    Encoder ECDC;
    Encoder ECDD;
    long _ECD[4];
};

MotorFeedback::MotorEncoder(void) : ECDA(Encoder(ECDAA, ECDAB)), ECDB(Encoder(ECDBA, ECDBB)), ECDC(Encoder(ECDCA, ECDCB)), ECDD(Encoder(ECDDA, ECDDB)), _ECD{0, 0, 0, 0} {}

long *MotorFeedback::read()
{
    _ECD[0] = ECDA.read();
    _ECD[1] = ECDB.read();
    _ECD[2] = ECDC.read();
    _ECD[3] = ECDD.read();
    return _ECD;
}

#endif