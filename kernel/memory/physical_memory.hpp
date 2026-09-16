#pragma once

#include <cstdint>

namespace PhysicalMemory
{
    void init();
    uint64_t allocate_frame();
    void free_frame(uint64_t address);
}