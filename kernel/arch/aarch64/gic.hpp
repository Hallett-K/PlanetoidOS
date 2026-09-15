#pragma once 

#include <cstdint>

namespace GIC
{
    void init();
    void enable_interrupt(uint32_t interrupt_id);
    uint32_t acknowledge_interrupt();
    void end_interrupt(uint32_t interrupt_id);
}