#include <cstdint>

#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "drivers/pi/status_led.h"
#include "drivers/uart.h"

extern "C" void kernel_main()
{
    status_led_init();
    status_led_off();

    Log::info("PlanetoidOS");

    uart_init();
    Log::info("UART Initialised.");

    Interrupts::init();

    while (true)
    {
        asm volatile("wfe");
    }
}