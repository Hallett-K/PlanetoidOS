#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#if OS_PLATFORM_PI5
    void uart_init();
#endif

    void uart_putc(char c);
    void uart_puts(const char* s);

#ifdef __cplusplus
}
#endif