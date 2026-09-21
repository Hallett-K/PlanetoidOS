#include <cstdint>

extern "C" void kernel_main()
{

    const uintptr_t GPIO_BASE = 0x107C000000UL;

    const uintptr_t GIO_LED_BASE_OFFSET = 0x1517C00;
    const uintptr_t GIO_LED_DATA_OFFSET = 0x04;
    
    const uintptr_t GIO_LED_BASE = GPIO_BASE + GIO_LED_BASE_OFFSET;
    const uintptr_t GIO_LED_DATA = GIO_LED_BASE + GIO_LED_DATA_OFFSET;

    const uint32_t LED_BIT = 1u << 9;

    volatile uint32_t* base = (volatile uint32_t*)GIO_LED_BASE;
    volatile uint32_t* data = (volatile uint32_t*)GIO_LED_DATA;

    // On
    // *data |= LED_BIT;

    // Off
    *data &= ~LED_BIT;

    while (true)
    {
        asm volatile("wfe");
    }
}