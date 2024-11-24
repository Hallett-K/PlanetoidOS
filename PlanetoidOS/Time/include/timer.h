#ifndef _PL_TIMER_H
#define _PL_TIMER_H

#include <stdint.h>
#include <util.h>

void timer_init(void);
void on_irq0(struct interrupt_frame* frame);
void sleep(uint32_t ms); // Temp
uint64_t get_ticks_since_boot(void); // Temp

#endif