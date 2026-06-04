#include "refiller.h"
#include "./internal/refiller.h"
#include "ESP32Servo.h"
#include "secrets.h"

//

namespace fed
{
    Servo refillerServo;

    int refillerServoPin = ATT_REFILLER_SERVO_PIN;
    int refillerServoTarget = 0;

    //

    void writeRefillerServo(int state)
    {
        refillerServoTarget = state;
        refillerServo.write(refillerServoTarget);
    }
}

namespace fed::internal
{
    void setupRefillerServo()
    {
        ESP32PWM::allocateTimer(0);
        refillerServo.setPeriodHertz(50);
        refillerServo.attach(refillerServoPin, 500, 2400);
    }
}
