#pragma once

#include <EEPROM.h>

/*
 * UnsafeEeprom - тонкая обёртка над EEPROM.h (на ESP32 это эмуляция
 * EEPROM поверх flash-памяти, а не отдельная физическая микросхема).
 *
 * Префикс "Unsafe" не фигура речи - называя так, мы делаем риск
 * видимым в каждом месте использования (UnsafeEeprom::Write(...)
 * само напоминает "здесь легко выстрелить себе в ногу"), а не
 * прячем его за нейтральным именем. Подробнее про риски и как их
 * обойти - см. docs/utils.md.
 */
namespace UnsafeEeprom
{
    // Вызывайте РОВНО ОДИН РАЗ за всю прошивку - из App::Init()
    // (src/app/app.cpp), до Init() модулей, которые пользуются
    // UnsafeEeprom. Не вызывайте из Init() своего модуля: если так
    // сделают два модуля, второй вызов переинициализирует буфер и
    // может задать другой sizeBytes - непредсказуемо, кто из них
    // "победит", в зависимости от порядка модулей в app.cpp.
    // sizeBytes - сколько байт зарезервировать СРАЗУ под все модули
    // проекта вместе; увеличивайте по мере добавления новых пользователей.
    void Init(size_t sizeBytes);

    // Записывает value по адресу address - но только в буфер в RAM,
    // на flash попадёт только после Commit().
    template <typename T>
    void Write(int address, const T& value)
    {
        EEPROM.put(address, value);
    }

    // Читает значение типа T по адресу address.
    template <typename T>
    T Read(int address)
    {
        T value;
        EEPROM.get(address, value);
        return value;
    }

    // Реально сохраняет буфер на flash. Вызывайте только когда данные
    // действительно изменились, не на каждой итерации loop() - см.
    // docs/utils.md про ресурс перезаписи flash.
    void Commit();
}
