#include <stdint.h>
#include "drivers/uart.h"

static const uintptr_t GPIO_BASE = 0x107C000000UL;

static const uintptr_t GIO_LED_BASE_OFFSET = 0x1517C00;
static const uintptr_t GIO_LED_DATA_OFFSET = 0x04;

static const uintptr_t GIO_LED_BASE = GPIO_BASE + GIO_LED_BASE_OFFSET;
static const uintptr_t GIO_LED_DATA = GIO_LED_BASE + GIO_LED_DATA_OFFSET;

static const uint32_t LED_BIT = 1u << 9;

static volatile uint32_t* base = (volatile uint32_t*)GIO_LED_BASE;
static volatile uint32_t* data = (volatile uint32_t*)GIO_LED_DATA;

void status_led_on()
{
    *data |= LED_BIT;
}

void status_led_off()
{
    *data &= ~LED_BIT;
}