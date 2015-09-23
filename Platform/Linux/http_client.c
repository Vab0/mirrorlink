#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Platform/conn.h"
#include "Utils/buffer.h"
#include "Utils/str.h"

struct http_req {
	str_t method;
	str_t path;
	str_t header;
	str_t body;
};

struct http_rsp {
	uint16_t errcode;
	str_t body;
};

struct http_req *http_client_make_req(char *method, char *path)
{
	struct http_req *req = (struct http_req *)calloc(1, sizeof(*req));
	str_append(&(req->method), method);
	str_append(&(req->path), path);
	return req;
}

void http_client_add_header(struct http_req *req, char *header)
{
	str_append(&(req->header), header);
}

int http_client_set_body(struct http_req *req, char *body)
{
	if (strcmp(req->method, "GET")) {
		str_append(&(req->body), body);
		return 0;
	}
	return -1;
}

struct http_rsp *http_client_send(char *ip, uint16_t port, struct http_req *req)
{
	int fd;
	char buf[100] = {0};
	str_t wbuf = 0;
	struct buffer rbuf;
	uint8_t i;
	struct http_rsp *rsp;
	int quit = 0;
	buffer_init(&rbuf, 0);
	fd = conn_open(ip, port);
	sprintf(buf, "%s %s HTTP/1.0\r\n", req->method, req->path);
	str_append(&wbuf, buf);
	if (req->body) {
		sprintf(buf, "Content-Length:%d\r\n", (int)strlen(req->body));
		str_append(&(req->header), buf);
	}
	if (req->header) {
		str_append(&wbuf, req->header);
	}
	str_append(&wbuf, "\r\n");
	if (!strcmp(req->method, "POST") && req->body) {
		str_append(&wbuf, req->body);
	}
	free(req->method);
	free(req->body);
	free(req->header);
	free(req->path);
	free(req);
	conn_write(fd, wbuf, strlen(wbuf));
	printf("request is %s\n", wbuf);
	rsp = (struct http_rsp *)calloc(1, sizeof(*rsp));
	while (0 == quit) {
		char *pos;
		uint32_t len = 0;
		int dummy = 0;
		int sr;
		quit = conn_read_all(fd, &rbuf);
		if (!rbuf.len) {
			continue;
		}
		buffer_append(&rbuf, 1);
		pos = rbuf.buf;
		printf("response is %s\n", pos);
		sr = sscanf(pos, "HTTP/1.%1d %3d %*s\r\n", &dummy, (int *)&(rsp->errcode));
		if (sr < 2) {
			continue;
		}
		printf("error code is %d\n", rsp->errcode);
		pos = strstr(pos, "Content-Length:");
		sr = sscanf(pos, "Content-Length: %d\r\n", &len);
		if (sr < 1) {
			continue;
		}
		if (len > 0) {
			pos = strstr(pos, "\r\n\r\n") + 4;
			if (strlen(pos) >= len) {
				str_append(&(rsp->body), pos);
				break;
			}
		} else {
			break;
		}
	}
	if (0 == rsp->errcode) {
		free(rsp);
		rsp = 0;
	}
	free(wbuf);
	buffer_clear(&rbuf);
	conn_close(fd);
	return rsp;
}

uint16_t http_client_get_errcode(struct http_rsp *rsp)
{
	if (rsp) {
		return rsp->errcode;
	} else {
		return 0;
	}
}

char *http_client_get_body(struct http_rsp *rsp)
{
	if (rsp) {
		return rsp->body;
	} else {
		return 0;
	}
}

void http_client_free_rsp(struct http_rsp *rsp)
{
	if (rsp) {
		free(rsp->body);
	}
	free(rsp);
}

