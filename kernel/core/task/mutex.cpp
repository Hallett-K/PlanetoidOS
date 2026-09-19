#include "mutex.hpp"

#include "core/interrupts.hpp"
#include "core/log.hpp"

void Mutex::init(MutexState* mutex)
{
    if (mutex == nullptr)
    {
        return;
    }

    mutex->locked = false;
    WaitQueue::init(&mutex->wait_queue);
}

void Mutex::lock(MutexState* mutex)
{
    if (mutex == nullptr)
    {
        return;
    }

    while (true)
    {
        const uint64_t irq_state = Interrupts::irq_save();

        if (!mutex->locked)
        {
            mutex->locked = true;
            Interrupts::irq_restore(irq_state);
            return;
        }

        WaitQueue::block_current(&mutex->wait_queue);
    }
}

void Mutex::unlock(MutexState* mutex)
{
    if (mutex == nullptr)
    {
        return;
    }

    const uint64_t irq_state = Interrupts::irq_save();

    mutex->locked = false;

    if (mutex->wait_queue.head != nullptr)
    {
        WaitQueue::wake_one(&mutex->wait_queue);
    }

    Interrupts::irq_restore(irq_state);
}