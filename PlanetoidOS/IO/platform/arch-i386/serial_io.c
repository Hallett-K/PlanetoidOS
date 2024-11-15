#ifndef _PL_SERIAL_IO_C
#define _PL_SERIAL_IO_C

#include <io.h>

void serial_set_bitrate(uint16_t port, uint16_t divisor)
{
    _io_outb(SERIAL_PORT_LINE_CMD(port), SERIAL_PORT_DLAB_BIT);
    _io_outb(SERIAL_PORT_DATA(port), (uint8_t)((divisor >> 8) & 0xFF));
    _io_outb(SERIAL_PORT_DATA(port), (uint8_t)(divisor & 0xFF));
}

void serial_line_config(uint16_t port, uint8_t cmd)
{
    _io_outb(SERIAL_PORT_LINE_CMD(port), cmd);
}

void serial_fifo_config(uint16_t port, uint8_t cmd)
{
    _io_outb(SERIAL_PORT_FIFO_CMD(port), cmd);
}

void serial_modem_config(uint16_t port, uint8_t cmd)
{
    _io_outb(SERIAL_PORT_MODEM_CMD(port), cmd);
}

uint8_t serial_get_line_status(uint16_t port)
{
    return _io_inb(SERIAL_PORT_STATUS(port));
}

uint8_t serial_get_fifo_empty(uint16_t port)
{
    return _io_inb(SERIAL_PORT_STATUS(port)) & 0x20;
}

void serial_write_char(uint16_t port, char c)
{
    while (serial_get_fifo_empty(port) == 0);
    _io_outb(port, (uint8_t)c);
}

void serial_write_string(uint16_t port, const char* str)
{
    for (const char* p = str; *p != '\0'; p++)
        serial_write_char(port, *p);
}

void serial_write_int(uint16_t port, int32_t n)
{
    char buffer[24];
    char* pos = buffer + sizeof(buffer) - 1;
    *pos = '\0';

    uint8_t negative = 0;
    if (n < 0)
    {
        negative = 1;
        n = -n;
    }

    do {
        *--pos = '0' + n % 10;
        n /= 10;
    } while (n > 0);

    if (negative)
        serial_write_char(port, '-');

    serial_write_string(port, pos);
}

#endif // _PL_SERIAL_IO_C