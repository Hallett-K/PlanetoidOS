#pragma once

#include <cstdint>

namespace VirtualMemory
{
    void init();

    uint64_t allocate_page();
    uint64_t allocate_pages(uint64_t page_count);
    bool free_page(uint64_t virtual_address);
    bool free_pages(uint64_t virtual_address, uint64_t page_count);
};