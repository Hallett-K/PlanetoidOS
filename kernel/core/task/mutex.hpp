#pragma once

#include "wait_queue.hpp"

namespace Mutex
{
    struct MutexState
    {
        bool locked;
        WaitQueue::TaskWaitQueue wait_queue;
    };

    void init(MutexState* mutex);
    void lock(MutexState* mutex);
    void unlock(MutexState* mutex);
}