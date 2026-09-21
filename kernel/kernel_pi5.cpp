#include <cstdint>
#include "core/log.hpp"
#include "drivers/pi/status_led.h"
#include "drivers/uart.h"

extern "C" void kernel_main()
{
    Log::info("PlanetoidOS");

    uart_init();
    Log::info("UART Initialised.");

    status_led_off();

    while (true)
    {
        asm volatile("wfe");
    }
}