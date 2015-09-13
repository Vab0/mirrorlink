#include "vnc_session.h"

#include <string.h>
#include <stdlib.h>

#include "Platform/conn.h"

void vnc_session_main_task(void *args)
{
	struct vnc_session *session = (struct vnc_session *)args;
	int fd = conn_open(session->ip, session->port);
	/* Protocol Version Handshake */
	{
		uint8_t buf[12];
		conn_read(fd, buf, 12);
		if (0 != memcmp(buf, "RFB 003.008\n", 12)) {
			conn_close(fd);
			return;
		} else {
			conn_write(fd, buf, 12);
		}
	}
	/* Security Handshake */
	{
		uint8_t num = 0;
		uint8_t *buf = 0;
		conn_read(fd, &num, 1);
		if (0 == num) {
			uint32_t n;
			conn_read(fd, (uint8_t *)&n, 4);
			buf = (uint8_t *)malloc(n);
			conn_read(fd, buf, n);
			free(buf);
			conn_close(fd);
			return;
		} else {
			uint8_t i;
			uint8_t flag;
			buf = (uint8_t *)malloc(num);
			conn_read(fd, buf, num);
			for (i = 0; i < num; i++) {
				if (0 == buf[i]) {
					break;
				} else if (1 == buf[i]) {
					flag = 1;
				}
			}
			free(buf);
			if (i != num) {
				if (flag) {
					conn_write(fd, &flag, 1);
				} else {
					conn_close(fd);
					return;
				}
			} else {
				conn_close(fd);
				return;
			}
		}
	}
	/* Security Result Handshake */
	{
		uint32_t r;
		uint32_t rlen;
		uint8_t *buf = 0;
		conn_read(fd, (uint8_t *)&r, 4);
		if (1 == r) {
			conn_read(fd, (uint8_t *)&rlen, 4);
			buf = (uint8_t *)malloc(rlen);
			conn_read(fd, buf, rlen);
			free(buf);
			conn_close(fd);
			return;
		} else if (0 != r) {
			free(buf);
			conn_close(fd);
			return;
		}
	}
	/* Initialization Messages */
	{
		uint8_t val = 0;
		uint8_t *buf = 0;
		uint8_t *tmp = 0;
		uint32_t len = 0;
		conn_write(fd, &val, 1);
		buf = (uint8_t *)malloc(24);
		conn_read(fd, buf, 24);
		len = ntohl(*(uint32_t *)(buf + 20));
		tmp = (uint8_t *)realloc(buf, 24 + len);
		if (tmp) {
			buf = tmp;
			conn_read(fd, buf + 24, len);
		} else {
			free(buf);
			conn_close(fd);
			return;
		}
		/* parse server infomation */
		free(buf);
	}
}

