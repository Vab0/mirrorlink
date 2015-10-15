#ifndef _ASYNC_QUEUE_H
#define _ASYNC_QUEUE_H

struct async_queue;

extern struct async_queue *async_queue_create(uint16_t size);

extern uint8_t async_queue_push(struct async_queue *queue, void *data);

extern void *async_queue_pop(struct async_queue *queue);

extern void async_queue_destory(struct async_queue *queue);

#endif
