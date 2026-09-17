#include "arch/aarch64/exception.hpp"
#include "arch/aarch64/gic.hpp"
#include "arch/aarch64/timer.hpp"
#include "core/interrupts.hpp"
#include "core/log.hpp"
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

    Log::Info("Pausing for one second!");
    Timer::delay(100);
    Log::Info("Second passed!");

    halt();
}