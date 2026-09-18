#pragma once 

#include "task.hpp"

namespace Scheduler
{
    void init();

    void add_task(Task::TaskState* task);
    void yield();

    void start();

    Task::TaskState* get_current_task();
};