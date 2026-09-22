#include "memory_map.hpp"

namespace
{
    Memory::MemoryMap memory_map =
    {
        {
#if OS_PLATFORM_PI5
            {
                0x00000000,
                0x40000000,
                Memory::ERegionType::Usable
            }
#else
            {
                0x40000000,
                0x100000000,
                Memory::ERegionType::Usable
            }
#endif
        },
        1
    };

    const uint64_t PAGE_SIZE = 4096;
}

const Memory::MemoryMap& Memory::get_memory_map()
{
    return memory_map;
}