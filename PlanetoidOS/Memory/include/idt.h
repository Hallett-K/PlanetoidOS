#ifndef _PL_IDT_H
#define _PL_IDT_H

#include <stdint.h>
#include <util.h>

#define IDT_ENTRIES 256

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_EOI 0x20

struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t attributes;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_descriptor
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern struct idt_descriptor idt;
__attribute__((aligned(0x10))) extern struct idt_entry idt_entries[IDT_ENTRIES];
extern uint32_t irq_numbers[];

void idt_init(void);
void idt_set_entry(uint8_t index, uint32_t offset, uint16_t selector, uint8_t attributes);

extern void idt_load(struct idt_descriptor* idt);

void irq_set_handler(uint8_t irq, void (*handler)(struct interrupt_frame* frame));
void irq_clear_handler(uint8_t irq);

void isr_handler(struct interrupt_frame* frame);
void irq_handler(struct interrupt_frame* frame);

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void isr128(void);
extern void isr177(void);

#endif // _PL_IDT_H