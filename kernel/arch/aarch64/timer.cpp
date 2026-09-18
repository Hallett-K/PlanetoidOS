#include "timer.hpp"

#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "core/task/scheduler.hpp"

namespace
{
    uint64_t timer_ticks = 0;
    uint64_t system_ticks = 0;
    uint32_t timer_frequency = 0;
}

uint64_t read_counter_frequency()
{
    uint64_t value;
    asm volatile("mrs %0, CNTFRQ_EL0" 
        : "=r"(value));
    return value;
}

uint64_t get_counter_value()
{
    uint64_t value;
    asm volatile("mrs %0, CNTPCT_EL0" 
        : "=r"(value));
    return value;
}

void on_timer_interrupt(uint32_t interrupt_id, exception_context* context)
{
    (void)interrupt_id;
    Timer::on_interrupt();
    Scheduler::preempt(context);
}

void Timer::init(uint32_t frequency)
{
    const uint64_t counter_frequency = read_counter_frequency();
    timer_ticks = counter_frequency / frequency;
    timer_frequency = frequency;

    asm volatile("msr CNTP_TVAL_EL0, %0"
        :
        : "r"(timer_ticks));

    asm volatile("msr CNTP_CTL_EL0, %0"
        :
        : "r"(1ULL));

    Interrupts::register_handler(30, on_timer_interrupt);
}

void Timer::on_interrupt()
{
    asm volatile("msr CNTP_TVAL_EL0, %0"
        :
        : "r"(timer_ticks));

    system_ticks++;
}

uint64_t Timer::get_ticks()
{
    return system_ticks;
}

void Timer::delay(uint64_t ticks)
{
    const uint64_t target = system_ticks + ticks;

    while (system_ticks < target)
    {
        asm volatile("wfe");
    }
}

uint32_t Timer::get_frequency()
{
    return timer_frequency;
}

uint64_t Timer::get_milliseconds()
{
    return (system_ticks * 1000) / timer_frequency;
}