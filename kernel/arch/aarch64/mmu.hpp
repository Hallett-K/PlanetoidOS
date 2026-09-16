#pragma once 

#include <cstdint>

namespace MMU
{
    alignas(4096) extern uint64_t level1_table[512];
    alignas(4096) extern uint64_t level2_table[512];
    alignas(4096) extern uint64_t level2_ram_table[512];

    void init();
    void enable();
}