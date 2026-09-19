#include "scheduler.hpp"

#include "arch/aarch64/timer.hpp"
#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "memory/kernel_heap.hpp"

namespace
{
    Task::TaskState* current_task = nullptr;
    Task::TaskState* task_list = nullptr;
    Task::TaskState* pending_cleanup = nullptr;

    Task::TaskState* launch_context = nullptr;
    Task::TaskState* idle_task = nullptr;
};

namespace
{
    void remove_task(Task::TaskState* task)
    {
        if (task == nullptr || task_list == nullptr)
        {
            return;
        }

        if (task->next_task == task)
        {
            task_list = nullptr;
            return;
        }

        Task::TaskState* previous_task = task_list;
        while (previous_task->next_task != task)
        {
            previous_task = previous_task->next_task;
        }

        previous_task->next_task = task->next_task;

        if (task_list == task)
        {
            task_list = task->next_task;
        }
    }

    void cleanup_terminated_task()
    {
        if (pending_cleanup == nullptr)
        {
            return;
        }

        Log::info("Cleaning up terminated task");

        Task::TaskState* task = pending_cleanup;
        pending_cleanup = nullptr;

        kfree((void*)task->stack_base);
        kfree(task);
    }

    void idle_task_func()
    {
        Log::info("Scheduler idling");
        while (true)
        {
            asm volatile("wfi");
        }
    }

    Task::TaskState* find_next_ready_task(Task::TaskState* start_task)
    {
        if (task_list == nullptr || start_task == nullptr)
        {
            return nullptr;
        }

        Task::TaskState* task = start_task;

        do
        {
            if (task->state == Task::EState::Ready)
            {
                return task;
            }

            task = task->next_task;
        }
        while (task != start_task);

        return nullptr;
    }
}

void Scheduler::init()
{
    current_task = nullptr;
    task_list = nullptr;
    pending_cleanup = nullptr;

    launch_context = Task::create_blank_task();
    launch_context->state = Task::EState::Running;

    idle_task = Task::create_task(idle_task_func, 4096);
    idle_task->state = Task::EState::Ready;
}

void Scheduler::add_task(Task::TaskState* task)
{
    if (task == nullptr)
    {
        return;
    }

    const uint64_t irq_state = Interrupts::irq_save();

    task->state = Task::EState::Ready;

    if (task_list == nullptr)
    {
        task_list = task;
        task->next_task = task;

        Interrupts::irq_restore(irq_state);
        return;
    }

    task->next_task = task_list->next_task;
    task_list->next_task = task;

    Interrupts::irq_restore(irq_state);
}

void Scheduler::yield()
{
    const uint64_t irq_state = Interrupts::irq_save();

    if (current_task == nullptr)
    {
        Interrupts::irq_restore(irq_state);
        return;
    }

    Task::TaskState* next_task = current_task->next_task;

    while (next_task != current_task && next_task->state != Task::EState::Ready)
    {
        next_task = next_task->next_task;
    }

    if (next_task == current_task)
    {
        Interrupts::irq_restore(irq_state);
        return;
    }

    current_task->state = Task::EState::Ready;
    next_task->state = Task::EState::Running;

    Task::TaskState* previous_task = current_task;
    current_task = next_task;

    task_switch(previous_task, next_task);
}

void Scheduler::start()
{
    Task::TaskState* first_task = nullptr;

    if (task_list != nullptr)
    {
        first_task = task_list;
    }
    else
    {
        first_task = idle_task;
    }

    if (first_task == nullptr)
    {
        return;
    }

    current_task = first_task;
    current_task->state = Task::EState::Running;

    task_switch(launch_context, current_task);
}

