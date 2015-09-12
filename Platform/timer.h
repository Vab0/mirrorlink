#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

extern int timer_wait(uint32_t s);

extern int timer_start(uint32_t s);

extern int timer_check(int fd);

extern void timer_stop(int fd);

#endif
