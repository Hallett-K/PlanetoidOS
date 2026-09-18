#include "scheduler.hpp"

namespace
{
    Task::TaskState* current_task = nullptr;
    Task::TaskState* task_list = nullptr;

    Task::TaskState* launch_context = nullptr;
};

void Scheduler::init()
{
    current_task = nullptr;
    task_list = nullptr;

    launch_context = Task::create_blank_task();
    launch_context->state = Task::EState::Running;
}

void Scheduler::add_task(Task::TaskState* task)
{
    if (task == nullptr)
    {
        return;
    }

    task->state = Task::EState::Ready;

    if (task_list == nullptr)
    {
        task_list = task;
        task->next_task = task;
        return;
    }

    task->next_task = task_list->next_task;
    task_list->next_task = task;
}

void Scheduler::yield()
{
    if (current_task == nullptr)
    {
        return;
    }

    Task::TaskState* next_task = current_task->next_task;

    while (next_task != current_task && next_task->state != Task::EState::Ready)
    {
        next_task = next_task->next_task;
    }

    if (next_task == current_task)
    {
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
    if (task_list == nullptr)
    {
        return;
    }

    current_task = task_list;
    current_task->state = Task::EState::Running;

    task_switch(launch_context, current_task);
}

Task::TaskState* Scheduler::get_current_task()
{
    return current_task;
}