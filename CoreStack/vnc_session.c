#include "vnc_session.h"

#include <string.h>
#include <stdlib.h>

#include "Platform/conn.h"

static void fb_update_parse(uint8_t *buf, uint16_t num);
static void server_cut_text_parse(uint8_t *buf, uint32_t len);
static void ex_message_parse(uint8_t etype, uint8_t *buf, uint16_t len);

void vnc_session_main_task(void *args)
{
	struct vnc_session *session = (struct vnc_session *)args;
	int fd = conn_open(session->ip, session->port);
	/* Protocol Version Handshake */
	{
		uint8_t buf[12];
		conn_read(fd, buf, 12);
		if (0 != memcmp(buf, "RFB 003.008\n", 12)) {
			conn_close(fd);
			return;
		} else {
			conn_write(fd, buf, 12);
		}
	}
	/* Security Handshake */
	{
		uint8_t num = 0;
		uint8_t *buf = 0;
		conn_read(fd, &num, 1);
		if (0 == num) {
			uint32_t n;
			conn_read(fd, (uint8_t *)&n, 4);
			buf = (uint8_t *)malloc(n);
			conn_read(fd, buf, n);
			free(buf);
			conn_close(fd);
			return;
		} else {
			uint8_t i;
			uint8_t flag;
			buf = (uint8_t *)malloc(num);
			conn_read(fd, buf, num);
			for (i = 0; i < num; i++) {
				if (0 == buf[i]) {
					break;
				} else if (1 == buf[i]) {
					flag = 1;
				}
			}
			free(buf);
			if (i != num) {
				if (flag) {
					conn_write(fd, &flag, 1);
				} else {
					conn_close(fd);
					return;
				}
			} else {
				conn_close(fd);
				return;
			}
		}
	}
	/* Security Result Handshake */
	{
		uint32_t r;
		uint32_t rlen;
		uint8_t *buf = 0;
		conn_read(fd, (uint8_t *)&r, 4);
		if (1 == r) {
			conn_read(fd, (uint8_t *)&rlen, 4);
			buf = (uint8_t *)malloc(rlen);
			conn_read(fd, buf, rlen);
			free(buf);
			conn_close(fd);
			return;
		} else if (0 != r) {
			free(buf);
			conn_close(fd);
			return;
		}
	}
	/* Initialization Messages */
	{
		uint8_t val = 0;
		uint8_t *buf = 0;
		uint8_t *tmp = 0;
		uint32_t len = 0;
		conn_write(fd, &val, 1);
		buf = (uint8_t *)malloc(24);
		conn_read(fd, buf, 24);
		len = ntohl(*(uint32_t *)(buf + 20));
		tmp = (uint8_t *)realloc(buf, 24 + len);
		if (tmp) {
			buf = tmp;
			conn_read(fd, buf + 24, len);
		} else {
			free(buf);
			conn_close(fd);
			return;
		}
		/* parse server infomation */
		free(buf);
	}
	/* Client to Server Messages */
	{
		uint8_t data[20];
		/* Set Encoding: MirrorLink, ContextInfo, DesktopSize */
		conn_write(fd, data, 16);
		/* Set Pixel Format: ARGB 888, RGB 565 */
		/* conn_write(fd, data, 20); */
	}
	while (1) {
		uint8_t msg_type;
		conn_read(fd, &msg_type, 1);
		switch (msg_type) {
			case 0:
			{
				uint8_t header[3];
				uint8_t *buf;
				uint16_t num;
				conn_read(fd, header, 3);
				num = (uint16_t)header[1] << 8 | header[2];
				buf = (uint8_t *)malloc(12 * num);
				conn_read(fd, buf, 12 * num);
				fb_update_parse(buf, num);
				free(buf);
			}
			break;
			case 1: /* Set Colour Map Entries */
				/* MUST NOT be used during a MirrorLink Session */
			break;
			case 2: /* Bell */
			break;
			case 3: /* Server Cut Text */
			{
				uint8_t header[7];
				uint8_t *buf;
				uint32_t len;
				conn_read(fd, header, 7);
				len = ((uint32_t)header[3] << 24) | ((uint32_t)header[4] << 16) | ((uint32_t)header[5] << 8) | header[6];
				buf = (uint8_t *)malloc(len);
				conn_read(fd, buf, len);
				server_cut_text_parse(buf, len);
				free(buf);
			}
			break;
			case 128:
			{
				uint8_t header[3];
				uint8_t *buf;
				uint16_t len;
				conn_read(fd, header, 3);
				len = ((uint16_t)header[1] << 8) | header[2];
				buf = (uint8_t *)malloc(len);
				conn_read(fd, buf, len);
				ex_message_parse(header[0], buf, len);
				free(buf);
			}
			break;
			default:
			break;
		}
	}
}

void ex_message_parse(uint8_t etype, uint8_t *buf, uint16_t len)
{
	switch (etype) {
		case 0: /* ByeBye */
			{

			}
			break;
		case 1: /* Server Display Configuration */
			{

			}
			break;
		case 2: /* Client Display COnfiguration */
			{

			}
			break;
		case 3: /* Server Event Configuration */
			{

			}
			break;
		case 4: /* Client Event Configuration */
			{

			}
			break;
		case 5: /* Event Mapping */
			{

			}
			break;
		case 6: /* Event Mapping Request */
			{

			}
			break;
		case 7: /* Key Event Listing */
			{

			}
			break;
		case 8: /* Key Event Listing Request */
			{

			}
			break;
		case 9: /* Virtual Keyboard Trigger */
			{

			}
			break;
		case 10: /* Virtual Keyboard Trigger Request */
			{

			}
			break;
		case 11: /* Device Status */
			{

			}
			break;
		case 12: /* Device Status Request */
			{

			}
			break;
		case 13: /* Content Attestation Response */
			{

			}
			break;
		case 14: /* Content Attestation Request */
			{

			}
			break;
		case 16: /* Framebuffer Blocking Notification */
			{

			}
			break;
		case 18: /* Audio Blocking Notification */
			{

			}
			break;
		case 20: /* Touch Event */
			{

			}
			break;
		case 21: /* Framebuffer Alternative Text */
			{

			}
			break;
		case 22: /* Framebuffer Alternative Text Request */
			{

			}
			break;
		default: /* Unknown MirrorLink Extension Message */
			{

			}
			break;
	}
}

void fb_update_parse(uint8_t *buf, uint16_t num)
{

}

void server_cut_text_parse(uint8_t *buf, uint32_t len)
{

}



















