#pragma once

#include <stdint.h>

/*
 * AppState - общее состояние прошивки, "единственный источник правды".
 *
 * Любой модуль может ПРОЧИТАТЬ любое поле напрямую (State.wifiConnected).
 * Но ИЗМЕНЯТЬ поле должен только тот модуль, который за него отвечает
 * (по договорённости - см. комментарий у каждого поля). Если это правило
 * не устраивает вашу задачу - обсудите с командой перед тем как нарушать.
 *
 * Для уведомления "что-то произошло" (а не просто "какое-то значение
 * изменилось") используйте EventBus (см. src/core/event_bus.h), а не
 * добавление ещё одного bool-флага сюда.
 */
struct AppState
{
    // Отвечает: src/modules/connectivity
    bool wifiConnected = false;

    // Отвечает: src/modules/dispenser
    bool dispenserBusy = false;
    uint16_t totalDispensedCount = 0;

    // Отвечает: src/modules/power
    float batteryVoltage = 0.0f;
};

extern AppState State;
