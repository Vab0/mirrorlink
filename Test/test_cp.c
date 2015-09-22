#include <stdio.h>
#include "CoreStack/remote_server.h"

int main(int argc, char *argv[])
{
	struct remote_server *server = 0;
#if 0
	char buf[100] = {0};
	char ip[16] = {0};
	char path[100] = {0};
	int ret = 0;
	int port;
	if (argc != 2) {
		printf("argc is %d\n", argc);
		return -1;
	}
	ret = sscanf(argv[1], "%s:%5d:%s", ip, &port, path);
	if (ret != 3) {
		printf("command parse error, %d\n", ret);
		return -1;
	}
	server = remote_server_create(ip, port, path);
#else
	server = remote_server_create("172.26.176.168", 45000, "/81db4fed-6235-4211-a762-87d3985ac8dc.xml");
#endif
	remote_server_get_application_list(server);
	remote_server_destory(server);
	return 0;
}
