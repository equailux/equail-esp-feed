#include "command.h"

//

namespace fed::internal
{
    bool readDuration(JsonObject data, const char *name, unsigned long &value)
    {
        JsonVariant field = data[name];
        if (field.isNull())
            return false;
        if (!field.is<long>())
            return false;

        long duration = field.as<long>();
        if (duration < 0)
            return false;

        value = static_cast<unsigned long>(duration);
        return true;
    }

    bool readCommand(JsonArray data, Command &command)
    {
        if (data.size() <= 0) return false;

        JsonVariant first = data[0];
        if (!first.is<JsonObject>()) return false;

        JsonObject config = first.as<JsonObject>();

        bool servo1 = readDuration(config, "servo1Ms", command.servo1Ms);
        bool servo2 = readDuration(config, "servo2Ms", command.servo2Ms);
        bool stepper = readDuration(config, "stepperMs", command.stepperMs);

        return servo1 && servo2 && stepper;
    }
}
