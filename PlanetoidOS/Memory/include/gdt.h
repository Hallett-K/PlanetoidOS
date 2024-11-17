#ifndef _PL_GDT_H
#define _PL_GDT_H

#include <stdint.h>

struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;

    uint8_t base_middle;
    uint8_t access; // Type, privellege, present
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_descriptor
{
    // Size and offset
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

#define NUM_GDT_ENTRIES 3
// Null, Kernel Code, Kernel Data

extern struct gdt_descriptor gdt;
extern struct gdt_entry gdt_entries[NUM_GDT_ENTRIES];

void gdt_init(void);
void gdt_set_entry(uint8_t index, uint32_t limit, uint32_t base, uint8_t access, uint8_t granularity);
extern void gdt_load(struct gdt_descriptor* gdt_ptr);

#endif