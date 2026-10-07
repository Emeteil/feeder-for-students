#include "unsafe_eeprom.h"

namespace UnsafeEeprom
{
    void Init(size_t sizeBytes)
    {
        EEPROM.begin(sizeBytes);
    }

    void Commit()
    {
        EEPROM.commit();
    }
}
