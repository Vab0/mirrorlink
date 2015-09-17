#ifndef _HTTP_CLIENT_H
#define _HTTP_CLIENT_H

#include <stdint.h>

struct http_req;

struct http_rsp;

extern struct http_req *http_client_make_req(uint8_t method);

extern void http_client_add_header(struct http_req *req, uint8_t *header);

extern struct http_rsp *http_client_send(uint8_t *ip, uint16_t port, struct http_req *req);

extern void http_client_free_req(struct http_req *req);

extern uint16_t http_client_get_errcode(struct http_rsp *rsp);

extern uint8_t *http_client_get_body(struct http_rsp *rsp);

extern void http_client_free_rsp(struct http_rsp *rsp);

#endif
