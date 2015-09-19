#include <stdio.h>
#include "Platform/http_client.h"

int main(void)
{
	struct http_req *rq = http_client_make_req("GET", "/0c26a07c-ac09-48bc-a3d6-dcaff5f323cf.xml");
	struct http_rsp *rp = http_client_send("192.168.1.8", 45000, rq);
	printf("errcode = %d\nbody = %s\n",
		http_client_get_errcode(rp), http_client_get_body(rp));
	http_client_free_rsp(rp);
	return 0;
}
