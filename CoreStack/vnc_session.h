#ifndef _VNC_SESSION_H
#define _VNC_SESSION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct context_info {
	uint32_t appid;
	uint16_t tlac;
	uint16_t tlcc;
	uint32_t ac;
	uint32_t cc;
	uint32_t cr;
} __attribute((packed));

struct pointer_event {
	uint8_t mask;
	uint16_t x;
	uint16_t y;
} __attribute((packed));

struct key_event {
	uint8_t flag;
	uint16_t padding;
	uint32_t key;
} __attribute((packed));

struct vnc_session_cb {
	void (*on_device_status)(int fd, uint32_t status);
	uint8_t *(*on_desktop_size_changed)(int fd, uint16_t w, uint16_t h);
	void (*on_framebuffer_update)(int fd, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	void (*on_context_infomation)(int fd, struct context_info *info);
};

extern void vnc_session_task(int fd, struct vnc_session_cb *cb);

extern void vnc_session_send_pointer_event(int fd, struct pointer_event pointer);

extern void vnc_session_send_key_event(int fd, struct key_event key);

extern void vnc_session_send_device_status(int fd, uint32_t status);

extern void vnc_session_send_byebye(int fd);

#ifdef __cplusplus
}
#endif

#endif
