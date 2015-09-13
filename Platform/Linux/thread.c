#include "Platform/thread.h"
#include "Utils/list.h"

#include <pthread.h>

#include <sys/timerfd.h>

#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

struct timer {
	int fd;
	int black;
	void *args;
	void (*dispatch)(int fd, void *args);
	struct list_head list;
};

struct thread_param {
	struct list_head timer;
	/* reference to msg queue */
	thread_func func;
	void *args;
};

static pthread_key_t key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

static void once_make_key(void);
static void once_destructor(void *args);
static void *common_thread_entry(void *args);

void once_make_key(void)
{
	pthread_key_create(&key, once_destructor);
}

void once_destructor(void *args)
{
	struct thread_param *param = (struct thread_param *)args;
	struct list_head *head = &(param->timer);
	struct list_head *cur;
	struct list_head *tmp;
	free(param->args);
	for (cur = head->next, tmp = cur->next; cur != head; cur = tmp, tmp = cur->next) {
		struct timer *timer = LIST_HEAD_ENTRY(struct timer, list, cur);
		list_head_remove(cur);
		free(timer);
	}
	free(param);
}

void *common_thread_entry(void *args)
{
	struct thread_param *param = (struct thread_param *)args;
	pthread_once(&key_once, once_make_key);
	if (0 == pthread_getspecific(key)) {
		pthread_setspecific(key, param);
	}
	param->func(param->args);
}

void thread_post(thread_func f, void *args)
{
	struct thread_param *param = (struct thread_param *)malloc(sizeof(*param));
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	param->func = f;
	param->args = args;
	param->timer.next = param->timer.prev = &(param->timer);
	/* reference to msg queue from tls */
	pthread_create(&thread, &attr, common_thread_entry, param);
}

int timer_start(uint32_t s, timer_func f, void *args)
{
	struct timer *t;
	struct itimerspec ts;
	int ret;
	int fd;
	uint64_t v = 0;
	struct list_head *head = &(((struct thread_param *)pthread_getspecific(key))->timer);
	if (0 == head) {
		return -1;
	}
	fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (-1 == fd) {
		return -1;
	}

	memset(&ts, 0, sizeof(ts));
	ts.it_interval.tv_sec = s;
	ret = timerfd_settime(fd, 0, &ts, 0);
	if (-1 == ret) {
		close(fd);
		return -1;
	}
	t = (struct timer *)malloc(sizeof(*t));
	t->fd = fd;
	t->black = 0;
	t->args = args;
	t->dispatch = f;
	list_head_insert(head, &(t->list));
	return fd;
}

void timer_dispatch(void)
{
	struct list_head *head = &(((struct thread_param *)pthread_getspecific(key))->timer);
	struct list_head *cur;
	struct list_head *tmp;

	if (0 == head) {
		return;
	}
	
	for (cur = head->next, tmp = cur->next; cur != head; cur = tmp, tmp = cur->next) {
		struct timer *timer = LIST_HEAD_ENTRY(struct timer, list, cur);
		if (0 == timer->black) {
			uint64_t val;
			int ret;
read_intr:
			ret = read(timer->fd, &val, sizeof(val));
			if (-1 == ret) {
				if (EINTR == errno) {
					goto read_intr;
				} else if (EAGAIN == errno || EWOULDBLOCK == errno) {
					continue;
				} else {
					break;
				}
			} else {
				timer->black = 1;
				timer->dispatch(timer->fd, timer->args);
			}
		}
	}
	
	{
		struct list_head *cur;
		struct list_head *tmp;
		for (cur = head->next, tmp = cur->next; cur != head; cur = tmp, tmp = cur->next) {
			struct timer *timer = LIST_HEAD_ENTRY(struct timer, list, cur);
			if (1 == timer->black) {
				timer->black = 0;
			} else if (2 == timer->black) {
				list_head_remove(cur);
				free(timer);
			}
		}
	}
}

void timer_stop(int fd)
{
	struct list_head *head = &(((struct thread_param *)pthread_getspecific(key))->timer);
	struct list_head *cur;
	struct list_head *tmp;

	if (0 == head) {
		return;
	}
	for (cur = head->next, tmp = cur->next; cur != head; cur = tmp, tmp = cur->next) {
		struct timer *timer = LIST_HEAD_ENTRY(struct timer, list, cur);
		if (fd == timer->fd) {
			if (timer->black) {
				timer->black = 2;
			} else {
				list_head_remove(cur);
				free(timer);
			}
			break;
		}
	}
}

