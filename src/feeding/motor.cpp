#include "motor.h"
#include "./internal/motor.h"
#include <Arduino.h>
#include "secrets.h"

//

namespace fed
{
    int motorPin1 = ATT_MOTOR_PIN1;
    int motorPin2 = ATT_MOTOR_PIN2;

    //

    void writeMotor(int state)
    {
        digitalWrite(motorPin1, state >= 1 ? HIGH : state <= -1 ? LOW : HIGH);
        digitalWrite(motorPin2, state >= 1 ? LOW : state <= -1 ? HIGH : HIGH);
    }
}

namespace fed::internal
{
    void setupMotor()
    {
        pinMode(motorPin1, OUTPUT);
        pinMode(motorPin2, OUTPUT);
        digitalWrite(motorPin1, HIGH);
        digitalWrite(motorPin2, HIGH);
    }
}
