#ifndef _VNC_SESSION_H
#define _VNC_SESSION_H

#include <stdint.h>

struct vnc_session {
	char *ip;
	uint16_t port;
};

extern void vnc_session_main_task(void *args);

#endif
