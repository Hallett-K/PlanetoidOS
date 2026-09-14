#include "core/log.hpp"

extern "C" void kernel_main()
{
    Log::Info("PlanetoidOS");
    Log::Info("UART Initialised.");
    Log::Error("This is a test error");

    while (true)
    {
        asm volatile("wfe");
    }
}