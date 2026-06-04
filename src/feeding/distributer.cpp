#include "distributer.h"
#include "./internal/distributer.h"
#include "ESP32Servo.h"
#include "secrets.h"

//

namespace fed
{
    Servo distributerServo;

    int distributerServoPin = ATT_DISTRIBUTER_SERVO_PIN;
    int distributerServoTarget = 0;

    //

    void writeDistributerServo(int state)
    {
        distributerServoTarget = state;
        distributerServo.write(distributerServoTarget);
    }
}

namespace fed::internal
{
    void setupDistributerServo()
    {
        ESP32PWM::allocateTimer(1);
        distributerServo.setPeriodHertz(50);
        distributerServo.attach(distributerServoPin, 500, 2400);
    }
}
