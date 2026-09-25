#include "telegram_bot.h"

#include <Arduino.h>

#include "../../core/config.h"
#include "../../core/app_state.h"
#include "../../core/event_bus.h"

namespace TelegramBot
{
    namespace
    {
        // Опрашиваем Telegram (getUpdates) раз в POLL_INTERVAL_MS, а не
        // в бесконечном блокирующем цикле - так плата успевает делать
        // остальные дела (экран, датчики, диспенсер) между опросами.
        const unsigned long POLL_INTERVAL_MS = 1000;
        unsigned long lastPollAtMs = 0;
    }

    void Init()
    {
        // TODO: инициализация HTTP-клиента (например WiFiClientSecure +
        // UniversalTelegramBot или аналог), TELEGRAM_BOT_TOKEN - из
        // src/core/config.h / config_local.h.
    }

    void Update()
    {
        if (!State.wifiConnected)
        {
            return;
        }

        if (millis() - lastPollAtMs < POLL_INTERVAL_MS)
        {
            return;
        }
        lastPollAtMs = millis();

        // TODO: опросить новые сообщения бота, обработать ответ пользователя.
        // Логика выдачи снека ещё не определена.
    }
}
