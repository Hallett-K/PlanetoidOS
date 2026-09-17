#pragma once

#include <cstdint>

namespace VirtualMemory
{
    void init();

    uint64_t allocate_page();
    bool free_page(uint64_t virtual_address);
};