#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdint.h>

struct buffer {
	uint8_t *buf;
	uint32_t size;
	uint32_t len;
};

extern void buffer_init(struct buffer *buf, uint32_t size);

extern void buffer_clear(struct buffer *buf);

extern void buffer_append(struct buffer *buf, int size);

#endif
