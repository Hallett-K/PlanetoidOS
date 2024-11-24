#include <timer.h>

#include <idt.h>
#include <io.h>
#include <util.h>
#include <vga.h>

uint64_t ticks;
uint64_t ticks_since_boot;
const uint32_t frequency = 100;

#define PIT_COMMAND 0x43
#define PIT_DATA 0x40

void timer_init(void)
{
    ticks = 0;
    ticks_since_boot = 0;

    irq_set_handler(0, on_irq0);

    uint32_t divisor = 1193180 / frequency;

    _io_outb(PIT_COMMAND, 0x36);
    _io_outb(PIT_DATA, divisor & 0xFF);
    _io_outb(PIT_DATA, (divisor >> 8) & 0xFF);

}

void on_irq0(struct interrupt_frame* frame)
{
    (void)frame; // Unused but hides warning
    ticks++;
    ticks_since_boot++;
}

void sleep(uint32_t ms)
{
    ticks = 0;

    while (ticks < ms)
    {
        __asm__("hlt");
    }

    ticks -= ms;
}

uint64_t get_ticks_since_boot(void)
{
    return ticks_since_boot;
}