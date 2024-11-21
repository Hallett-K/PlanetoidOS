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

struct tss_entry
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

struct gdt_descriptor
{
    // Size and offset
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

#define NUM_GDT_ENTRIES 6
// Null, Kernel Code, Kernel Data, User Code, User Data, Task State Segment

extern struct gdt_descriptor gdt;
extern struct gdt_entry gdt_entries[NUM_GDT_ENTRIES];
extern struct tss_entry tss_entry_;

void gdt_init(void);
void gdt_set_entry(uint8_t index, uint32_t limit, uint32_t base, uint8_t access, uint8_t granularity);
void write_tss(uint8_t index, uint16_t ss0, uint32_t esp0);

extern void gdt_load(struct gdt_descriptor* gdt_ptr);
extern void tss_load(void);
#endif