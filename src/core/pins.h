#pragma once

/*
 * Распиновка платы - единственное место в проекте, где мы указываем
 * номера GPIO. Если деталь переехала на другую ножку - меняем ОДНУ
 * строчку здесь, а не ищем номер пина по всему коду.
 *
 * Заполняйте по мере того как определяетесь с конкретными компонентами.
 * Не используйте "магические числа" вида digitalWrite(5, HIGH) в модулях -
 * всегда заводите здесь PIN_... и используйте его.
 */

// ---- Дисплей (SPI) ------------------------------------------------------
// #define PIN_DISPLAY_CS   5
// #define PIN_DISPLAY_DC   17
// #define PIN_DISPLAY_RST  4
// #define PIN_DISPLAY_MOSI 23
// #define PIN_DISPLAY_SCK  18
// #define PIN_DISPLAY_MISO -1

// ---- Сенсорный экран (если отдельный контроллер тача) -------------------
// #define PIN_TOUCH_SDA 21
// #define PIN_TOUCH_SCL 22
// #define PIN_TOUCH_IRQ -1

// ---- Диспенсер (шаговики/серво по отсекам) -------------------------------
// #define PIN_STEPPER1_STEP 25
// #define PIN_STEPPER1_DIR  26
// #define PIN_STEPPER1_EN   27

// #define PIN_STEPPER2_STEP 32
// #define PIN_STEPPER2_DIR  33
// #define PIN_STEPPER2_EN   14

// #define PIN_STEPPER3_STEP 16
// #define PIN_STEPPER3_DIR  13
// #define PIN_STEPPER3_EN   19

// #define PIN_SERVO1        -1

// ---- Датчики --------------------------------------------------------------
// #define PIN_IR_OBSTACLE_1 -1