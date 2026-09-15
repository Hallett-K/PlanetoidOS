#include "gic.hpp"

const uintptr_t GIC_DIST_BASE = 0x08000000;
const uintptr_t GIC_DIST_CTL = GIC_DIST_BASE + 0x000;
const uintptr_t GIC_DIST_TYPER = GIC_DIST_BASE + 0x004;
const uintptr_t GIC_DIST_ISENABLER_BASE = GIC_DIST_BASE + 0x100;
const uintptr_t GIC_DIST_ICENABLER_BASE = GIC_DIST_BASE + 0x180;
const uintptr_t GIC_DIST_PRIORITY_BASE = GIC_DIST_BASE + 0x400;

const uintptr_t GIC_CPU_BASE = 0x08010000;
const uintptr_t GIC_CPU_CTL = GIC_CPU_BASE + 0x000;
const uintptr_t GIC_CPU_PMR = GIC_CPU_BASE + 0x004;
const uintptr_t GIC_CPU_IAR = GIC_CPU_BASE + 0x00C;
const uintptr_t GIC_CPU_EOI = GIC_CPU_BASE + 0x010;

uint32_t read_32(uintptr_t addr)
{
    return *(volatile uint32_t*)addr;
}

void write_32(uintptr_t addr, uint32_t value)
{
    *(volatile uint32_t*)addr = value;
}

void GIC::init()
{
    write_32(GIC_DIST_CTL, 0);
    
    const uint32_t typer = read_32(GIC_DIST_TYPER);
    const uint32_t interrupt_line_count = (typer & 0x1F) + 1;

    for (uint32_t i = 1; i < interrupt_line_count; i++)
    {
        write_32(GIC_DIST_ICENABLER_BASE + (i * 4), 0xFFFFFFFF);
    }

    for (uint32_t i = 0; i < interrupt_line_count * 8; i++)
    {
        write_32(GIC_DIST_PRIORITY_BASE + (i * 4), 0xA0A0A0A0);
    }

    write_32(GIC_CPU_PMR, 0xFF);
    write_32(GIC_CPU_CTL, 1);

    write_32(GIC_DIST_CTL, 1);
}

void GIC::enable_interrupt(uint32_t interrupt_id)
{
    const uint32_t register_index = interrupt_id / 32;
    const uint32_t bit = interrupt_id % 32;

    write_32(GIC_DIST_ISENABLER_BASE + (register_index * 4), 1U << bit);
}

uint32_t GIC::acknowledge_interrupt()
{
    return read_32(GIC_CPU_IAR);
}

void GIC::end_interrupt(uint32_t interrupt_id)
{
    write_32(GIC_CPU_EOI, interrupt_id);
}