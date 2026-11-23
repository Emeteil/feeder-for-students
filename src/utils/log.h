#pragma once

namespace Log
{
    enum class Level
    {
        Debug,
        Info,
        Warning,
        Error,
        Off
    };

    void Init();

    void Write(Level level, const char *module, const char *format, ...)
        __attribute__((format(printf, 3, 4)));

    void Debug(const char *module, const char *format, ...)
        __attribute__((format(printf, 2, 3)));
    void Info(const char *module, const char *format, ...)
        __attribute__((format(printf, 2, 3)));
    void Warning(const char *module, const char *format, ...)
        __attribute__((format(printf, 2, 3)));
    void Error(const char *module, const char *format, ...)
        __attribute__((format(printf, 2, 3)));
}
