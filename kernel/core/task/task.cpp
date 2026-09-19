#include "task.hpp"

#include "memory/kernel_heap.hpp"
#include "scheduler.hpp"


namespace
{
    uint64_t align_stack_size(uint64_t stack_size)
    {
        return (stack_size + 15) & ~15ULL;
    }
};

extern "C" void task_entry_trampoline();

Task::TaskState* Task::create_task(void(*entry_point)(), uint64_t stack_size)
{
    if (entry_point == nullptr || stack_size == 0)
    {
        return nullptr;
    }

    const uint64_t aligned_stack_size = align_stack_size(stack_size);

    TaskState* task = (TaskState*)kmalloc(sizeof(TaskState));

    if (task == nullptr)
    {
        return nullptr;
    }

    void* stack = kmalloc(aligned_stack_size);
    if (stack == nullptr)
    {
        kfree(task);
        return nullptr;
    }

    for (uint32_t i = 0; i < 32; i++)
    {
        task->cpu_context.x[i] = 0;
    }

    task->cpu_context.x[19] = (uint64_t)entry_point;

    task->cpu_context.sp = (uint64_t)stack + aligned_stack_size;
    task->cpu_context.pc = (uint64_t)task_entry_trampoline;

    task->cpu_context.pstate = 0x5;

    task->stack_base = (uint64_t)stack;
    task->stack_size = aligned_stack_size;
    task->state = EState::Ready;

    task->wake_tick = 0;

    task->next_task = nullptr;

    return task;
}

Task::TaskState* Task::create_blank_task()
{
    TaskState* task = (TaskState*)kmalloc(sizeof(TaskState));

    if (task == nullptr)
    {
        return nullptr;
    }

    const uint64_t aligned_stack_size = align_stack_size(4096);

    void* stack = kmalloc(aligned_stack_size);
    if (stack == nullptr)
    {
        kfree(task);
        return nullptr;
    }

    for (uint32_t i = 0; i < 32; i++)
    {
        task->cpu_context.x[i] = 0;
    }

    task->cpu_context.sp = (uint64_t)stack + aligned_stack_size;
    task->cpu_context.pc = (uint64_t)0;

    task->cpu_context.pstate = 0x5;

    task->stack_base = (uint64_t)stack;
    task->stack_size = aligned_stack_size;
    task->state = EState::Ready;

    task->wake_tick = 0;

    task->next_task = nullptr;

    return task;
}
