#pragma once

#include <cstdint>

namespace Interrupts
{
    using Handler = void(*)(uint32_t);

    void init();
    bool register_handler(uint32_t interrupt_id, Handler handler);
    bool unregister_handler(uint32_t interrupt_id);
    void dispatch_interrupt(uint32_t interrupt_id);
}