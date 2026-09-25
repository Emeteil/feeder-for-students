#include "sensors.h"

#include <Arduino.h>

#include "../../core/pins.h"
#include "../../core/event_bus.h"

namespace Sensors
{
    namespace
    {
        // Пример неблокирующего опроса раз в N миллисекунд вместо delay(N).
        const unsigned long POLL_INTERVAL_MS = 50;
        unsigned long lastPollAtMs = 0;
    }

    void Init()
    {
        // TODO: pinMode(PIN_IR_OBSTACLE_1, INPUT) и т.п., см. src/core/pins.h.
    }

    void Update()
    {
        if (millis() - lastPollAtMs < POLL_INTERVAL_MS)
        {
            return;
        }
        lastPollAtMs = millis();

        // TODO: прочитать датчик(и) и при необходимости
        // EventBus::Publish({EventBus::EventType::ObstacleDetected, sensorIndex});
    }
}
