#include "telegram_bot.h"

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h>

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

         // Настройки сервопривода (для теста)
        Servo testServo;
        const int SERVO_PIN = 18; // номер пина, куда подключен сигнальный провод серво
        bool isServoRotated = false;
        unsigned long servoRotatedAtMs = 0;
        const unsigned long SERVO_RETURN_DELAY_MS = 1500; // Вернется в исходное положение через 1.5 сек
    }

    void Init()
    {
        // TODO: инициализация HTTP-клиента (например WiFiClientSecure +
        // UniversalTelegramBot или аналог), TELEGRAM_BOT_TOKEN - из
        // src/core/config.h / config_local.h.

        client.setInsecure();

        //client.setProxy(HTTP_PROXY_URL); пока что не получается

        bot.updateToken(TELEGRAM_BOT_TOKEN);

        testServo.attach(SERVO_PIN);
        testServo.write(0);
        
        Serial.println("TelegramBot: Модуль и тестовый сервопривод инициализированы.");
    }

    void Update()
    {
        if (isServoRotated && (millis() - servoRotatedAtMs >= SERVO_RETURN_DELAY_MS))
        {
            testServo.write(0);
            isServoRotated = false;
            Serial.println("TelegramBot | Сервопривод вернулся в 0 градусов");
        }

        if (!State.wifiConnected)
        {
            return;
        }

        if (millis() - lastPollAtMs < POLL_INTERVAL_MS)
        {
            return;
        }
        lastPollAtMs = millis();

        // Запрашиваем новые сообщения один раз за тик таймера
        int newMessages = bot.getUpdates(bot.last_message_received + 1);

        if (newMessages > 0)
        {
            for (int i = 0; i < newMessages; i++)
            {
                String chat_id = String(bot.messages[i].chat_id);
                String text = bot.messages[i].text;

                Serial.print("TelegramBot | chat_id: ");
                Serial.print(chat_id);
                Serial.print(" | text: ");
                Serial.println(text);

                if (chat_id != TELEGRAM_CHAT_ID)
                {
                    Serial.println("TelegramBot | Неизвестный CHAT_ID, пропускаю");
                    continue;
                }

                if (text == "/start")
                {
                    Serial.println("TelegramBot | Отвечаю на /start");
                    bot.sendMessage(chat_id, "Привет!", "");
                }

                else if (text == "/rotate")
                {
                    Serial.println("TelegramBot | Получена команда /rotate, поворачиваю серво!");
                    bot.sendMessage(chat_id, "Вращаю сервопривод на 90 градусов!", "");
                    
                    // Поворачиваем серво на 90 градусов и запускаем таймер возврата
                    testServo.write(90);
                    isServoRotated = true;
                    servoRotatedAtMs = millis();
                }
            }
        }
    }
}
