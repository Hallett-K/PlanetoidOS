#pragma once

#include <cstdarg>
#include <cstdint>

namespace Log
{
    void info(const char* message);
    void error(const char* message);

    void info_s(const char* format, ...);
    void error_s(const char* format, ...);
}