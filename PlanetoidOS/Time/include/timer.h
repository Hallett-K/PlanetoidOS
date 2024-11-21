#ifndef _PL_TIMER_H
#define _PL_TIMER_H

#include <stdint.h>
#include <util.h>

void init_timer(void);
void onIrq0(struct interrupt_frame* frame);

#endif