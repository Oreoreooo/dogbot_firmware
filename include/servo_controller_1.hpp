
#include <Servo.h>

#define FOREARM_SERVO_PIN 47
#define GRIPPER_SERVO_PIN 48

class ServoController
{
public:
    ServoController();

    void setServoPosition(int pan, int tilt, int shoulder, int forearm, int gripper);

private:
    Servo forearm_;
    Servo gripper_;
};

ServoController::ServoController() :  forearm_(), gripper_()
{
    forearm_.attach(FOREARM_SERVO_PIN);
    gripper_.attach(GRIPPER_SERVO_PIN);
}

void ServoController::setServoPosition(int pan, int tilt, int shoulder, int forearm, int gripper)
{
    forearm_.write(forearm);
    gripper_.write(gripper);
}
