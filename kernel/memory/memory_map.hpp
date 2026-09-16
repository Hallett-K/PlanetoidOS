#pragma once

#include <cstdint>

namespace Memory
{
    enum class ERegionType : uint8_t
    {
        Usable = 0,
        Reserved,
        Device
    };

    struct MemoryRegion
    {
        uint64_t base;
        uint64_t size;
        ERegionType type;
    };

    const uint32_t MAX_MEMORY_REGIONS = 16;

    struct MemoryMap
    {
        MemoryRegion regions[MAX_MEMORY_REGIONS];
        uint32_t count;
    };

    const MemoryMap& get_memory_map();
}