#include "memory_map.hpp"

namespace
{
    Memory::MemoryMap memory_map =
    {
        {
            {
                0x40000000,
                0x100000000,
                Memory::ERegionType::Usable
            }
        },
        1
    };

    const uint64_t PAGE_SIZE = 4096;
}

const Memory::MemoryMap& Memory::get_memory_map()
{
    return memory_map;
}