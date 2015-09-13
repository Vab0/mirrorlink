#ifndef _REFCNT_H
#define _REFCNT_H

#include <stdint.h>

struct refcnt {
	volatile uint32_t cnt;
	void *data;
	void (*destructor)(void *);
};

extern void refcnt_bind(struct refcnt *ref, void *data, void (*dtor)(void *));

extern void refcnt_inc(struct refcnt *ref);

extern void refcnt_dec(struct refcnt *ref);

extern void *refcnt_get(struct refcnt *ref);

#endif