[[noreturn]] void Scheduler::terminate_current()
{
    if (current_task == nullptr || current_task == idle_task)
    {
        while (true)
        {
            asm volatile("wfi");
        }
    }

    Task::TaskState* terminated_task = current_task;
    pending_cleanup = terminated_task;
    remove_task(terminated_task);

    if (task_list == nullptr)
    {
        idle_task->state = Task::EState::Running;
        current_task = idle_task;

        task_switch(terminated_task, idle_task);
        while (true)
        {
            asm volatile("wfi");
        }
    }

    Task::TaskState* next_task = find_next_ready_task(task_list);

    if (next_task == nullptr)
    {
        idle_task->state = Task::EState::Running;
        current_task = idle_task;

        task_switch(terminated_task, idle_task);
        while (true)
        {
            asm volatile("wfi");
        }
    }

    next_task->state = Task::EState::Running;
    current_task = next_task;

    task_switch(terminated_task, next_task);
    while (true)
    {
        asm volatile("wfi");
    }
}

Task::TaskState* Scheduler::get_current_task()
{
    return current_task;
}

void Scheduler::preempt(exception_context* context)
{
    if (current_task == nullptr || context == nullptr)
    {
        return;
    }

    cleanup_terminated_task();

    if (current_task == idle_task)
    {
        Task::TaskState* next_task = task_list;

        while (next_task != nullptr)
        {
            if (next_task->state == Task::EState::Ready)
            {
                break;
            }

            next_task = next_task->next_task;

            if (next_task == task_list)
            {
                next_task = nullptr;
                break;
            }
        }

        if (next_task == nullptr)
        {
            return;
        }

        next_task->state = Task::EState::Running;
        current_task = next_task;

        for (uint32_t i = 0; i < 31; i++)
        {
            context->x[i] = next_task->cpu_context.x[i];
        }

        context->sp = next_task->cpu_context.sp;
        context->elr = next_task->cpu_context.pc;
        context->spsr = next_task->cpu_context.pstate;

        return;
    }

    if (current_task->state != Task::EState::Running)
    {
        return;
    }

    for (uint32_t i = 0; i < 31; i++)
    {
        current_task->cpu_context.x[i] = context->x[i];
    }

    current_task->cpu_context.sp = context->sp;
    current_task->cpu_context.pc = context->elr;
    current_task->cpu_context.pstate = context->spsr;

    current_task->state = Task::EState::Ready;

    Task::TaskState* next_task = current_task->next_task;

    while (next_task != current_task && next_task->state != Task::EState::Ready)
    {
        next_task = next_task->next_task;
    }

    if (next_task == current_task)
    {
        current_task->state = Task::EState::Running;
        return;
    }

    next_task->state = Task::EState::Running;
    current_task = next_task;

    for (uint32_t i = 0; i < 31; i++)
    {
        context->x[i] = next_task->cpu_context.x[i];
    }

    context->sp = next_task->cpu_context.sp;
    context->elr = next_task->cpu_context.pc;
    context->spsr = next_task->cpu_context.pstate;
}

void Scheduler::sleep(uint64_t ticks)
{
    if (current_task == nullptr || ticks == 0)
    {
        return;
    }

    const uint64_t irq_state = Interrupts::irq_save();

    Task::TaskState* previous_task = current_task;

    current_task->wake_tick = Timer::get_ticks() + ticks;
    current_task->state = Task::EState::Sleeping;

    Task::TaskState* next_task = current_task->next_task;

    while (next_task != previous_task && next_task->state != Task::EState::Ready)
    {
        next_task = next_task->next_task;
    }

    if (next_task == previous_task)
    {
        idle_task->state = Task::EState::Running;
        current_task = idle_task;

        task_switch(previous_task, idle_task);
        return;
    }

    next_task->state = Task::EState::Running;
    current_task = next_task;

    task_switch(previous_task, next_task);
}

void Scheduler::update_sleeping_tasks()
{
    if (task_list == nullptr)
    {
        return;
    }

    const uint64_t current_tick = Timer::get_ticks();

    Task::TaskState* task = task_list;

    do
    {
        if (task->state == Task::EState::Sleeping && current_tick >= task->wake_tick)
        {
            task->state = Task::EState::Ready;
        }

        task = task->next_task;
    }
    while (task != task_list);
}

extern "C" [[noreturn]] void task_terminated()
{
    Scheduler::terminate_current();
}