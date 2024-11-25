#include <mem.h>
#include <io.h>
#include <vga.h>
void memory_init(struct multiboot_info_t *multiboot_info)
{
    (void)multiboot_info;

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
}