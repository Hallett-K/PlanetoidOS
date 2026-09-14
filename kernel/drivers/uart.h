#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void uart_putc(char c);
    void uart_puts(const char* s);

#ifdef __cplusplus
}
#endif