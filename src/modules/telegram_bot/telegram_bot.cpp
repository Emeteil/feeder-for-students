#include "telegram_bot.h"

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <AsyncTelegram2.h>
#include <ESP32Servo.h>

#include "../../core/config.h"
#include "../../core/app_state.h"
#include "../../core/pins.h"

namespace TelegramBot
{
    namespace
    {
        // Раз в сколько миллисекунд можно снова пробовать подключиться к Telegram
        const unsigned long START_RETRY_MS = 5000;

        // Через сколько миллисекунд серво возвращается в 0 градусов
        const unsigned long SERVO_RETURN_DELAY_MS = 1500;

        // Если столько миллисекунд новых сообщений нет, очередь старых считается разобранной
        const unsigned long DRAIN_QUIET_MS = 3000;

        WiFiClientSecure client;
        AsyncTelegram2 bot(client);

        bool botStarted = false;
        unsigned long lastStartAttemptAtMs = 0;

        // Пока draining == true, все входящие сообщения читаем и выбрасываем.
        // Так разбирается очередь команд, накопившаяся до перезагрузки.
        bool draining = false;
        unsigned long lastMessageSeenAtMs = 0;

        Servo testServo;
        bool isServoRotated = false;
        unsigned long servoRotatedAtMs = 0;

        void UpdateServo()
        {
            if (isServoRotated && millis() - servoRotatedAtMs >= SERVO_RETURN_DELAY_MS)
            {
                testServo.write(0);
                isServoRotated = false;
                Serial.println("TelegramBot | Сервопривод вернулся в 0 градусов");
            }
        }

        void StartBot()
        {
            if (botStarted)
            {
                return;
            }
            if (millis() - lastStartAttemptAtMs < START_RETRY_MS)
            {
                return;
            }

            lastStartAttemptAtMs = millis();

            // begin() делает TLS-рукопожатие и блокирует loop() на 1-4 секунды.
            // Вызывается только при (пере)подключении.
            botStarted = bot.begin();

            if (botStarted)
            {
                draining = true;
                lastMessageSeenAtMs = millis();
                Serial.println("TelegramBot | Подключились к Telegram, разбираю старую очередь");
            }
            else
            {
                Serial.println("TelegramBot | Не удалось подключиться к Telegram, повторю позже");
            }
        }

        void DrainTick(MessageType type)
        {
            if (type != MessageNoData)
            {
                lastMessageSeenAtMs = millis();
                Serial.println("TelegramBot | Пропускаю сообщение, пришедшее до старта");
                return;
            }

            if (millis() - lastMessageSeenAtMs >= DRAIN_QUIET_MS)
            {
                draining = false;
                Serial.println("TelegramBot | Очередь старых сообщений разобрана");
            }
        }

        void HandleMessage(const TBMessage &msg)
        {
            // У групп и каналов chat_id отрицательный - такие сообщения не обрабатываем.
            if (msg.chatId < 0)
            {
                Serial.println("TelegramBot | Сообщение не из личного чата, пропускаю");
                return;
            }

            Serial.print("TelegramBot | Команда от пользователя ");
            Serial.print(msg.sender.id);
            Serial.print(": ");
            Serial.println(msg.text);

            if (msg.text == "/start")
            {
                bot.sendMessage(msg, "Привет!");
            }
            else if (msg.text == "/rotate")
            {
                int randomAngle = random(0, 181);
                testServo.write(randomAngle);
                isServoRotated = true;
                servoRotatedAtMs = millis();
                String reply = "Вращаю сервопривод на " + String(randomAngle) + " градусов!";
                bot.sendMessage(msg, reply);
            }
        }
    }

    void Init()
    {
        client.setInsecure();

        // Вариант 1: поменять хост на свой релей
        // bot.setTelegramHost(TELEGRAM_RELAY_HOST);

        // Вариант 2: CORS-прокси, адрес Telegram добавляется префиксом к пути
        bot.setTelegramHost(TELEGRAM_CORS_PROXY_HOST);
        bot.setCorsProxyPrefix(TELEGRAM_CORS_PROXY_PREFIX);

        bot.setTelegramToken(TELEGRAM_BOT_TOKEN);
        bot.setUpdateTime(1000);

        testServo.attach(PIN_SERVO_TEST);
        testServo.write(0);

        Serial.println("TelegramBot | Модуль инициализирован");
    }

    void Update()
    {
        UpdateServo();

        if (!State.wifiConnected)
        {
            botStarted = false;
            return;
        }

        StartBot();
        if (!botStarted)
        {
            return;
        }

        // getNewMessage() не ждёт ответа сервера: берёт сообщение, если оно уже пришло
        TBMessage msg;
        MessageType type = bot.getNewMessage(msg);

        if (draining)
        {
            DrainTick(type);
            return;
        }

        if (type == MessageText)
        {
            HandleMessage(msg);
        }
    }
}
