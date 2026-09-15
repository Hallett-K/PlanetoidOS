#pragma once

#include <cstdint>

namespace Timer
{
    void init(uint32_t frequency);
    void on_interrupt();
    uint64_t get_ticks();
    void delay(uint64_t ticks);
    uint32_t get_frequency();
    uint64_t get_milliseconds();
}