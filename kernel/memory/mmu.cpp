#include "mmu.hpp"
#include "core/log.hpp"
#include "memory/physical_memory.hpp"

namespace MMU
{
    alignas(4096) uint64_t level1_table[512];
    alignas(4096) uint64_t level2_table[512];
    alignas(4096) uint64_t level2_ram_table[512];
    alignas(4096) uint64_t level3_kernel_table[512];
}

namespace
{
    const uint64_t TABLE_DESCRIPTOR = 0b11;
    const uint64_t BLOCK_DESCRIPTOR = 0b01;
    const uint64_t PAGE_DESCRIPTOR = 0b11;
    
    const uint64_t ADDRESS_MASK = 0x0000FFFFFFFFF000ULL;
};

namespace TranslationAttributes
{
    const uint64_t ATTR_T0SZ = 25;
    const uint64_t ATTR_TG0_4KB = 0ULL << 14;
    const uint64_t ATTR_SH0_INNER_SHAREABLE = 0b11ULL << 12;
    const uint64_t ATTR_IRGN0 = 0b01ULL << 8;
    const uint64_t ATTR_ORGN0 = 0b01ULL << 10;
}

namespace MemorySections
{
    extern "C"
    {
        extern uint8_t __text_start;
        extern uint8_t __text_end;
        
        extern uint8_t __rodata_start;
        extern uint8_t __rodata_end;

        extern uint8_t __data_start;
        extern uint8_t __data_end;

        extern uint8_t __bss_start;
        extern uint8_t __bss_end;

        extern uint8_t __stack_bottom;
        extern uint8_t __stack_top;
    }
}

uint64_t make_table_descriptor(uint64_t address)
{
    return (address & ADDRESS_MASK) | TABLE_DESCRIPTOR;
}

uint64_t make_normal_block_descriptor(uint64_t address, uint64_t permissions)
{
    return (address & ADDRESS_MASK) 
    | BLOCK_DESCRIPTOR
    | MemoryBlockAttributes::ATTR_NORMAL_MEMORY
    | MemoryBlockAttributes::ATTR_ACCESS
    | MemoryBlockAttributes::ATTR_SHAREABLE
    | permissions;
}

uint64_t make_device_block_descriptor(uint64_t address, uint64_t permissions)
{
    return (address & ADDRESS_MASK) 
    | BLOCK_DESCRIPTOR
    | MemoryBlockAttributes::ATTR_DEVICE_MEMORY
    | MemoryBlockAttributes::ATTR_ACCESS
    | permissions;
}

uint64_t make_normal_page_descriptor(uint64_t address, uint64_t permissions)
{
    return (address & ADDRESS_MASK)
    | PAGE_DESCRIPTOR
    | MemoryBlockAttributes::ATTR_NORMAL_MEMORY
    | MemoryBlockAttributes::ATTR_ACCESS
    | MemoryBlockAttributes::ATTR_SHAREABLE
    | permissions;
}

uint64_t make_device_page_descriptor(uint64_t address, uint64_t permissions)
{
    return (address & ADDRESS_MASK)
    | PAGE_DESCRIPTOR
    | MemoryBlockAttributes::ATTR_DEVICE_MEMORY
    | MemoryBlockAttributes::ATTR_ACCESS
    | permissions;
}

uint64_t get_level1_index(uint64_t virtual_address)
{
    return (virtual_address >> 30) & 0x1FF;
}

uint64_t get_level2_index(uint64_t virtual_address)
{
    return (virtual_address >> 21) & 0x1FF;
}

uint64_t get_level3_index(uint64_t virtual_address)
{
    return (virtual_address >> 12) & 0x1FF;
}

uint64_t* get_or_create_level2_table(uint64_t virtual_address)
{
    const uint64_t level1_index = get_level1_index(virtual_address);
    uint64_t& level1_entry = MMU::level1_table[level1_index];

    if (level1_entry != 0)
    {
        const uint64_t table_address = level1_entry & ADDRESS_MASK;
        return (uint64_t*)table_address;
    }

    MMU::PageTable page_table = MMU::allocate_page_table();

    if (page_table.virtual_address == nullptr)
    {
        return nullptr;
    }

    level1_entry = make_table_descriptor(page_table.physical_address);
    return page_table.virtual_address;
}

uint64_t* get_or_create_level3_table(uint64_t* level2_table, uint64_t virtual_address)
{
    const uint64_t level2_index = get_level2_index(virtual_address);
    uint64_t& level2_entry = level2_table[level2_index];

    if (level2_entry != 0)
    {
        if ((level2_entry & 0b11) != TABLE_DESCRIPTOR)
        {
            return nullptr;
        }

        const uint64_t table_address = level2_entry & ADDRESS_MASK;
        return (uint64_t*)table_address;
    }

    MMU::PageTable page_table = MMU::allocate_page_table();

    if (page_table.virtual_address == nullptr)
    {
        return nullptr;
    }

    level2_entry = make_table_descriptor(page_table.physical_address);
    return page_table.virtual_address;
}

