#include <gdt.h>

struct gdt_descriptor gdt;
struct gdt_entry gdt_entries[NUM_GDT_ENTRIES];

#include <io.h>

void gdt_init(void)
{
    gdt.size = sizeof(struct gdt_entry) * NUM_GDT_ENTRIES - 1;
    gdt.offset = (uint32_t)&gdt_entries;

    gdt_set_entry(0, 0, 0, 0, 0); // Null segment
    gdt_set_entry(1, 0xFFFFF, 0x00000000, 0x9A, 0xCF); // Kernel code segment
    gdt_set_entry(2, 0xFFFFF, 0x00000000, 0x92, 0xCF); // Kernel data segment

    serial_write_string(SERIAL_PORT_COM1, "GDT initialized - loading into memory\n");

    gdt_load(&gdt);

    serial_write_string(SERIAL_PORT_COM1, "GDT loaded into memory\n");
}

void gdt_set_entry(uint8_t index, uint32_t limit, uint32_t base, uint8_t access, uint8_t granularity)
{
    gdt_entries[index].limit_low = (limit & 0xFFFF);
    gdt_entries[index].base_low = (base & 0xFFFF);
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    gdt_entries[index].access = access;
    gdt_entries[index].granularity = granularity;
    gdt_entries[index].base_high = (base >> 24) & 0xFF;
}