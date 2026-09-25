#pragma once

/*
 * TelegramBot - общение с Telegram Bot API поверх Wi-Fi (HTTPS-запросы).
 * Задачи из чата/бота: показать вопрос, принять ответ, при правильном
 * ответе попросить Dispenser выдать снек.
 *
 * Владелец модуля: команда "сеть / телеграм-бот".
 *
 * Работает только когда State.wifiConnected == true (см. src/core/app_state.h).
 * Запросы к Telegram API должны быть неблокирующими (не ждать ответ
 * в цикле с delay()) - см. docs/architecture.md, раздел
 * "Блокирующие операции".
 */
namespace TelegramBot
{
    void Init();
    void Update();
}
