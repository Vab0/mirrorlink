#ifndef _CONN_H
#define _CONN_H

#include <stdint.h>

int conn_open(char *ip, uint16_t port);

extern void conn_close(int fd);

extern int conn_read(int fd, uint8_t *buf, uint32_t len);

extern int conn_read_all(int fd, uint8_t **buf);

extern int conn_write(int fd, uint8_t *buf, uint32_t len);

#endif

