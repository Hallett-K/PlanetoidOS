#include "mmu.hpp"

namespace MMU
{
    alignas(4096) uint64_t level1_table[512];
    alignas(4096) uint64_t level2_table[512];
}

namespace
{
    const uint64_t TABLE_DESCRIPTOR = 0b11;
    const uint64_t BLOCK_DESCRIPTOR = 0b01;
    
    const uint64_t ADDRESS_MASK = 0x0000FFFFFFFFF000ULL;
};

namespace MemoryBlockAttributes
{
    const uint64_t ATTR_NORMAL_MEMORY = 0ULL << 2;
    const uint64_t ATTR_DEVICE_MEMORY = 1ULL << 2;
    const uint64_t ATTR_ACCESS = 1ULL << 10; // This block has been accessed
    const uint64_t ATTR_SHAREABLE = 1ULL << 8;

    const uint64_t ATTR_EL1_RW = 0ULL << 6; // Allow read/write for EL1 only

    const uint64_t ATTR_PXN = 1ULL << 53; // Privelleged Execute Never
    const uint64_t ATTR_UXN = 1ULL << 54; // Unprivelleged Execute Never
};

namespace TranslationAttributes
{
    const uint64_t ATTR_T0SZ = 25;
    const uint64_t ATTR_TG0_4KB = 0ULL << 14;
    const uint64_t ATTR_SH0_INNER_SHAREABLE = 0b11ULL << 12;
    const uint64_t ATTR_IRGN0 = 0b01ULL << 8;
    const uint64_t ATTR_ORGN0 = 0b01ULL << 10;
}

uint64_t make_table_descriptor(uint64_t address)
{
    return (address & ADDRESS_MASK) | TABLE_DESCRIPTOR;
}

uint64_t make_normal_block_descriptor(uint64_t address)
{
    return (address & ADDRESS_MASK) 
    | BLOCK_DESCRIPTOR
    | MemoryBlockAttributes::ATTR_NORMAL_MEMORY
    | MemoryBlockAttributes::ATTR_ACCESS
    | MemoryBlockAttributes::ATTR_SHAREABLE
    | MemoryBlockAttributes::ATTR_EL1_RW;
}

uint64_t make_device_block_descriptor(uint64_t address)
{
    return (address & ADDRESS_MASK) 
    | BLOCK_DESCRIPTOR
    | MemoryBlockAttributes::ATTR_DEVICE_MEMORY
    | MemoryBlockAttributes::ATTR_ACCESS
    | MemoryBlockAttributes::ATTR_EL1_RW
    | MemoryBlockAttributes::ATTR_PXN
    | MemoryBlockAttributes::ATTR_UXN;
}

void MMU::init()
{
    level1_table[0] = make_table_descriptor((uint64_t)level2_table); // 0x00000000 - 0x3FFFFFFF covered by level 2 table
    level1_table[1] = make_normal_block_descriptor(0x40000000); // 0x40000000 - 0x7FFFFFFF identity mapped

    level2_table[64] = make_device_block_descriptor(0x08000000); // identity map GIC block
    level2_table[72] = make_device_block_descriptor(0x09000000); // identity map UART block

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

    asm volatile("dsb sy"
        :
        :
        : "memory");

    asm volatile("isb"
        :
        :
        : "memory");

    // Enable MMU

    uint64_t sctlr;
    asm volatile("mrs %0, sctlr_el1"
        : "=r"(sctlr));
    sctlr |= 1ULL;
    asm volatile("msr sctlr_el1, %0"
        : 
        : "r"(sctlr)
        : "memory");

    asm volatile("isb"
        :
        :
        : "memory");
}