#include "power.h"

#include <Arduino.h>

#include "../../core/pins.h"
#include "../../core/app_state.h"

namespace Power
{
    namespace
    {
        const unsigned long POLL_INTERVAL_MS = 2000;
        unsigned long lastPollAtMs = 0;
    }

    void Init()
    {
        // TODO: pinMode для ADC-пина делителя напряжения, если нужен
        // (см. PIN_BATTERY_VOLTAGE_ADC в src/core/pins.h).
    }

    void Update()
    {
        if (millis() - lastPollAtMs < POLL_INTERVAL_MS)
        {
            return;
        }
        lastPollAtMs = millis();

        // TODO: прочитать ADC, перевести в вольты с учётом делителя напряжения,
        // записать в State.batteryVoltage.
    }
}
