#include <stdint.h>
#include "uart.h"

#if OS_PLATFORM_PI5
static const uintptr_t UART_BASE = 0x107D000000UL;
#elif OS_PLATFORM_VIRT
static const uintptr_t UART_BASE = 0x09000000;
#endif

static const uintptr_t UART_DATA = UART_BASE;
static const uintptr_t UART_FLAGS = UART_BASE + 0x18;

static const uint32_t UART_FLAG_TXFF = 1u << 5;

static volatile uint32_t* data = (volatile uint32_t*)UART_DATA;
static volatile uint32_t* flags = (volatile uint32_t*)UART_FLAGS;

void uart_init()
{
#if OS_PLATFORM_PI5
    const uintptr_t UART_IBRD = UART_BASE + 0x24;
    const uintptr_t UART_FBRD = UART_BASE + 0x28;
    const uintptr_t UART_LCRH = UART_BASE + 0x2C;
    const uintptr_t UART_CR = UART_BASE + 0x30;
    const uintptr_t UART_ICR = UART_BASE + 0x44;

    *(volatile uint32_t*)UART_CR = 0x00000000;

    *(volatile uint32_t*)UART_ICR = 0x7FF;

    *(volatile uint32_t*)UART_IBRD = 26;
    *(volatile uint32_t*)UART_FBRD = 3;

    *(volatile uint32_t*)UART_LCRH = (1 << 4) | ( 1 << 5) | (1 << 6);
    *(volatile uint32_t*)UART_CR = (1 << 0) | (1 << 8);
#endif
}

void uart_putc(char c)
{
    while (*flags & UART_FLAG_TXFF) { 0; }
    *data = (uint32_t)c;
}

void uart_puts(const char* s)
{
    while (*s)
    {
        uart_putc(*s++);
    }
}