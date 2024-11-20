#include <idt.h>

#include <io.h>

__attribute__((aligned(0x10))) struct idt_entry idt_entries[IDT_ENTRIES];
struct idt_descriptor idt;
uint32_t irq_numbers[] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

void idt_init(void)
{
    idt.base = (uint32_t)&idt_entries;
    idt.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;

    memset(&idt_entries, 0, sizeof(struct idt_entry) * IDT_ENTRIES);

    _io_outb(PIC_MASTER_COMMAND, 0x11);
    _io_outb(PIC_SLAVE_COMMAND, 0x11);

    _io_outb(PIC_MASTER_DATA, 0x20);
    _io_outb(PIC_SLAVE_DATA, 0x28);

    _io_outb(PIC_MASTER_DATA, 0x04);
    _io_outb(PIC_SLAVE_DATA, 0x02);

    _io_outb(PIC_MASTER_DATA, 0x01);
    _io_outb(PIC_SLAVE_DATA, 0x01);

    _io_outb(PIC_MASTER_DATA, 0x00);
    _io_outb(PIC_SLAVE_DATA, 0x00);

    idt_set_entry(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_entry(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_entry(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set_entry(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_entry(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set_entry(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set_entry(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_set_entry(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_set_entry(8, (uint32_t)isr8, 0x08, 0x8E);
    idt_set_entry(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_set_entry(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_entry(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_entry(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_entry(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_entry(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_entry(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_entry(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_entry(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_entry(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_entry(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_entry(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_entry(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_entry(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_entry(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_entry(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_entry(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_entry(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_entry(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_entry(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_entry(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_entry(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_entry(31, (uint32_t)isr31, 0x08, 0x8E);

    idt_set_entry(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_entry(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_entry(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_entry(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_entry(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_entry(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_entry(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_entry(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_entry(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_entry(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_entry(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_entry(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_entry(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_entry(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_entry(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_entry(47, (uint32_t)irq15, 0x08, 0x8E);

    // Syscalls
    idt_set_entry(128, (uint32_t)isr128, 0x08, 0x8E);
    idt_set_entry(177, (uint32_t)isr177, 0x08, 0x8E);

    idt_load(&idt);

    serial_write_string(SERIAL_PORT_COM1, "IDT loaded into memory\n");
}

void idt_set_entry(uint8_t index, uint32_t offset, uint16_t selector, uint8_t attributes)
{
    idt_entries[index].offset_low = (offset & 0xFFFF);
    idt_entries[index].offset_high = (offset >> 16) & 0xFFFF;
    idt_entries[index].selector = selector;
    idt_entries[index].zero = 0;
    idt_entries[index].attributes = attributes | 0x60;
}

void irq_set_handler(uint8_t irq, void (*handler)(struct interrupt_frame* frame))
{
    irq_numbers[irq] = (uint32_t)handler;
}

void irq_clear_handler(uint8_t irq)
{
    irq_numbers[irq] = 0;
}

char* isr_names[] = {
    "Division by zero",
    "Debug",
    "Non maskable interrupt",
    "Breakpoint",
    "Into detected overflow",
    "Out of bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment fault",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(struct interrupt_frame* frame)
{
    if (frame->interrupt_number < 32)
    {
        serial_write_string(SERIAL_PORT_COM1, "INTERRUPT: ");
        serial_write_string(SERIAL_PORT_COM1, isr_names[frame->interrupt_number]);
        serial_write_string(SERIAL_PORT_COM1, "\nSystem halted.\n");
        __asm__ volatile("cli; hlt");
    }
}

void irq_handler(struct interrupt_frame* frame)
{
    void (*handler)(struct interrupt_frame*);
    handler = (void (*)(struct interrupt_frame*))irq_numbers[frame->interrupt_number - 32];
    if (handler)
    {
        handler(frame);
    }
    
    if (frame->interrupt_number >= 40)
    {
        _io_outb(PIC_SLAVE_COMMAND, PIC_EOI);
    }

    _io_outb(PIC_MASTER_COMMAND, PIC_EOI);
}