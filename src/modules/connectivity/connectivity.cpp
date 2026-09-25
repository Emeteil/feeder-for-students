#include "connectivity.h"

#include <WiFi.h>

#include "../../core/config.h"
#include "../../core/app_state.h"
#include "../../core/event_bus.h"

namespace Connectivity
{
    namespace
    {
        // Пример неблокирующего переподключения: НЕ используем
        // WiFi.waitForConnectResult() (он блокирует), а просто
        // периодически проверяем WiFi.status() внутри Update().
        const unsigned long RECONNECT_INTERVAL_MS = 5000;
        unsigned long lastAttemptAtMs = 0;
        bool wasConnected = false;

        void TryConnect()
        {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            lastAttemptAtMs = millis();
        }
    }

    void Init()
    {
        WiFi.mode(WIFI_STA);
        TryConnect();
    }

    void Update()
    {
        bool isConnected = WiFi.status() == WL_CONNECTED;
        State.wifiConnected = isConnected;

        if (isConnected && !wasConnected)
        {
            EventBus::Publish({EventBus::EventType::NetworkConnected});
        }
        else if (!isConnected && wasConnected)
        {
            EventBus::Publish({EventBus::EventType::NetworkDisconnected});
        }
        wasConnected = isConnected;

        if (!isConnected && millis() - lastAttemptAtMs >= RECONNECT_INTERVAL_MS)
        {
            TryConnect();
        }
    }
}
