#ifndef _PL_KEYBOARD_H
#define _PL_KEYBOARD_H

#include <util.h>

void keyboard_init(void);
void on_irq1(struct interrupt_frame* frame);

#define PL_KEY_LEFT_SHIFT 42
#define PL_KEY_RIGHT_SHIFT 54
#define PL_KEY_CAPS_LOCK 58

#endif // _PL_KEYBOARD_H