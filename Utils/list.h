#ifndef _LIST_H
#define _LIST_H

#include <stddef.h>

#define LIST_HEAD_ENTRY(type, mem, ptr) (type *)((unsigned char *)(ptr) - offsetof(type, mem))

#define LIST_HEAD_FOREACH(head, cur, tmp) \
for (cur = head->next, tmp = cur->next; cur != head; cur = tmp, tmp = cur->next)

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

extern void list_head_insert(struct list_head *head, struct list_head *v);

extern void list_head_remove(struct list_head *v);

#endif
