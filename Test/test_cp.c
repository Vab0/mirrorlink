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
	server = remote_server_create("192.168.1.8", 45000, "/616546a3-e346-4c46-b304-4a1d26e571df.xml");
#endif
	remote_server_get_application_list(server);
	remote_server_destory(server);
	return 0;
}
