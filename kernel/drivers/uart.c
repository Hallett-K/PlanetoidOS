#include <stdint.h>
#include "uart.h"

const uintptr_t UART_BASE = 0x09000000;
const uintptr_t UART_DATA = UART_BASE;
const uintptr_t UART_FLAGS = UART_BASE + 0x018;

const uint32_t UART_FLAG_TXFF = 1u << 5;
const uint32_t UART_FLAG_BUSY = 1u << 3;

volatile uint32_t* data = (uint32_t*)UART_DATA;
volatile uint32_t* flags = (uint32_t*)UART_FLAGS;

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