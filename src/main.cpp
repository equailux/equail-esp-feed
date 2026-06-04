#include <Arduino.h>
#include "./station.h"
#include "./websocket.h"
#include "./feeding/main.h"
#include "./feeding/sequence.h"

//

void setup()
{
    Serial.begin(115200);

    sta::setup();
    wsc::setup();
    fed::setup();
}

void loop()
{
    fed::loop();
    if (!wsc::loop()) return;
}
