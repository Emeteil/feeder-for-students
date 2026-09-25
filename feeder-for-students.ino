/*
 * Кормушка для студентов - прошивка ESP32 (Arduino Core).
 * ==========================================================
 *
 * Этот файл - единственная точка входа. Он намеренно почти пустой:
 * вся логика разложена по модулям в src/modules/, а порядок их
 * инициализации и обновления собран в src/app/app.cpp.
 *
 * Полное описание архитектуры - в docs/architecture.md.
 * Как собрать/прошить проект - в README.md.
 *
 * Важно: loop() не должен содержать delay() или другой блокирующий
 * код. См. docs/architecture.md, раздел "Блокирующие операции",
 * и пример неблокирующего таймера в src/modules/dispenser/dispenser.cpp.
 */
#include <Arduino.h>
#include "src/core/config.h"
#include "src/app/app.h"

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    App::Init();
}

void loop()
{
    App::Update();
}
