#include "virtual_memory.hpp"

#include "mmu.hpp"
#include "physical_memory.hpp"

namespace
{
    const uint64_t VIRTUAL_MEMORY_START = 0x80000000;
    const uint64_t VIRTUAL_MEMORY_END = 0x81000000;

    const uint64_t PAGE_SIZE = 4096;
    const uint64_t VIRTUAL_PAGE_COUNT = (VIRTUAL_MEMORY_END - VIRTUAL_MEMORY_START) / PAGE_SIZE;
    const uint64_t VIRTUAL_BITMAP_SIZE = VIRTUAL_PAGE_COUNT / 8;
}

namespace VirtualBitmap
{
    uint8_t virtual_bitmap[VIRTUAL_BITMAP_SIZE];
}

namespace
{
    void set_page_used(uint64_t page)
    {
        const uint64_t byte_index = page / 8;
        const uint8_t bit_index = page % 8;

        VirtualBitmap::virtual_bitmap[byte_index] |= (uint8_t)(1U << bit_index);
    }

    void set_page_free(uint64_t page)
    {
        const uint64_t byte_index = page / 8;
        const uint8_t bit_index = page % 8;

        VirtualBitmap::virtual_bitmap[byte_index] &= (uint8_t)(~(1U << bit_index));
    }

    bool is_page_used(uint64_t page)
    {
        const uint64_t byte_index = page / 8;
        const uint8_t bit_index = page % 8;

        return (VirtualBitmap::virtual_bitmap[byte_index] & (uint8_t)(1U << bit_index)) != 0;
    }

    uint64_t find_free_page()
    {
        for (uint64_t page = 0; page < VIRTUAL_PAGE_COUNT; page++)
        {
            if (!is_page_used(page))
            {
                return page;
            }
        }

        return VIRTUAL_PAGE_COUNT;
    }
}

void VirtualMemory::init()
{
    for (uint64_t i = 0; i < VIRTUAL_BITMAP_SIZE; i++)
    {
        VirtualBitmap::virtual_bitmap[i] = 0;
    }
}

uint64_t VirtualMemory::allocate_page()
{
    const uint64_t page = find_free_page();
    if (page == VIRTUAL_PAGE_COUNT)
    {
        return 0;
    }

    const uint64_t frame = PhysicalMemory::allocate_frame();
    if (frame == 0)
    {

        return 0;
    }

    const uint64_t virtual_address = VIRTUAL_MEMORY_START + (page * PAGE_SIZE);

    if (MMU::map_page(virtual_address, frame, MMU::EMemoryType::Normal, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN))
    {
        set_page_used(page);
        return virtual_address;
    }

    PhysicalMemory::free_frame(frame);

    return 0;
}

bool VirtualMemory::free_page(uint64_t virtual_address)
{
    if (virtual_address < VIRTUAL_MEMORY_START || virtual_address >= VIRTUAL_MEMORY_END)
    {
        return false;
    }

    if ((virtual_address & (PAGE_SIZE - 1)) != 0)
    {
        return false;
    }

    const uint64_t page = (virtual_address - VIRTUAL_MEMORY_START) / PAGE_SIZE;
    if (!is_page_used(page))
    {
        return false;
    }

    const uint64_t physical_address = MMU::get_physical_address(virtual_address);
    if (physical_address == 0)
    {
        return false;
    }

    if (!MMU::unmap_page(virtual_address))
    {
        return false;
    }

    PhysicalMemory::free_frame(physical_address);
    set_page_free(page);

    return true;
}