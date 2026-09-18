#include "interrupts.hpp"

#include "arch/aarch64/exception.hpp"

namespace
{
    const uint32_t MAX_INTERRUPTS = 1024;
    Interrupts::Handler handlers[MAX_INTERRUPTS] = {};
}

void Interrupts::init()
{
    for (uint32_t i = 0; i < MAX_INTERRUPTS; i++)
    {
        handlers[i] = nullptr;
    }
}

bool Interrupts::register_handler(uint32_t interrupt_id, Handler handler)
{
    if (interrupt_id >= MAX_INTERRUPTS)
    {
        return false;
    }

    if (handler == nullptr)
    {
        return false;
    }

    if (handlers[interrupt_id] != nullptr)
    {
        return false;
    }

    handlers[interrupt_id] = handler;
    return true;
}

bool Interrupts::unregister_handler(uint32_t interrupt_id)
{
    if (interrupt_id >= MAX_INTERRUPTS)
    {
        return false;
    }

    if (handlers[interrupt_id] == nullptr)
    {
        return false;
    }

    handlers[interrupt_id] = nullptr;
    return true;
}

void Interrupts::dispatch_interrupt(uint32_t interrupt_id, exception_context* context)
{
    if (interrupt_id >= MAX_INTERRUPTS)
    {
        return;
    }

    Handler handler = handlers[interrupt_id];
    if (handler != nullptr)
    {
        handler(interrupt_id, context);
    }
}