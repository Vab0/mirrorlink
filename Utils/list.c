#include "list.h"

void list_head_insert(struct list_head *head, struct list_head *v)
{
	v->next = head;
	v->prev = head->prev;
	head->prev->next = v;
	head->prev = v;
}

void list_head_remove(struct list_head *v)
{
	v->prev->next = v->next;
	v->next->prev = v->prev;
}
