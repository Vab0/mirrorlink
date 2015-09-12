#include <sys/timerfd.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

int timer_wait(uint32_t s)
{
	struct itimerspec t;
	int ret;
	uint64_t v = 0;
	int fd = timerfd_create(CLOCK_MONOTONIC, 0);
	if (-1 == fd) {
		return -1;
	}
	
	memset(&t, 0, sizeof(t));
	t.it_value.tv_sec = s;
	ret = timerfd_settime(fd, 0, &t, 0);
	if (-1 == ret) {
		close(fd);
		return -1;
	}
read_intr:
	ret = read(fd, &v, sizeof(v));
	if (-1 == ret) {
		if (EINTR == errno) {
			goto read_intr;
		} else {
			close(fd);
			return -1;
		}
	} else {
		close(fd);
	}
	return 0;
}

int timer_start(uint32_t s)
{
	struct itimerspec t;
	int ret;
	uint64_t v = 0;
	int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
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
	return fd;
}

int timer_check(int fd)
{
	uint64_t val;
	int ret;
read_intr:
	ret = read(fd, &val, sizeof(val));
	if (-1 == ret) {
		if (EINTR == errno) {
			goto read_intr;
		} else if (EAGAIN == errno || EWOULDBLOCK == errno) {
			return 0;
		} else {
			return -1;
		}
	} else {
		return ret;
	}
}

void timer_stop(int fd)
{
	close(fd);
}


