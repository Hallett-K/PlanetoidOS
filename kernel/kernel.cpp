#include "drivers/uart.h"

extern "C" void kernel_main()
{
    uart_puts("Hello from PlanetoidOS!\r\n");
    
    while (true)
    {
        asm volatile("wfe");
    }
}