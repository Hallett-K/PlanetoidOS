#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <vga.h>
#include <io.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <keyboard.h>
#include <mem.h>

struct multiboot_info_t* multiboot_info;

void kernel_early_init(struct multiboot_info_t *multiboot_info_, uint32_t magic)
{
    serial_init();

    if (magic != 0x2BADB002)
    {
        serial_write_string(SERIAL_PORT_COM1, "Invalid multiboot magic number\n");
        return;
    }

    multiboot_info = multiboot_info_;
    memory_init(multiboot_info_);
    
    serial_write_string(SERIAL_PORT_COM1, "Multiboot info received\n");
}

void kernel_main(void) 
{
    serial_write_string(SERIAL_PORT_COM1, "Initializing GDT\n");
    gdt_init();

    serial_write_string(SERIAL_PORT_COM1, "Initializing IDT\n");
    idt_init();

    serial_write_string(SERIAL_PORT_COM1, "Initializing keyboard\n");
    keyboard_init();

    vga_init();
    vga_writestring("PlanetoidOS v0.0\n");

    serial_write_string(SERIAL_PORT_COM1, "PlanetoidOS v0.0\n");

    timer_init();

    sleep(1000);
    vga_writestring("Done Sleeping!\n");

    for (;;);
}
