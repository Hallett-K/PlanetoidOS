#pragma once

#include <cstdarg>
#include <cstdint>

namespace Log
{
    void info(const char* format, ...);
    void error(const char* format, ...);
}