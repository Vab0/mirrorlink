#include <stdint.h>
#include "Platform/refcnt.h"

void refcnt_bind(struct refcnt *ref, void *data, void (*dtor)(void *))
{
	uint32_t val = 1;
	ref->data = data;
	ref->destructor = dtor;
	__atomic_store(&(ref->cnt), &val, 0);
}

void refcnt_inc(struct refcnt *ref)
{
	__atomic_add_fetch(&(ref->cnt), 1, 0);
}

void refcnt_dec(struct refcnt *ref)
{
	if (0 == __atomic_sub_fetch(&(ref->cnt), 1, 0)) {
		ref->destructor(ref->data);
	}
}

void *refcnt_get(struct refcnt *ref)
{
	uint32_t val = 0;
	__atomic_load(&(ref->cnt), &val, 0);
	if (val) {
		return ref->data;
	} else {
		return 0;
	}
}
