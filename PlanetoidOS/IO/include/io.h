#ifndef _PL_IO_H
#define _PL_IO_H

#include <stdint.h>

extern uint8_t _io_inb(uint16_t port);
extern void _io_outb(uint16_t port, uint8_t data);
extern void _io_wait(void);

#endif // _PL_IO_H