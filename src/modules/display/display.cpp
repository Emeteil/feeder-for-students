#include "display.h"

#include "../../core/pins.h"
#include "../../core/event_bus.h"

namespace Display
{
    namespace
    {
        // Пример подписки на событие: EventBus сам вызовет эту функцию,
        // когда Connectivity опубликует NetworkConnected/NetworkDisconnected.
        void OnNetworkConnected(const EventBus::Event& event)
        {
            (void)event;
            // TODO: показать иконку "Wi-Fi подключён"
        }

        void OnNetworkDisconnected(const EventBus::Event& event)
        {
            (void)event;
            // TODO: показать иконку "нет Wi-Fi"
        }
    }

    void Init()
    {
        EventBus::Subscribe(EventBus::EventType::NetworkConnected, OnNetworkConnected);
        EventBus::Subscribe(EventBus::EventType::NetworkDisconnected, OnNetworkDisconnected);

        // TODO: инициализация дисплея (SPI, библиотека TFT/LVGL и т.п.),
        // пины брать из src/core/pins.h.
    }

    void Update()
    {
        // TODO: неблокирующая отрисовка текущего экрана и опрос тача.
        // НЕ используйте delay()! Пример неблокирующей техники - в
        // docs/architecture.md, раздел "Блокирующие операции".
    }
}
