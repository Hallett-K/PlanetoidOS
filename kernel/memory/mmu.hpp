#pragma once 

#include <cstdint>

namespace MMU
{
    alignas(4096) extern uint64_t level1_table[512];
    alignas(4096) extern uint64_t level2_table[512];
    alignas(4096) extern uint64_t level2_ram_table[512];
    alignas(4096) extern uint64_t level3_kernel_table[512];

    void init();
    void enable();

    void data_barrier();
    void instruction_barrier();
    void invalidate_tlb();
    void invalidate_tlb(uint64_t virtual_address);
    void invalidate_tlb_range(uint64_t virtual_address, uint64_t page_count);

    struct PageTable
    {
        uint64_t physical_address;
        uint64_t* virtual_address;
    };

    enum class EMemoryType : uint8_t
    {
        Normal = 0,
        Device
    };

    PageTable allocate_page_table();
    bool map_page(uint64_t virtual_address, uint64_t physical_address, EMemoryType memry_type, uint64_t permissions);
    bool unmap_page(uint64_t virtual_address);
};

namespace MemoryBlockAttributes
{
    const uint64_t ATTR_NORMAL_MEMORY = 0ULL << 2;
    const uint64_t ATTR_DEVICE_MEMORY = 1ULL << 2;
    const uint64_t ATTR_ACCESS = 1ULL << 10; // This block has been accessed
    const uint64_t ATTR_SHAREABLE = 0b11ULL << 8;

    const uint64_t ATTR_EL1_RW = 0b00ULL << 6; // Allow read/write for EL1 only
    const uint64_t ATTR_EL1_RO = 0b10ULL << 6; // Read Only from EL1

    const uint64_t ATTR_PXN = 1ULL << 53; // Privelleged (EL1) Execute Never
    const uint64_t ATTR_UXN = 1ULL << 54; // Unprivelleged (EL0) Execute Never
};