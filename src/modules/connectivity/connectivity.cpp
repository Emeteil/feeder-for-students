#include "connectivity.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "../../core/config.h"
#include "../../core/app_state.h"
#include "../../core/event_bus.h"
#include "../../utils/log.h"

namespace Connectivity
{
    namespace
    {
        constexpr char LOG_MODULE[] = "Connectivity";

        // Пример неблокирующего переподключения: НЕ используем
        // WiFi.waitForConnectResult() (он блокирует), а просто
        // периодически проверяем WiFi.status() внутри Update().
        const unsigned long RECONNECT_INTERVAL_MS = 5000;
        unsigned long lastAttemptAtMs = 0;
        bool wasConnected = false;

        void TryConnect()
        {
            Log::Debug(LOG_MODULE, "Connecting to Wi-Fi SSID '%s'...", WIFI_SSID);
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
            Log::Info(LOG_MODULE, "Network connected");
        }
        else if (!isConnected && wasConnected)
        {
            EventBus::Publish({EventBus::EventType::NetworkDisconnected});
            Log::Warning(LOG_MODULE, "Network disconnected");
        }
        wasConnected = isConnected;

        if (!isConnected && millis() - lastAttemptAtMs >= RECONNECT_INTERVAL_MS)
        {
            TryConnect();
        }
    }
}
