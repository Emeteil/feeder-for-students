#include "log.h"

#include <Arduino.h>
#include <stdarg.h>

#include "../core/config.h"

namespace Log
{
    namespace
    {
        const char *LevelName(Level level)
        {
            switch (level)
            {
                case Level::Debug:
                    return "DEBUG";
                case Level::Info:
                    return "INFO";
                case Level::Warning:
                    return "WARN";
                case Level::Error:
                    return "ERROR";
                case Level::Off:
                    return "OFF";
            }
            return "UNKNOWN";
        }

        void WriteV(Level level, const char *module, const char *format, va_list args)
        {
            if (level < LOG_LEVEL || level == Level::Off)
            {
                return;
            }

            Serial.printf("%s | %s | ", module, LevelName(level));
            Serial.vprintf(format, args);
            Serial.println();
        }
    }

    void Init()
    {
        Serial.begin(SERIAL_BAUD_RATE);
    }

    void Write(Level level, const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        WriteV(level, module, format, args);
        va_end(args);
    }

    void Debug(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        WriteV(Level::Debug, module, format, args);
        va_end(args);
    }

    void Info(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        WriteV(Level::Info, module, format, args);
        va_end(args);
    }

    void Warning(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        WriteV(Level::Warning, module, format, args);
        va_end(args);
    }

    void Error(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        WriteV(Level::Error, module, format, args);
        va_end(args);
    }
}
