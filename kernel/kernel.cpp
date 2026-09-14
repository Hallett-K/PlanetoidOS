#include "core/log.hpp"
#include "arch/aarch64/exception.hpp"

extern "C" void exception_handler(exception_context* context)
{
    (void)context;

    Log::Error("Sync excpetion received!");
    
    while (true)
    {
        asm volatile("wfe");
    }
}

extern "C" void kernel_main()
{
    Log::Info("PlanetoidOS");
    Log::Info("UART Initialised.");
    Log::Error("This is a test error");

    asm volatile(".inst 0x00000000");

    Log::Error("This error should not be hit!");

    while (true)
    {
        asm volatile("wfe");
    }
}