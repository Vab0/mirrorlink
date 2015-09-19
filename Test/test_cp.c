#include <stdio.h>
#include "CoreStack/remote_server.h"

int main(void)
{
	struct remote_server *server = remote_server_create("192.168.1.8", 45000, "/02bcde8a-f44e-459a-ba5f-00afef221505.xml");
	remote_server_get_application_list(server);
	remote_server_destory(server);
	return 0;
}
