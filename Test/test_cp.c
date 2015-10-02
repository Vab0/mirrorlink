#include <stdio.h>
#include "CoreStack/remote_server.h"

int main(int argc, char *argv[])
{
	struct remote_server *server = 0;
#if 1
	char *ip = 0;
	char *path = 0;
	int ret = 0;
	int port;

	if (argc != 4) {
		printf("argc is %d\n", argc);
		return -1;
	}
	ip = argv[1];
	sscanf(argv[2], "%d", &port);
	path = argv[3];

	server = remote_server_create(ip, port, path);
#else
	server = remote_server_create("192.168.1.6", 45000, "/97ab1f15-ca94-492f-b0c0-9a98694050b3.xml");
#endif
	remote_server_get_application_list(server, 0, "*");
	remote_server_destory(server);
	return 0;
}
