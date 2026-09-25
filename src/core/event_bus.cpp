#include "event_bus.h"

#include <stddef.h>

namespace EventBus
{
    namespace
    {
        // Сколько модулей одновременно может подписаться на один тип события.
        // Статический массив вместо std::vector - на MCU не хотим ловить
        // фрагментацию кучи из-за постоянных allocate/free.
        const uint8_t MAX_SUBSCRIBERS_PER_EVENT = 10;

        EventHandler handlers[static_cast<size_t>(EventType::EventTypeCount)][MAX_SUBSCRIBERS_PER_EVENT];
        uint8_t handlerCount[static_cast<size_t>(EventType::EventTypeCount)];
    }

    void Init()
    {
        for (size_t type = 0; type < static_cast<size_t>(EventType::EventTypeCount); type++)
        {
            handlerCount[type] = 0;
            for (uint8_t slot = 0; slot < MAX_SUBSCRIBERS_PER_EVENT; slot++)
            {
                handlers[type][slot] = nullptr;
            }
        }
    }

    bool Subscribe(EventType type, EventHandler handler)
    {
        size_t typeIndex = static_cast<size_t>(type);
        if (handlerCount[typeIndex] >= MAX_SUBSCRIBERS_PER_EVENT)
        {
            return false;
        }

        handlers[typeIndex][handlerCount[typeIndex]] = handler;
        handlerCount[typeIndex]++;
        return true;
    }

    void Publish(const Event& event)
    {
        size_t typeIndex = static_cast<size_t>(event.type);
        for (uint8_t slot = 0; slot < handlerCount[typeIndex]; slot++)
        {
            handlers[typeIndex][slot](event);
        }
    }
}
