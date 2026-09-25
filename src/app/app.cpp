#include "app.h"

#include "../core/event_bus.h"
#include "../core/app_state.h"

#include "../modules/connectivity/connectivity.h"
#include "../modules/display/display.h"
#include "../modules/dispenser/dispenser.h"
#include "../modules/sensors/sensors.h"
#include "../modules/telegram_bot/telegram_bot.h"
#include "../modules/power/power.h"

AppState State;

namespace App
{
    void Init()
    {
        // EventBus должен быть готов до Init() остальных модулей,
        // т.к. они подписываются на события внутри своего Init().
        EventBus::Init();

        Connectivity::Init();
        Display::Init();
        Dispenser::Init();
        Sensors::Init();
        TelegramBot::Init();
        Power::Init();
    }

    void Update()
    {
        // Порядок Update() модулей обычно не важен, т.к. они общаются
        // через события и AppState, а не напрямую друг с другом.
        Connectivity::Update();
        Display::Update();
        Dispenser::Update();
        Sensors::Update();
        TelegramBot::Update();
        Power::Update();
    }
}
