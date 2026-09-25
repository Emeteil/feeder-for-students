#pragma once

#include <stdint.h>

/*
 * EventBus - общая "доска объявлений" для модулей.
 *
 * Зачем это нужно: если Display будет напрямую вызывать функции
 * Dispenser, а Dispenser - функции Display, модули превратятся
 * в клубок взаимных #include и любое изменение одного модуля будет
 * ломать другой. Вместо этого модуль публикует событие ("выдай снек
 * из отсека 2"), а кто угодно (хоть Dispenser, хоть TelegramBot,
 * хоть новый модуль, который вы добавите через месяц) может на это
 * событие подписаться. Модули не знают о существовании друг друга -
 * это и есть тот самый "общий API для взаимодействия".
 *
 * Как добавить новое взаимодействие между модулями:
 *   1. Добавьте новый EventType в перечисление ниже (перед EventTypeCount).
 *   2. В модуле-источнике вызовите EventBus::Publish({EventType::Ваш, ...}).
 *   3. В модуле-получателе в Init() вызовите
 *      EventBus::Subscribe(EventType::Ваш, ВашОбработчик).
 * Новый модуль при этом никак не трогает код существующих модулей.
 *
 * Подробнее и с примером - docs/architecture.md.
 */
namespace EventBus
{
    enum class EventType : uint8_t
    {
        // Датчик препятствия сработал на пине/индексе `intValue`.
        ObstacleDetected = 0,

        // Wi-Fi подключился/отключился.
        NetworkConnected,
        NetworkDisconnected,

        // Сюда добавляйте новые события.

        // Всегда должно быть последним - используется для размера таблиц.
        EventTypeCount
    };

    struct Event
    {
        EventType type;
        int32_t intValue = 0;  // смысл зависит от типа события, см. комментарии выше
        int32_t intValue2 = 0;
    };

    using EventHandler = void (*)(const Event& event);

    // Вызывается один раз из App::Init(), до Init() остальных модулей.
    void Init();

    // Подписывает handler на событие типа type.
    // Возвращает false, если для этого типа события уже нет свободных слотов
    // (см. MAX_SUBSCRIBERS_PER_EVENT в event_bus.cpp) - увеличьте константу.
    bool Subscribe(EventType type, EventHandler handler);

    // Синхронно вызывает всех подписчиков события. Обработчики должны быть
    // короткими и не блокирующими (никакого delay() внутри!) - так же, как
    // и Update() модулей.
    void Publish(const Event& event);
}
