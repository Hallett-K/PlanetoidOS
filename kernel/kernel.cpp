#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "arch/aarch64/exception.hpp"
#include "arch/aarch64/gic.hpp"
#include "arch/aarch64/mmu.hpp"
#include "arch/aarch64/timer.hpp"

void enable_interrupts()
{
    asm volatile("msr daifclr, #2"
        : 
        :
        : "memory");

    asm volatile("isb");
}

extern "C" void kernel_main()
{
    Log::Info("PlanetoidOS");
    Log::Info("UART Initialised.");
    Log::Error("This is a test error");

    Interrupts::init();
    GIC::init();

    MMU::init();
    Log::Info("MMU Enabled!");

    GIC::enable_interrupt(30); // Timer interrupt
    Timer::init(100); // ticks every 1/100th of a second
    enable_interrupts();

    Log::Info("Pausing for one second!");
    Timer::delay(100);
    Log::Info("Second passed!");

    while (true)
    {
        asm volatile("wfe");
    }
}