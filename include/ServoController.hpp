
#include <Servo.h>

// #define PAN_SERVO_PIN 2
// #define TILT_SERVO_PIN 3
// #define SHOULDER_SERVO_PIN 4
// #define FOREARM_SERVO_PIN 5
// #define GRIPPER_SERVO_PIN 6

class ServoController
{
public:
    ServoController();

    void setServoPosition(int pan, int tilt, int shoulder, int forearm, int gripper);

private:
    Servo pan_;
    Servo tilt_;
    Servo shoulder_;
    Servo forearm_;
    Servo gripper;
};

ServoController::ServoController() : pan_(), tilt_(), shoulder_(), forearm_(), gripper()
{
    // pan_.attach(PAN_SERVO_PIN);
    // tilt_.attach(TILT_SERVO_PIN);
    // shoulder_.attach(SHOULDER_SERVO_PIN);
    // forearm_.attach(FOREARM_SERVO_PIN);
    // gripper.attach(GRIPPER_SERVO_PIN);
}

void ServoController::setServoPosition(int pan, int tilt, int shoulder, int forearm, int gripper)
{
    // Set the servo position
    // panServo.write(pan);
    // tiltServo.write(tilt);
    // shoulderServo.write(shoulder);
    // forearmServo.write(forearm);
    // gripperServo.write(gripper);
}
