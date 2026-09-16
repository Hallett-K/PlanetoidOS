#include "arch/aarch64/exception.hpp"
#include "arch/aarch64/gic.hpp"
#include "arch/aarch64/timer.hpp"
#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "memory/mmu.hpp"
#include "memory/physical_memory.hpp"

void enable_interrupts()
{
    asm volatile("msr daifclr, #2"
        : 
        :
        : "memory");

    asm volatile("isb");
}

void halt()
{
    while (true)
    {
        asm volatile("wfe");
    }
}

extern "C" void kernel_main()
{
    Log::Info("PlanetoidOS");
    Log::Info("UART Initialised.");

    Interrupts::init();
    GIC::init();
    Log::Info("GIC initialised!");
    MMU::init();
    Log::Info("MMU Enabled!");

    GIC::enable_interrupt(30); // Timer interrupt
    Timer::init(100); // ticks every 1/100th of a second
    enable_interrupts();

    PhysicalMemory::init();

    const uint64_t physical_frame = PhysicalMemory::allocate_frame();
    if (physical_frame == 0)
    {
        Log::Error("Failed to allocate physical frame!");
        halt();
    }

    const uint64_t virtual_address = 0x80000000;

    bool res = MMU::map_page(virtual_address, physical_frame, MMU::EMemoryType::Normal, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN);
    if (!res)
    {
        Log::Error("Failed to map page!");
        halt();
    }

    Log::Info("Dynamic page mapping succeeded!");

    uint64_t* test_memory = (uint64_t*)virtual_address;
    *test_memory = 0x123456789ABCDEF0ULL;
    if (*test_memory != 0x123456789ABCDEF0ULL)
    {
        Log::Error("Virtual memory read/write test failed!");
        halt();
    }

    Log::Info("Virtual memory read/write test passed!");

    uint64_t virtual_address_b = 0x80000000;
    uint64_t physical_frame_b = physical_frame;

    if (!MMU::unmap_page(virtual_address_b))
    {
        Log::Error("Failed to unmap page!");
        halt();
    }

    Log::Info("Page unmapped successfully!");

    if (MMU::unmap_page(virtual_address_b))
    {
        Log::Error("Unmapping an already unmapped page succeeded!");
        halt();
    }

    const uint64_t physical_frame_c = PhysicalMemory::allocate_frame();
    if (physical_frame_c == 0)
    {
        Log::Error("Failed to allocate physical frame for remapping!");
        halt();
    }

    if (physical_frame_c == physical_frame_b)
    {
        Log::Error("New physical frame identical to previous!");
        halt();
    }

    if (!MMU::map_page(virtual_address_b, physical_frame_c, MMU::EMemoryType::Normal, MemoryBlockAttributes::ATTR_EL1_RW | MemoryBlockAttributes::ATTR_PXN))
    {
        Log::Error("Failed to remap page!");
        halt();
    }

    uint64_t* test_memory_b = (uint64_t*)virtual_address_b;
    *test_memory_b = 0xABCDEF0123456789ULL;
    if (*test_memory_b != 0xABCDEF0123456789ULL)
    {
        Log::Error("Remapped virtual memory read/write test failed!");
        halt();
    }

    Log::Info("Page unmap and remap test passed!");

    Log::Info("Pausing for one second!");
    Timer::delay(100);
    Log::Info("Second passed!");

    halt();
}