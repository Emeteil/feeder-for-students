#include "telegram_bot.h"

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <AsyncTelegram2.h>
#include <inttypes.h>

#include "../../core/config.h"
#include "../../core/app_state.h"
#include "../../utils/log.h"

namespace TelegramBot
{
    namespace
    {
        constexpr char LOG_MODULE[] = "TelegramBot";

        // Раз в сколько миллисекунд можно снова пробовать подключиться к Telegram
        const unsigned long START_RETRY_MS = 5000;

        // Если столько миллисекунд новых сообщений нет, очередь старых считается разобранной
        const unsigned long DRAIN_QUIET_MS = 1000;

        WiFiClientSecure client;
        AsyncTelegram2 bot(client);

        bool botStarted = false;
        unsigned long lastStartAttemptAtMs = 0;

        // Пока draining == true, все входящие сообщения читаем и выбрасываем.
        // Так разбирается очередь команд, накопившаяся до перезагрузки.
        bool draining = false;
        unsigned long lastMessageSeenAtMs = 0;

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
                Log::Info(LOG_MODULE, "Подключились к Telegram, разбираю старую очередь");
            }
            else
            {
                Log::Warning(LOG_MODULE, "Не удалось подключиться к Telegram, повторю позже");
            }
        }

        void DrainTick(MessageType type)
        {
            if (type != MessageNoData)
            {
                lastMessageSeenAtMs = millis();
                Log::Debug(LOG_MODULE, "Пропускаю сообщение, пришедшее до старта");
                return;
            }

            if (millis() - lastMessageSeenAtMs >= DRAIN_QUIET_MS)
            {
                draining = false;
                Log::Info(LOG_MODULE, "Очередь старых сообщений разобрана");
            }
        }

        void HandleMessage(const TBMessage &msg)
        {
            // У групп и каналов chat_id отрицательный - такие сообщения не обрабатываем.
            if (msg.chatId < 0)
            {
                Log::Debug(LOG_MODULE, "Сообщение не из личного чата, пропускаю");
                return;
            }

            Log::Debug(LOG_MODULE, "Команда от пользователя %" PRId64 ": %s", msg.sender.id, msg.text.c_str());

            if (msg.text == "/start")
            {
                bot.sendMessage(msg, "Привет!");
                return;
            }

            if (msg.text == "/help")
            {
                bot.sendMessage(msg, "Я бот проекта Feeder for Students.");
                return;
            }

            String reply;
            reply += "Данные входящего сообщения";
            reply += "\nID сообщения: ";
            reply += msg.messageID;
            reply += "\nID чата: ";
            reply += msg.chatId;
            reply += "\nID отправителя: ";
            reply += msg.sender.id;
            reply += "\nИмя: ";
            reply += msg.sender.firstName;
            reply += "\nФамилия: ";
            reply += msg.sender.lastName;
            reply += "\nИмя пользователя: ";
            reply += msg.sender.username;
            reply += "\nТип сообщения: ";
            reply += static_cast<int>(msg.messageType);
            reply += "\nВремя отправки (Unix): ";
            reply += msg.date;
            reply += "\nВремя микроконтроллера: ";
            reply += millis();
            reply += "\nТекст:\n";
            reply += msg.text;

            bot.sendMessage(msg, reply.c_str());
        }
    }

    void Init()
    {
        client.setInsecure();

        // Вариант 1: поменять хост на свой релей
        #ifdef TELEGRAM_RELAY_HOST
        bot.setTelegramHost(TELEGRAM_RELAY_HOST);
        #endif

        // Вариант 2: CORS-прокси, адрес Telegram добавляется префиксом к пути
        #ifdef TELEGRAM_CORS_PROXY_HOST
        bot.setTelegramHost(TELEGRAM_CORS_PROXY_HOST);
        bot.setCorsProxyPrefix(TELEGRAM_CORS_PROXY_PREFIX);
        #endif

        bot.setTelegramToken(TELEGRAM_BOT_TOKEN);
        bot.setUpdateTime(500);

        Log::Info(LOG_MODULE, "Модуль инициализирован");
    }

    void Update()
    {
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