void MMU::init()
{
    level1_table[0] = make_table_descriptor((uint64_t)level2_table); // 0x00000000 - 0x3FFFFFFF covered by level 2 table
    level1_table[1] = make_table_descriptor((uint64_t)level2_ram_table); // 0x40000000 - 0x7FFFFFFF RAM

    level2_table[64] = make_device_block_descriptor(0x08000000, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN | MemoryBlockAttributes::ATTR_UXN); // identity map GIC block
    level2_table[72] = make_device_block_descriptor(0x09000000, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN | MemoryBlockAttributes::ATTR_UXN); // identity map UART block

    level2_ram_table[0] = make_table_descriptor((uint64_t)level3_kernel_table); // 0x40000000 - 0x40200000 Kernel
    for (uint32_t i = 1; i < 512; i++)
    {
        level2_ram_table[i] = make_normal_block_descriptor(0x40000000 + (i * 0x200000), MemoryBlockAttributes::ATTR_EL1_RW);
    }

    for (uint32_t i = 0; i < 512; i++)
    {
        level3_kernel_table[i] = 0;
    }

    for (uint64_t address = (uint64_t)&MemorySections::__text_start; address < (uint64_t)&MemorySections::__text_end; address += 0x1000)
    {
        const uint32_t index = (address - 0x40000000) / 0x1000;
        level3_kernel_table[index] = make_normal_page_descriptor(address, MemoryBlockAttributes::ATTR_EL1_RO);
    }

    for (uint64_t address = (uint64_t)&MemorySections::__rodata_start; address < (uint64_t)&MemorySections::__rodata_end; address += 0x1000)
    {
        const uint32_t index = (address - 0x40000000) / 0x1000;
        level3_kernel_table[index] = make_normal_page_descriptor(address, MemoryBlockAttributes::ATTR_EL1_RO | MemoryBlockAttributes::ATTR_PXN);
    }

    for (uint64_t address = (uint64_t)&MemorySections::__data_start; address < (uint64_t)&MemorySections::__data_end; address += 0x1000)
    {
        const uint32_t index = (address - 0x40000000) / 0x1000;
        level3_kernel_table[index] = make_normal_page_descriptor(address, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN);
    }

    for (uint64_t address = (uint64_t)&MemorySections::__bss_start; address < (uint64_t)&MemorySections::__bss_end; address += 0x1000)
    {
        const uint32_t index = (address - 0x40000000) / 0x1000;
        level3_kernel_table[index] = make_normal_page_descriptor(address, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN);
    }

    for (uint64_t address = (uint64_t)&MemorySections::__stack_bottom; address < (uint64_t)&MemorySections::__stack_top; address += 0x1000)
    {
        const uint32_t index = (address - 0x40000000) / 0x1000;
        level3_kernel_table[index] = make_normal_page_descriptor(address, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN);
    }

    asm volatile("msr ttbr0_el1, %0"
        :
        : "r"(level1_table)
        : "memory");

    const uint64_t normal_memory_id = 0xFF;
    const uint64_t device_memory_id = 0x04;
    const uint64_t mair_lookup = (device_memory_id << 8) | (normal_memory_id << 0);

    asm volatile("msr mair_el1, %0"
        :
        : "r"(mair_lookup)
        : "memory");

    const uint64_t translation_regime = TranslationAttributes::ATTR_T0SZ
        | TranslationAttributes::ATTR_TG0_4KB
        | TranslationAttributes::ATTR_SH0_INNER_SHAREABLE
        | TranslationAttributes::ATTR_IRGN0
        | TranslationAttributes::ATTR_ORGN0;

    asm volatile("msr tcr_el1, %0"
        : 
        : "r"(translation_regime)
        : "memory");

    data_barrier();
    instruction_barrier();

    // Enable MMU
    uint64_t sctlr;
    asm volatile("mrs %0, sctlr_el1"
        : "=r"(sctlr));
    sctlr |= 1ULL;
    asm volatile("msr sctlr_el1, %0"
        : 
        : "r"(sctlr)
        : "memory");

    instruction_barrier();
}

void MMU::data_barrier()
{
    asm volatile("dsb sy" : : : "memory");
}

void MMU::instruction_barrier()
{
    asm volatile("isb" : : : "memory");
}

void MMU::invalidate_tlb()
{
    asm volatile("tlbi vmalle1" : : : "memory");
}

