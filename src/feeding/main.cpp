#include "main.h"
#include "command.h"
#include "sequence.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "ws.h"
#include "./internal/refiller.h"
#include "./internal/distributer.h"
#include "./internal/motor.h"
#include "./internal/limitter.h"

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
        fed::internal::setupRefillerServo();
        fed::internal::setupDistributerServo();
        fed::internal::setupMotor();
        fed::internal::setupLimitter();
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
