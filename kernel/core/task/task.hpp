#pragma once

#include <cstdint>

namespace Task
{
    enum class EState : uint8_t
    {
        Ready = 0,
        Running,
        Sleeping,
        Blocked,
        Terminated
    };

    struct CPUContext
    {
        uint64_t x[31];
        uint64_t sp;
        uint64_t pc;
        uint64_t pstate;
    };

    struct TaskState
    {
        CPUContext cpu_context;

        uint64_t stack_base;
        uint64_t stack_size;
        
        EState state;

        uint64_t wake_tick;

        TaskState* next_task;
        TaskState* wait_next;
        TaskState* cleanup_next;
    };

    TaskState* create_task(void(*entry_point)(), uint64_t stack_size);
    TaskState* create_blank_task();
};

extern "C" void task_switch(Task::TaskState* current, Task::TaskState* next);