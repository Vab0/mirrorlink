#ifndef _REMOTE_SERVER_H
#define _REMOTE_SERVER_H

#include <stdint.h>

struct remote_server;

extern struct remote_server *remote_server_create(char *ip, uint16_t port, char *path);

extern int remote_server_get_application_list(struct remote_server *server);
extern void remote_server_destory(struct remote_server *server);
#endif
