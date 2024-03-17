
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

#define DIRA1 34 // Motor A Direction +
#define DIRA2 35 // Motor A Direction -
#define DIRB1 36 // Motor B Direction +
#define DIRB2 37 // Motor B Direction -
#define DIRC1 43 // Motor C Direction +
#define DIRC2 42 // Motor C Direction -
#define DIRD1 A5 // Motor D Direction +
#define DIRD2 A4 // Motor D Direction -

#define PWMA 12 // Motor A PWM
#define PWMB 8  // Motor B PWM
#define PWMC 6  // Motor C PWM
#define PWMD 5  // Motor D PWM

unsigned long curr_time = 0;
unsigned long prev_time = 0;
unsigned long interval = 0;

Encoder ECD_A;
Encoder ECD_B;
Encoder ECD_C;
Encoder ECD_D;

long ECDA = 0;
long ECDB = 0;
long ECDC = 0;
long ECDD = 0;

long ECDA_prev = 0;
long ECDB_prev = 0;
long ECDC_prev = 0;
long ECDD_prev = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(ECDAA, INPUT);
    pinMode(ECDAB, INPUT);
    pinMode(ECDBA, INPUT);
    pinMode(ECDBB, INPUT);
    pinMode(ECDCA, INPUT);
    pinMode(ECDCB, INPUT);
    pinMode(ECDDA, INPUT);
    pinMode(ECDDB, INPUT);

    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(PWMC, OUTPUT);
    pinMode(PWMD, OUTPUT);

    pinMode(DIRA1, OUTPUT);
    pinMode(DIRA2, OUTPUT);
    pinMode(DIRB1, OUTPUT);
    pinMode(DIRB2, OUTPUT);
    pinMode(DIRC1, OUTPUT);
    pinMode(DIRC2, OUTPUT);
    pinMode(DIRD1, OUTPUT);
    pinMode(DIRD2, OUTPUT);
}

void loop()
{
    curr_time = micros();
    interval = curr_time - prev_time;
    if (interval >= 10000)
    {
        prev_time = curr_time;

        ECDA_prev = ECDA;
        ECDB_prev = ECDB;
        ECDC_prev = ECDC;
        ECDD_prev = ECDD;

        ECDA = -ECD_A.read();
        ECDB = ECD_B.read();
        ECDC = -ECD_C.read();
        ECDD = ECD_D.read();

        Serial.print(interval);
        Serial.print(",");
        Serial.print(ECDA - ECDA_prev);
        Serial.print(",");
        Serial.print(ECDB - ECDB_prev);
        Serial.print(",");
        Serial.print(ECDC - ECDC_prev);
        Serial.print(",");
        Serial.println(ECDD - ECDD_prev);
    }
}