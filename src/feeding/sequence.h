#pragma once
#include "command.h"

//

namespace fed::internal
{
    void startSequence(const Command &command);
    void loopSequence();
    bool isSequenceRunning();
}
