#include <cstdint>

#include "arch/aarch64/gic.hpp"
#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "drivers/pi/status_led.h"
#include "drivers/uart.h"
#include "memory/mmu.hpp"

extern "C" void kernel_main()
{
    status_led_init();
    status_led_on();
    uart_init();
    Interrupts::init();
    GIC::init();
    MMU::init();
    MMU::enable();
    status_led_off();

    while (true)
    {
        asm volatile("wfe");
    }
}