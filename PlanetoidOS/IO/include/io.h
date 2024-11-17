#ifndef _PL_IO_H
#define _PL_IO_H

#include <stdint.h>

extern uint8_t _io_inb(uint16_t port);
extern void _io_outb(uint16_t port, uint8_t data);
extern void _io_wait(void);

// Serial

#define SERIAL_PORT_COM1 0x3F8

#define SERIAL_PORT_DATA(base) (base)
#define SERIAL_PORT_FIFO_CMD(base) (base + 2)
#define SERIAL_PORT_LINE_CMD(base) (base + 3)
#define SERIAL_PORT_MODEM_CMD(base) (base + 4)
#define SERIAL_PORT_STATUS(base) (base + 5)

#define SERIAL_PORT_DLAB_BIT 0x80

void serial_set_bitrate(uint16_t port, uint16_t divisor);
void serial_line_config(uint16_t port, uint8_t cmd);
void serial_fifo_config(uint16_t port, uint8_t cmd);
void serial_modem_config(uint16_t port, uint8_t cmd);
uint8_t serial_get_line_status(uint16_t port);
uint8_t serial_get_fifo_empty(uint16_t port);
void serial_write_char(uint16_t port, char c);
void serial_write_string(uint16_t port, const char* str);
void serial_write_int(uint16_t port, int32_t n); // TEMP
void serial_write_ulong(uint16_t port, uint64_t n); // TEMP

void serial_init(void);

#endif // _PL_IO_H