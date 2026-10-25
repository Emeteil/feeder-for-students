#include "telegram_bot.h"

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

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

        WiFiClientSecure client;
        UniversalTelegramBot bot("", client);
    }

    void Init()
    {
        // TODO: инициализация HTTP-клиента (например WiFiClientSecure +
        // UniversalTelegramBot или аналог), TELEGRAM_BOT_TOKEN - из
        // src/core/config.h / config_local.h.

        client.setInsecure();

        client.setProxy(HTTP_PROXY_URL);

        bot.updateToken(TELEGRAM_BOT_TOKEN);

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

        // Запрашиваем новые сообщения у Telegram
        int newMessages = bot.getUpdates(bot.last_message_received + 1);

        // Обрабатываем ёочередь сообщений
        while (newMessages)
        {
            for (int i = 0; i < newMessages; i++)
            {
                String chat_id = String(bot.messages[i].chat_id);
                String text    = bot.messages[i].text;

                Serial.print("TelegramBot | chat_id: ");
                Serial.print(chat_id);
                Serial.print(" | text: ");
                Serial.println(text);

                // Защита: проверяем, что пишет именно твой CHAT_ID из настроек конфигурации
                if (chat_id != TELEGRAM_CHAT_ID)
                {
                    Serial.println("TelegramBot | Неизвестный CHAT_ID, игнорирую.");
                    continue;
                }

                // Обработка стартовой команды
                if (text == "/start")
                {
                    Serial.println("TelegramBot | Отвечаю на команду /start");
                    bot.sendMessage(chat_id, "Привет!", "");
                    
                    // ЗДЕСЬ в будущем будет логика выдачи снека:
                    // Например: Dispenser::dispense();
                }
            }
            
            // Проверяем, не пришло ли еще что-то, пока мы обрабатывали старые сообщения
            newMessages = bot.getUpdates(bot.last_message_received + 1);
        }
    }
}
