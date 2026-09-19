#include "wait_queue.hpp"

#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "scheduler.hpp"

void WaitQueue::init(TaskWaitQueue* queue)
{
    if (queue == nullptr)
    {
        return;
    }

    queue->head = nullptr;
    queue->tail = nullptr;
}

void WaitQueue::block_current(TaskWaitQueue* queue)
{
    if (queue == nullptr)
    {
        return;
    }

    Task::TaskState* current_task = Scheduler::get_current_task();
    if (current_task == nullptr)
    {
        return;
    }

    current_task->state = Task::EState::Blocked;
    current_task->wait_next = nullptr;

    if (queue->tail == nullptr)
    {
        queue->head = current_task;
        queue->tail = current_task;
    }
    else
    {
        queue->tail->wait_next = current_task;
        queue->tail = current_task;
    }

    Scheduler::block_current_task();
}

void WaitQueue::wake_one(TaskWaitQueue* queue)
{
    if (queue == nullptr)
    {
        return;
    }

    Task::TaskState* task = queue->head;
    if (task == nullptr)
    {
        return;
    }

    queue->head = task->wait_next;

    if (queue->head == nullptr)
    {
        queue->tail = nullptr;
    }

    task->wait_next = nullptr;
    task->state = Task::EState::Ready;
}

void WaitQueue::wake_all(TaskWaitQueue* queue)
{
    if (queue == nullptr)
    {
        return;
    }

    while (queue->head != nullptr)
    {
        wake_one(queue);
    }
}