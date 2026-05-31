#pragma once
#include <ArduinoJson.h>

//

namespace fed
{
    struct Command
    {
        unsigned long servo1Ms;
        unsigned long servo2Ms;
        unsigned long stepperMs;
    };
}

namespace fed::internal
{
    bool readCommand(JsonArray data, Command &command);
}
