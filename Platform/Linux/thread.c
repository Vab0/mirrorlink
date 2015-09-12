#include "../thread.h"

#include <pthread.h>

#include <stdlib.h>

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

struct timer {
	int fd;
	int black;
	void *args;
	void (*dispatch)(int fd, void *args);
	struct list_head *list;
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
	free(param->args);
	/* destory timer */
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
	struct thread_param *param = (struct thread_param *)malloc(*param);
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	param->func = f;
	param->args = args;
	param->timer.next = param->timer.prev = &(param->timer);
	/* reference to msg queue from tls */
	pthread_create(&thread, &attr, THREAD_FUNCTION, param);
}

int timer_start(uint32_t s, timer_func f, void *args);
{
	struct timer *t;
	struct itimerspec t;
	int ret;
	int fd;
	uint64_t v = 0;
	struct list_head *head = ((struct thread_param *)pthread_getspecific(key))->timer;
	if (0 == head) {
		return -1;
	}
	fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (-1 == fd) {
		return -1;
	}

	memset(&t, 0, sizeof(t));
	t.it_interval.tv_sec = s;
	ret = timerfd_settime(fd, 0, &t, 0);
	if (-1 == ret) {
		close(fd);
		return -1;
	}
	t = (struct timer *)malloc(sizeof(*t));
	t->fd = fd;
	t->black = 0;
	t->args = args;
	t->dispatch = f;
	/* list insert to head */
	return fd;
}

void timer_dispatch(void)
{
	struct list_head *head = ((struct thread_param *)pthread_getspecific(key))->timer;
	struct timer *timer = 0;
	if (0 == head) {
		return;
	}
	{
		uint64_t val;
		int ret;
read_intr:
		ret = read(fd, &val, sizeof(val));
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
	{
		if (1 == timer->black) {
			timer->black = 0;
		} else if (2 == timer->black) {
			/* remove from head */
			free(timer);
		}
	}
}

void timer_stop(int fd)
{
	struct list_head *head = ((struct thread_param *)pthread_getspecific(key))->timer;
	struct timer *timer = 0;
	if (0 == head) {
		return;
	}
	if (timer->black) {
		timer->black = 2;
	} else {
		/* remove from head */
		free(timer);
	}
}

