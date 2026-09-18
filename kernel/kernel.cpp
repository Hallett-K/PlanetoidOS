#include "arch/aarch64/exception.hpp"
#include "arch/aarch64/gic.hpp"
#include "arch/aarch64/timer.hpp"
#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "memory/kernel_heap.hpp"
#include "memory/mmu.hpp"
#include "memory/physical_memory.hpp"
#include "memory/virtual_memory.hpp"

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
    VirtualMemory::init();
    KernelHeapAllocator::init();

    void* small_a = kmalloc(64);
    void* large = kmalloc(100000);
    void* small_b = kmalloc(128);

    if (small_a == nullptr || large == nullptr || small_b == nullptr)
    {
        Log::Error("Mixed kmalloc test failed");
        return;
    }

    ((uint8_t*)small_a)[0] = 0x11;
    ((uint8_t*)large)[99999] = 0x22;
    ((uint8_t*)small_b)[0] = 0x33;

    kfree(large);
    kfree(small_a);
    kfree(small_b);

    Log::Info("Pausing for one second!");
    Timer::delay(100);
    Log::Info("Second passed!");

    halt();
}