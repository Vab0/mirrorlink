#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Platform/conn.h"
#include "Utils/mem.h"

struct http_req {
	char *method;
	char *path;
	char **header;
	uint8_t count;
	uint32_t blen;
	char *body;
};

struct http_rsp {
	uint16_t errcode;
	char *body;
};

struct http_req *http_client_make_req(char *method, char *path)
{
	struct http_req *req = (struct http_req *)calloc(sizeof(*req), 1);
	req->method = method;
	safe_str_append(&(req->path), path);
	return req;
}

int http_client_add_header(struct http_req *req, char *header)
{
	uint32_t len = strlen((const char *)header);
	char *h = (char *)malloc(len);
	safe_append((void **)&(req->header), (req->count + 1) * sizeof(*(req->header)));
	if (!req->header) {
		return -1;
	}
	strcpy((char *)h, (char *)header);
	req->header[req->count++] = h;
	return 0;
}

int htto_client_set_body(struct http_req *req, char *body)
{
	if (strcmp(req->method, "GET")) {
		safe_str_append(&(req->body), body);
		if (req->body) {
			req->blen = strlen(body);
			return 0;
		}
	}
	return -1;
}

struct http_rsp *http_client_send(char *ip, uint16_t port, struct http_req *req)
{
	int fd;
	char buf[100];
	char *wbuf = 0;
	char *rbuf = 0;
	uint8_t i;
	struct http_rsp *rsp;
	fd = conn_open(ip, port);
	safe_append((void **)&wbuf, 100);
	sprintf(wbuf, "%s %s HTTP/1.0\r\n", req->method, req->path);
	for (i = 0; i < req->count; i++) {
		safe_str_append(&wbuf, req->header[i]);
		safe_str_append(&wbuf, "\r\n");
		free(req->header[i]);
	}
	safe_str_append(&wbuf, "\r\n");
	if (!strcmp(req->method, "POST")) {
		safe_str_append(&wbuf, req->body);
	}
	free(req->body);
	free(req->header);
	free(req->path);
	free(req);
	conn_write(fd, wbuf, strlen(wbuf));
	rsp = (struct http_rsp *)calloc(sizeof(*rsp), 1);
	while (0 == conn_read_all(fd, &rbuf)) {
		char *pos = rbuf;
		uint32_t len = 0;
		int dummy = 0;
		int sr = sscanf(pos, "HTTP/1.%1d %3d %s\r\n", &dummy, (int *)&(rsp->errcode), buf);
		if (sr < 3) {
			continue;
		}
		pos = strstr(pos, "Content-Length:");
		sr = sscanf(pos, "Content-Length: %d\r\n", &len);
		if (sr < 1) {
			continue;
		}
		if (len > 0) {
			pos = strstr(pos, "\r\n\r\n") + 4;
			if (strlen(pos) >= len) {
				pos[len] = 0;
				safe_str_append(&(rsp->body), pos);
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
	free(rbuf);
	conn_close(fd);
	return rsp;
}

uint16_t http_client_get_errcode(struct http_rsp *rsp)
{
	return rsp->errcode;
}

char *http_client_get_body(struct http_rsp *rsp)
{
	return rsp->body;
}

void http_client_free_rsp(struct http_rsp *rsp)
{
	free(rsp->body);
	free(rsp);
}

