#include <stdio.h>
#include "CoreStack/remote_server.h"

int main(int argc, char *argv[])
{
	struct remote_server *server = 0;
	char buf[100] = {0};
	char ip[16] = {0};
	char path[100] = {0};
	int ret = 0;
	int port;
	if (argc != 2) {
		return -1;
	}
	ret = sscanf(buf, "%s:%d:%s", ip, &port, path);
	if (ret != 3) {
		return -1;
	}
	server = remote_server_create(ip, port, path);
	remote_server_get_application_list(server);
	remote_server_destory(server);
	return 0;
}
