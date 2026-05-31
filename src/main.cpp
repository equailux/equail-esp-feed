#include <Arduino.h>
#include "Wire.h"
#include "./station.h"
#include "./websocket.h"
#include "./feeding/main.h"

//

void setup()
{
    Serial.begin(115200);
    Wire.begin(21, 22);

    sta::setup();
    wsc::setup();
    fed::setup();
}

void loop()
{
    fed::loop();
    if (!wsc::loop()) return;
}
