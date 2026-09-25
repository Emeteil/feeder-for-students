#pragma once

/*
 * Общие настройки проекта, которые НЕ являются секретами -
 * их можно и нужно коммитить в git.
 *
 * Секреты (Wi-Fi пароль, токен телеграм-бота и т.п.) сюда не пишем!
 * Они живут в src/core/config_local.h - этот файл в .gitignore
 * и никогда не попадёт в репозиторий. Смотрите
 * src/core/config_local.h.example - скопируйте его в config_local.h
 * и подставьте свои значения.
 */

#define SERIAL_BAUD_RATE 115200

// Как часто (мс) можно печатать debug-лог одного и того же события,
// чтобы не засорять Serial Monitor. Используйте вместе с millis(),
// а не delay() - см. docs/architecture.md, раздел "Блокирующие операции".
#define DEBUG_LOG_THROTTLE_MS 1000

#include "config_local.h"
