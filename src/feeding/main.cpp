#include "main.h"
#include "command.h"
#include "sequence.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "ws.h"
#include "./internal/servo.h"
#include "./internal/motor.h"

//

namespace fed
{
    void onCreate(JsonArray data)
    {
        if (isRunning())
        {
            Serial.printf("Feeding command ignored: automation already running.\n");
            return;
        }

        Command command = {0, 0, 0};

        if (!internal::readCommand(data, command))
        {
            Serial.printf("Feeding command ignored: invalid data.\n");
            return;
        }

        internal::startSequence(command);
    }

    void setup()
    {
        fed::internal::setupServo();
        fed::internal::setupMotor();
        ws::onEvent("Feed", ws::EventQuery::Create, onCreate);
    }

    void loop()
    {
        internal::loopSequence();
    }

    bool isRunning()
    {
        return internal::isSequenceRunning();
    }
}
