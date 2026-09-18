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

uint64_t VirtualMemory::allocate_pages(uint64_t page_count)
{
    if (page_count == 0)
    {
        return 0;
    }

    if (page_count > VIRTUAL_PAGE_COUNT)
    {
        return 0;
    }

    uint64_t consecutive_free_pages = 0;
    uint64_t first_page = VIRTUAL_PAGE_COUNT;
    for (uint64_t current_page = 0; current_page < VIRTUAL_PAGE_COUNT; current_page++)
    {
        if (is_page_used(current_page))
        {
            consecutive_free_pages = 0;
            continue;
        }

        consecutive_free_pages++;

        if (consecutive_free_pages == page_count)
        {
            first_page = current_page - page_count + 1;
            break;
        }
    }

    if (first_page == VIRTUAL_PAGE_COUNT)
    {
        return 0;
    }

    uint64_t allocated_page_count = 0;

    for (uint64_t page = first_page; page < first_page + page_count; page++)
    {
        const uint64_t virtual_address = VIRTUAL_MEMORY_START + (page * PAGE_SIZE);
        const uint64_t frame = PhysicalMemory::allocate_frame();
        if (frame == 0)
        {
            break;
        }

        if (!MMU::map_page(virtual_address, frame, MMU::EMemoryType::Normal, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN))
        {
            PhysicalMemory::free_frame(frame);
            break;
        }

        set_page_used(page);
        allocated_page_count++;
    }

    if (allocated_page_count != page_count)
    {
        for (uint64_t page = first_page; page < first_page + allocated_page_count; page++)
        {
            const uint64_t virtual_address = VIRTUAL_MEMORY_START + (page * PAGE_SIZE);
            const uint64_t frame = MMU::get_physical_address(virtual_address);

            MMU::unmap_page(virtual_address);
            PhysicalMemory::free_frame(frame);
            set_page_free(page);
        }

        return 0;
    }

    return VIRTUAL_MEMORY_START + (first_page * PAGE_SIZE);
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

bool VirtualMemory::free_pages(uint64_t virtual_address, uint64_t page_count)
{
    if (page_count == 0 || page_count > VIRTUAL_PAGE_COUNT)
    {
        return false;
    }

    if (virtual_address < VIRTUAL_MEMORY_START || virtual_address >= VIRTUAL_MEMORY_END)
    {
        return false;
    }

    if ((virtual_address & (PAGE_SIZE - 1)) != 0)
    {
        return false;
    }

    const uint64_t last_virtual_address = virtual_address + ((page_count - 1) * PAGE_SIZE);
    if (last_virtual_address >= VIRTUAL_MEMORY_END)
    {
        return false;
    }

    for (uint64_t i = 0; i < page_count; i++)
    {
        const uint64_t page_address = virtual_address + (i * PAGE_SIZE);
        if (!is_page_used((page_address - VIRTUAL_MEMORY_START) / PAGE_SIZE))
        {
            return false;
        }

        if (MMU::get_physical_address(page_address) == 0)
        {
            return false;
        }
    }

    for (uint64_t i = 0; i < page_count; i++)
    {
        if (!free_page(virtual_address + (i * PAGE_SIZE)))
        {
            return false;
        }
    }

    return true;
}