#include <cstdint>
#include "drivers/pi/status_led.h"

extern "C" void kernel_main()
{

    status_led_off();

    while (true)
    {
        asm volatile("wfe");
    }
}