#include "physical_memory.hpp"

#include "core/log.hpp"
#include "memory_map.hpp"

namespace KernelMemory
{
    extern "C"
    {
        extern uint8_t __text_start;
        extern uint8_t __stack_top;
    }
}

namespace
{
#if OS_PLATFORM_PI5
    const uint64_t PHYSICAL_MEMORY_BASE = 0x00000000;
#else
    const uint64_t PHYSICAL_MEMORY_BASE = 0x40000000;
#endif
    const uint64_t PAGE_SIZE = 4096;
    uint64_t frame_count = 0;

    const uint64_t KERNEL_START = (uint64_t)&KernelMemory::__text_start;
    const uint64_t KERNEL_END = (uint64_t)&KernelMemory::__stack_top;
};

namespace PhysicalBitmap
{
    extern "C"
    {
        extern uint8_t __physical_bitmap_start;
        extern uint8_t __physical_bitmap_end;
    }

    uint8_t* const bitmap_start = &__physical_bitmap_start;
}

namespace
{
    uint64_t get_bitmap_size()
    {
        return (uint64_t)&PhysicalBitmap::__physical_bitmap_end - (uint64_t)&PhysicalBitmap::__physical_bitmap_start;
    }

    uint64_t get_frame_number(uint64_t address)
    {
        return (address - PHYSICAL_MEMORY_BASE) / PAGE_SIZE;
    }

    void set_frame_used(uint64_t frame)
    {
        const uint64_t byte_index = frame / 8;
        const uint8_t bit_index = frame % 8;

        PhysicalBitmap::bitmap_start[byte_index] |= (uint8_t)(1U << bit_index);
    }

    void set_frame_range_used(uint64_t address, uint64_t size)
    {
        const uint64_t aligned_start = address & ~(PAGE_SIZE - 1);
        const uint64_t aligned_end = (address + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        const uint64_t first_frame = get_frame_number(aligned_start);
        const uint64_t frame_count_in_range = (aligned_end - aligned_start) / PAGE_SIZE;

        for (uint64_t frame = 0; frame < frame_count_in_range; frame++)
        {
            set_frame_used(first_frame + frame);
        }
    }

    void set_frame_free(uint64_t frame)
    {
        const uint64_t byte_index = frame / 8;
        const uint8_t bit_index = frame % 8;

        PhysicalBitmap::bitmap_start[byte_index] &= (uint8_t)(~(1U << bit_index));
    }

    bool is_frame_used(uint64_t frame)
    {
        const uint64_t byte_index = frame / 8;
        const uint8_t bit_index = frame % 8;

        return (PhysicalBitmap::bitmap_start[byte_index] & (uint8_t)(1U << bit_index)) != 0;
    }
}

void PhysicalMemory::init()
{
    const uint64_t bitmap_size = get_bitmap_size();

    for (uint64_t i = 0; i < bitmap_size; i++)
    {
        PhysicalBitmap::bitmap_start[i] = 0xFF;
    }

    const Memory::MemoryMap& memory_map = Memory::get_memory_map();
    frame_count = 0;

    for (uint32_t i = 0; i < memory_map.count; i++)
    {
        const Memory::MemoryRegion& region = memory_map.regions[i];
        if (region.type != Memory::ERegionType::Usable)
        {
            continue;
        }

        const uint64_t first_frame = get_frame_number(region.base);
        const uint64_t frame_count_in_region = region.size / PAGE_SIZE;
        frame_count += frame_count_in_region;

        for (uint64_t frame = 0; frame < frame_count_in_region; frame++)
        {
            set_frame_free(first_frame + frame);
        }
    }

#if OS_PLATFORM_PI5
    set_frame_range_used(0x00000000, KERNEL_START);
#endif

    set_frame_range_used(KERNEL_START, KERNEL_END - KERNEL_START);
}

uint64_t PhysicalMemory::allocate_frame()
{
    for (uint64_t frame = 0; frame < frame_count; frame++)
    {
        if (is_frame_used(frame))
        {
            continue;
        }

        set_frame_used(frame);

        return PHYSICAL_MEMORY_BASE + (frame * PAGE_SIZE);
    }

    return 0;
}

void PhysicalMemory::free_frame(uint64_t address)
{
    if (address < PHYSICAL_MEMORY_BASE)
    {
        return;
    }

    if ((address % PAGE_SIZE) != 0)
    {
        return;
    }

    const uint64_t frame = get_frame_number(address);

    if (frame >= frame_count)
    {
        return;
    }

    set_frame_free(frame);
}