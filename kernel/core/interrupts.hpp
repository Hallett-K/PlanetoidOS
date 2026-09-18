#pragma once

#include <cstdint>

struct exception_context;
namespace Interrupts
{
    using Handler = void(*)(uint32_t, exception_context*);

    void init();
    bool register_handler(uint32_t interrupt_id, Handler handler);
    bool unregister_handler(uint32_t interrupt_id);
    void dispatch_interrupt(uint32_t interrupt_id, exception_context* context);
}