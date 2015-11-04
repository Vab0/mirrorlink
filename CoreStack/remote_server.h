#ifndef _REMOTE_SERVER_H
#define _REMOTE_SERVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct remote_server;

extern struct remote_server *remote_server_create(const char *ip, uint16_t port, const char *path);

extern uint16_t remote_server_get_application_list(struct remote_server *server, uint32_t pid, char *filter);

extern uint16_t remote_server_launch_application(struct remote_server *server, uint32_t appid, uint32_t pid);

extern uint16_t remote_server_set_client_profile(struct remote_server *server, uint32_t pid);

extern void remote_server_destory(struct remote_server *server);

#ifdef __cplusplus
}
#endif

#endif
