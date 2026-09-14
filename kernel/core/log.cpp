#include "log.hpp"

extern "C"
{
    void uart_putc(char c);
    void uart_puts(const char* s);
}

namespace Log
{
    void Info(const char* message)
    {
        uart_puts("[INFO] ");
        uart_puts(message);
        uart_puts("\r\n");
    }

    void Error(const char* message)
    {
        uart_puts("[ERROR] ");
        uart_puts(message);
        uart_puts("\r\n");
    }
}