void MMU::invalidate_tlb(uint64_t virtual_address)
{
    const uint64_t page = virtual_address >> 12;
    asm volatile("tlbi vae1, %0" : : "r"(page) : "memory");
}

void MMU::invalidate_tlb_range(uint64_t virtual_address, uint64_t page_count)
{
    for (uint64_t i = 0; i < page_count; i++)
    {
        invalidate_tlb(virtual_address + (i * 0x1000));
    }

    data_barrier();
    instruction_barrier();
}

MMU::PageTable MMU::allocate_page_table()
{
    const uint64_t physical_address = PhysicalMemory::allocate_frame();

    if (physical_address == 0)
    {
        return { 0, nullptr };
    }

    uint64_t* virtual_address = (uint64_t*)physical_address;

    for (uint32_t i = 0; i < 512; i++)
    {
        virtual_address[i] = 0;
    }

    return {
        physical_address,
        virtual_address
    };
}

bool MMU::map_page(uint64_t virtual_address, uint64_t physical_address, MMU::EMemoryType memory_type, uint64_t permissions)
{
    if ((virtual_address & 0xFFF) != 0 || (physical_address & 0xFFF) != 0)
    {
        return false;
    }

    uint64_t* level2_table = get_or_create_level2_table(virtual_address);
    if (level2_table == nullptr)
    {
        return false;
    }

    uint64_t* level3_table = get_or_create_level3_table(level2_table, virtual_address);
    if (level3_table == nullptr)
    {
        return false;
    }

    const uint64_t level3_index = get_level3_index(virtual_address);
    if (level3_table[level3_index] != 0)
    {
        return false;
    }

    uint64_t descriptor;
    if (memory_type == EMemoryType::Normal)
    {
        descriptor = make_normal_page_descriptor(physical_address, permissions);
    }
    else
    {
        descriptor = make_device_page_descriptor(physical_address, permissions);
    }

    level3_table[level3_index] = descriptor;

    data_barrier();
    invalidate_tlb(virtual_address);
    data_barrier();
    instruction_barrier();

    return true;
}

bool MMU::unmap_page(uint64_t virtual_address)
{
    if ((virtual_address & 0xFFF) != 0)
    {
        return false;
    }

    const uint64_t level1_index = get_level1_index(virtual_address);
    const uint64_t level1_entry = level1_table[level1_index];

    if (level1_entry == 0)
    {
        return false;
    }

    if ((level1_entry & 0b11) != TABLE_DESCRIPTOR)
    {
        return false;
    }

    uint64_t* level2_table = (uint64_t*)(level1_entry & ADDRESS_MASK);

    const uint64_t level2_index = get_level2_index(virtual_address);
    const uint64_t level2_entry = level2_table[level2_index];

    if (level2_entry == 0)
    {
        return false;
    }

    if ((level2_entry & 0b11) != TABLE_DESCRIPTOR)
    {
        return false;
    }

    uint64_t* level3_table = (uint64_t*)(level2_entry & ADDRESS_MASK);
    
    const uint64_t level3_index = get_level3_index(virtual_address);
    if (level3_table[level3_index] == 0)
    {
        return false;
    }

    level3_table[level3_index] = 0;

    data_barrier();
    invalidate_tlb(virtual_address);
    data_barrier();
    instruction_barrier();

    return true;
}

uint64_t MMU::get_physical_address(uint64_t virtual_address)
{
    const uint64_t page_offset = virtual_address & 0xFFF;

    const uint64_t level1_index = get_level1_index(virtual_address);
    const uint64_t level1_entry = level1_table[level1_index];

    if (level1_entry == 0)
    {
        return 0;
    }

    if ((level1_entry & 0b11) != TABLE_DESCRIPTOR)
    {
        return 0;
    }

    uint64_t* level2_table = (uint64_t*)(level1_entry & ADDRESS_MASK);

    const uint64_t level2_index = get_level2_index(virtual_address);
    const uint64_t level2_entry = level2_table[level2_index];

    if (level2_entry == 0)
    {
        return 0;
    }

    if ((level2_entry & 0b11) != TABLE_DESCRIPTOR)
    {
        return 0;
    }

    uint64_t* level3_table = (uint64_t*)(level2_entry & ADDRESS_MASK);
    
    const uint64_t level3_index = get_level3_index(virtual_address);
    const uint64_t level3_entry = level3_table[level3_index];
    if (level3_entry == 0)
    {
        return 0;
    }

    if ((level3_entry & 0b11) != PAGE_DESCRIPTOR)
    {
        return 0;
    }

    return (level3_entry & ADDRESS_MASK) + page_offset;
}