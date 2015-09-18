#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Platform/conn.h"
#include "Utils/mem.h"

struct http_req {
	uint8_t method;
	uint8_t **header;
	uint8_t count;
};

struct http_rsp {
	uint16_t errcode;
	uint8_t *body;
};

struct http_req *http_client_make_req(uint8_t method)
{
	struct http_req *req = (struct http_req *)malloc(sizeof(*req));
	req->method = method;
	req->header = 0;
	req->count = 0;
	return req;
}

int http_client_add_header(struct http_req *req, uint8_t *header)
{
	uint32_t len = strlen((const char *)header);
	uint8_t *h = (uint8_t *)malloc(len);
	safe_append((void **)&(req->header), (req->count + 1) * sizeof(*(req->header)));
	if (!req->header) {
		return -1;
	}
	strcpy((char *)h, (char *)header);
	req->header[req->count++] = h;
	return 0;
}

struct http_rsp *http_client_send(char *ip, uint16_t port, struct http_req *req)
{
	int fd;
	uint8_t *wbuf = 0;
	uint32_t wlen = 0;
	uint8_t *rbuf = 0;
	int ret = 0;
	struct http_rsp *rsp;
	fd = conn_open(ip, port);
	conn_write(fd, wbuf, wlen);
	while (0 != conn_read_all(fd, &rbuf)) {
		/* parse http response here */
	}
	rsp = (struct http_rsp *)malloc(sizeof(*rsp));
	conn_close(fd);
	return rsp;
}

void http_client_free_req(struct http_req *req)
{
	uint8_t i;
	for (i = 0; i < req->count; i++) {
		free(req->header[i]);
	};
	free(req->header);
	free(req);
}

uint16_t http_client_get_errcode(struct http_rsp *rsp)
{
	return rsp->errcode;
}

uint8_t *http_client_get_body(struct http_rsp *rsp)
{
	return rsp->body;
}

void http_client_free_rsp(struct http_rsp *rsp)
{
	free(rsp->body);
	free(rsp);
}

