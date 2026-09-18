#pragma once 

#include "arch/aarch64/exception.hpp"
#include "task.hpp"

namespace Scheduler
{
    void init();

    void add_task(Task::TaskState* task);
    void yield();

    void start();
    [[noreturn]] void terminate_current();

    Task::TaskState* get_current_task();

    void preempt(exception_context* context);
};

extern "C" [[noreturn]] void task_terminated();