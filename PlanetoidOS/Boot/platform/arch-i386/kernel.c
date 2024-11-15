#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <vga.h>
#include <io.h>

void init_serial(void)
{
    serial_set_bitrate(SERIAL_PORT_COM1, 1);
    serial_line_config(SERIAL_PORT_COM1, 0x03);
    serial_fifo_config(SERIAL_PORT_COM1, 0xC7);
    serial_modem_config(SERIAL_PORT_COM1, 0x03);
}

void kernel_main(void) 
{
    init_serial();

    vga_init();
    vga_writestring("PlanetoidOS v0.0");

    serial_write_string(SERIAL_PORT_COM1, "PlanetoidOS v0.0\n");
}
