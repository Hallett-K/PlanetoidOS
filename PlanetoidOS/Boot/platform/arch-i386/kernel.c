#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <vga.h>
#include <io.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>

struct multiboot_info_t
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint32_t framebuffer_addr_low;
    uint32_t framebuffer_addr_high;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t color_info[4];
} __attribute__((packed));

struct multiboot_memory_map_entry_t
{
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

    uint32_t type;
} __attribute__((packed));

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
    
    serial_write_string(SERIAL_PORT_COM1, "Multiboot info received\n");
}

void kernel_main(void) 
{
    serial_write_string(SERIAL_PORT_COM1, "Initializing GDT\n");
    gdt_init();

    serial_write_string(SERIAL_PORT_COM1, "Initializing IDT\n");
    idt_init();

    vga_init();
    vga_writestring("PlanetoidOS v0.0\n");

    serial_write_string(SERIAL_PORT_COM1, "PlanetoidOS v0.0\n");

    //serial_write_int(SERIAL_PORT_COM1, 1 / 0); // Division by zero - Our ISR will catch this!

    for (uint32_t i = 0; i < multiboot_info->mmap_length; i += sizeof(struct multiboot_memory_map_entry_t))
    {
        serial_write_string(SERIAL_PORT_COM1, "Slot ");
        serial_write_int(SERIAL_PORT_COM1, i);
        serial_write_string(SERIAL_PORT_COM1, ": ");

        vga_writestring("Slot ");
        vga_writeint(i);
        vga_writestring(": ");

        struct multiboot_memory_map_entry_t* entry = (struct multiboot_memory_map_entry_t*)(multiboot_info->mmap_addr + i);
        serial_write_string(SERIAL_PORT_COM1, "Memory type: ");
        vga_writestring("Memory type: ");
        switch (entry->type)
        {
            case MULTIBOOT_MEMORY_AVAILABLE:
                serial_write_string(SERIAL_PORT_COM1, "Available - ");
                vga_writestring("Available - ");
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                serial_write_string(SERIAL_PORT_COM1, "Reserved - ");
                vga_writestring("Reserved - ");
                break;
            case MULTIBOOT_MEMORY_ACPI:
                serial_write_string(SERIAL_PORT_COM1, "ACPI - ");
                vga_writestring("ACPI - ");
                break;
            case MULTIBOOT_MEMORY_NVS:
                serial_write_string(SERIAL_PORT_COM1, "NVS - ");
                vga_writestring("NVS - ");
                break;
            case MULTIBOOT_MEMORY_BADRAM:
                serial_write_string(SERIAL_PORT_COM1, "Bad RAM - ");
                vga_writestring("Bad RAM - ");
                break;
        }

        serial_write_string(SERIAL_PORT_COM1, "Start: "); 
        vga_writestring("Start: ");
        uint64_t start = entry->addr_low | ((uint64_t)entry->addr_high << 32);
        serial_write_ulong(SERIAL_PORT_COM1, start);
        vga_writeint(start);
        serial_write_string(SERIAL_PORT_COM1, " - Length: ");
        vga_writestring(" - Length: ");
        uint64_t length = entry->len_low | ((uint64_t)entry->len_high << 32);
        serial_write_ulong(SERIAL_PORT_COM1, length);
        vga_writeint(length);
        serial_write_string(SERIAL_PORT_COM1, "\n");
        vga_writestring("\n");
    }

    init_timer();

    sleep(1000);
    vga_writestring("Done Sleeping!\n");

    for (;;);
}
