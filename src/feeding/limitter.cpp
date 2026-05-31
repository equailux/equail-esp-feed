#include "limitter.h"
#include "./internal/limitter.h"
#include <Arduino.h>
#include "secrets.h"

//

namespace fed
{
    int leftLimitterPin = SSR_LIMITTER1_PIN;
    int rightLimitterPin = SSR_LIMITTER2_PIN;

    //

    bool readLeftLimitter()
    {
        return digitalRead(leftLimitterPin) == LOW;
    }

    bool readRightLimitter()
    {
        return digitalRead(rightLimitterPin) == LOW;
    }
}

namespace fed::internal
{
    void setupLimitter()
    {
        pinMode(leftLimitterPin, INPUT_PULLUP);
        pinMode(rightLimitterPin, INPUT_PULLUP);
    }
}
