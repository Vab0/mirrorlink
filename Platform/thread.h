#ifndef _THREAD_H
#define _THREAD_H

#include <stdint.h>

typedef void (*thread_func)(void *args);

typedef void (*timer_func)(int fd, void *args);

extern void thread_post(thread_func f, void *args);

extern int timer_start(uint32_t s, timer_func f, void *args);

extern void timer_stop(int fd);

extern void timer_dispatch(void);

#endif
