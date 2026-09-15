#include "core/log.hpp"
#include "arch/aarch64/exception.hpp"

extern "C" void kernel_main()
{
    Log::Info("PlanetoidOS");
    Log::Info("UART Initialised.");
    Log::Error("This is a test error");

    //asm volatile(".inst 0x00000000");
    volatile uint64_t* invalid_addr = (uint64_t*)0x50000000ULL;
    *invalid_addr = 0;

    Log::Error("This error should not be hit!");

    while (true)
    {
        asm volatile("wfe");
    }
}