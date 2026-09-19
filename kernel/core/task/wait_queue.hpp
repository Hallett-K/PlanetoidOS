#pragma once

#include "task.hpp"

namespace WaitQueue
{
    struct TaskWaitQueue
    {
        Task::TaskState* head;
        Task::TaskState* tail;
    };

    void init(TaskWaitQueue* queue);
    void block_current(TaskWaitQueue* queue);
    void wake_one(TaskWaitQueue* queue);
    void wake_all(TaskWaitQueue* queue);
}