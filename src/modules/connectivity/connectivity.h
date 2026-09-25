#pragma once

/*
 * Connectivity - подключение к Wi-Fi.
 *
 * Владелец модуля: команда "сеть / телеграм-бот".
 *
 * Публичный API модуля - это ровно то, что видно в этом .h файле.
 * Другим модулям не нужно (и не должно) знать, как именно внутри
 * устроено подключение - только AppState.wifiConnected и события
 * NetworkConnected/NetworkDisconnected (см. src/core/event_bus.h).
 *
 * Модуль называется Connectivity, а не Network - имя Network уже
 * занято глобальным объектом `Network` из самого ядра ESP32 Arduino
 * (esp32-arduino core 3.x, заголовок Network.h).
 */
namespace Connectivity
{
    void Init();
    void Update();
}
