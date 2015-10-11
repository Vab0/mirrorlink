#include <stdio.h>
#include "CoreStack/remote_server.h"

int main(int argc, char *argv[])
{
	struct remote_server *server = 0;
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
/*	remote_server_set_client_profile(server, 0); */
	if (server) {
		uint32_t id = 0;
		remote_server_get_application_list(server, 0, "*");
		scanf("%d", &id);
		remote_server_launch_application(server, id, 0);
		scanf("%d", &id);
		vnc_session_main_task(ip, id);
	}
	remote_server_destory(server);
	return 0;
}
