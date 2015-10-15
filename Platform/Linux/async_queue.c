#include <stdint.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <sys/select.h>
#include <errno.h>

struct async_queue {
	int efd;
	uint16_t head;
	uint16_t tail;
	void **buf;
	uint16_t size;
};

struct async_queue *async_queue_create(uint16_t size)
{
	struct async_queue *queue = (struct async_queue *)calloc(1, sizeof(*queue));
	if (!size) {
		return 0;
	}
	queue->efd = eventfd(0, EFD_SEMAPHORE|EFD_NONBLOCK);
	queue->buf = malloc(sizeof(*(queue->buf)) * size);
	queue->size = size;
	return queue;
}

uint8_t async_queue_push(struct async_queue *queue, void *data)
{
	fd_set wfds;
	fd_set efds;
	struct timeval tv;
	int ret;
	FD_ZERO(&efds);
	FD_ZERO(&wfds);
	FD_SET(fd, &wfds);
	FD_SET(fd, &efds);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if ((queue->tail + 1) % queue->size == queue->head) {
		return 0;
	}
	queue->buf[queue->tail] = data;
	queue->tail = (queue->tail + 1) % queue->size;
select_intr:
	ret = select(queue->efd + 1, 0, &wfds, &efds, &tv);
	if (0 == ret) {
		return 0;
	} else if (-1 == ret) {
		if (EINTR == errno) {
			goto select_intr;
		} else {
			return 0;
		}
	} else {
		uint32_t i;
		for (i = 0; i < ret; i++) {
			if (FD_ISSET(fd, &efds)) {
				return 0;
			} else if (FD_ISSET(fd, &wfds)) {
				int t;
				uint64_t dummy = 1;
write_intr:
				t = write(queue->efd, &dummy, 8);
				if (t <= 0) {
					if ((EAGAIN == errno) || (EWOULDBLOCK == errno)) {
						goto select_intr;
					} else if (EINTR == errno) {
						goto write_intr;
					} else {
						return 0;
					}
				}
			}
		}
	}
	return 1;
}

void *async_queue_pop(struct async_queue *queue)
{
	fd_set rfds;
	fd_set efds;
	struct timeval tv;
	int ret;
	void *data;
	FD_ZERO(&efds);
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	FD_SET(fd, &efds);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
select_intr:
	ret = select(queue->efd + 1, &rfds, 0, &efds, &tv);
	if (0 == ret) {
		return -2;
	} else if (-1 == ret) {
		if (EINTR == errno) {
			goto select_intr;
		} else {
			return 0;
		}
	} else {
		uint32_t i;
		for (i = 0; i < ret; i++) {
			if (FD_ISSET(fd, &efds)) {
				return 0;
			} else if (FD_ISSET(fd, &rfds)) {
				int t;
				uint64_t dummy;
read_intr:
				t = read(fd, &dummy, 8);
				if (-1 == t) {
					if ((EAGAIN == errno) || (EWOULDBLOCK == errno)) {
						goto select_intr;
					} else if (EINTR == errno) {
						goto read_intr;
					}
				}
			}
		}
	}
	if (queue->head == queue->tail) {
		return 0;
	}
	data = queue->buf[queue->head];
	queue->head = (queue->head + 1) % queue->size;
	return data;
}

void async_queue_destory(struct async_queue *queue)
{
	if (queue) {
		free(queue->buf);
		free(queue);
		close(queue->efd);
	}
}

