#include <gdt.h>

struct gdt_descriptor gdt;
struct gdt_entry gdt_entries[NUM_GDT_ENTRIES];
struct tss_entry tss_entry_;

#include <io.h>
#include <util.h>
void gdt_init(void)
{
    gdt.size = sizeof(struct gdt_entry) * NUM_GDT_ENTRIES - 1;
    gdt.offset = (uint32_t)&gdt_entries;

    gdt_set_entry(0, 0, 0, 0, 0); // Null segment
    gdt_set_entry(1, 0xFFFFF, 0x00000000, 0x9A, 0xCF); // Kernel code segment
    gdt_set_entry(2, 0xFFFFF, 0x00000000, 0x92, 0xCF); // Kernel data segment
    gdt_set_entry(3, 0xFFFFF, 0x00000000, 0xFA, 0xCF); // User code segment
    gdt_set_entry(4, 0xFFFFF, 0x00000000, 0xF2, 0xCF); // User data segment
    write_tss(5, 0x10, 0x0);

    serial_write_string(SERIAL_PORT_COM1, "GDT initialized - loading into memory\n");

    gdt_load(&gdt);
    tss_load();

    serial_write_string(SERIAL_PORT_COM1, "GDT loaded into memory - entered protected mode\n");
}

void gdt_set_entry(uint8_t index, uint32_t limit, uint32_t base, uint8_t access, uint8_t granularity)
{
    gdt_entries[index].limit_low = (limit & 0xFFFF);
    gdt_entries[index].base_low = (base & 0xFFFF);
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    gdt_entries[index].access = access;
    gdt_entries[index].granularity = (limit >> 16) & 0x0F;
    gdt_entries[index].granularity |= granularity & 0xF0;
    gdt_entries[index].base_high = (base >> 24) & 0xFF;
}

void write_tss(uint8_t index, uint16_t ss0, uint32_t esp0)
{
    uint32_t base = (uint32_t)&tss_entry_;
    uint32_t limit = base + sizeof(struct tss_entry);

    memset(&tss_entry_, 0, sizeof(struct tss_entry));

    tss_entry_.ss0 = ss0;
    tss_entry_.esp0 = esp0;
    
    tss_entry_.cs = 0x08 | 0x3;
    tss_entry_.ss = 0x10 | 0x3;
    tss_entry_.es = 0x10 | 0x3;
    tss_entry_.ds = 0x10 | 0x3;
    tss_entry_.fs = 0x10 | 0x3;
    tss_entry_.gs = 0x10 | 0x3;

    gdt_set_entry(index, limit, base, 0xE9, 0x00);
}