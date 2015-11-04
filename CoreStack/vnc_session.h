#ifndef _VNC_SESSION_H
#define _VNC_SESSION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern struct vnc_session *vnc_session_start(const char *ip, uint16_t port);

extern void vnc_session_stop(struct vnc_session *session);

extern uint8_t vnc_session_doworks(struct vnc_session *session);

#ifdef __cplusplus
}
#endif

#endif
