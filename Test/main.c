#include <stdio.h>
#include "Platform/http_client.h"

int main(void)
{
	struct http_req *rq = http_client_make_req("GET", "/");
	struct http_rsp *rp = http_client_send("115.239.210.27", 80, rq);
	printf("errcode = %d\nbody = %s\n",
		http_client_get_errcode(rp), http_client_get_body(rp));
	http_client_free_rsp(rsp);
	return 0;
}
