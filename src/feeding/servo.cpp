#include "servo.h"
#include "./internal/servo.h"
#include "ESP32Servo.h"
#include "secrets.h"

//

namespace fed
{
    Servo servo1;
    Servo servo2;

    int servo1Pin = ATT_SERVO1_PIN;
    int servo2Pin = ATT_SERVO2_PIN;
    int servo1Target = 0;
    int servo2Target = 0;

    //

    void writeServo(int state)
    {
        servo1.write(state <= 180 ? state : servo1Target);
        servo2.write(state >= 181 ? state % 180 : servo2Target);
        servo1Target = state <= 180 ? state : servo1Target;
        servo2Target = state >= 181 ? state % 180 : servo2Target;
    }
}

namespace fed::internal
{
    void setupServo()
    {
        ESP32PWM::allocateTimer(0);
        ESP32PWM::allocateTimer(1);
        servo1.setPeriodHertz(50);
        servo2.setPeriodHertz(50);
        servo1.attach(servo1Pin, 500, 2400);
        servo2.attach(servo2Pin, 500, 2400);
    }
}