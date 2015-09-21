#ifndef _HTTP_CLIENT_H
#define _HTTP_CLIENT_H

#include <stdint.h>

struct http_req;

struct http_rsp;

extern struct http_req *http_client_make_req(char *method, char *path);

extern void http_client_add_header(struct http_req *req, char *header);

extern int http_client_set_body(struct http_req *req, uint8_t *body);

extern struct http_rsp *http_client_send(char *ip, uint16_t port, struct http_req *req);

extern uint16_t http_client_get_errcode(struct http_rsp *rsp);

extern char *http_client_get_body(struct http_rsp *rsp);

extern void http_client_free_rsp(struct http_rsp *rsp);

#endif
