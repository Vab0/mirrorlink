#include "vnc_session.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <arpa/inet.h>

#include "Platform/conn.h"
#include "Utils/buffer.h"

struct server_dispinfo {
	uint8_t maxv;
	uint8_t minv;
	uint16_t :10;
	uint16_t sfat:1;
	uint16_t :1;
	uint16_t sfds:1;
	uint16_t sfus:1;
	uint16_t sfr:1;
	uint16_t sfos:1;
	uint16_t rpwidth;
	uint16_t rpheight;
	uint32_t :6;
	uint32_t sgray:1;
	uint32_t dgray:1;
	uint32_t other16:1;
	uint32_t :3;
	uint32_t rgb343:1;
	uint32_t rgb444:1;
	uint32_t rgb555:1;
	uint32_t rgb565:1;
	uint32_t other24:1;
	uint32_t :6;
	uint32_t rgb888:1;
	uint32_t other32:1;
	uint32_t :6;
	uint32_t argb888:1;
} __attribute((packed));

struct server_evinfo {
	uint16_t kblc;
	uint16_t kbcc;
	uint16_t uilc;
	uint16_t uicc;
	uint32_t knob;
	uint32_t device;
	uint32_t multimedia;
	uint32_t :16;
	uint32_t fun_num:8;
	uint32_t evmap:1;
	uint32_t kel:1;
	uint32_t vkt:1;
	uint32_t keypad:1;
	uint32_t tepm:8;
	uint32_t tnse:8;
	uint32_t pebm:8;
	uint32_t :6;
	uint32_t tes:1;
	uint32_t pes:1;
} __attribute((packed));

struct context_info {
	uint32_t appid;
	uint16_t tlac;
	uint16_t tlcc;
	uint32_t ac;
	uint32_t cc;
	uint32_t cr;
} __attribute((packed));

struct vnc_session {
	int fd;
	uint16_t rfb_width;
	uint16_t rfb_height;
	struct server_dispinfo sdinfo;
	struct server_evinfo seinfo;
	uint8_t status;
	uint8_t quit;
};

static void fb_update_parse(struct vnc_session *session, uint16_t num);
static void server_cut_text_parse(struct vnc_session *session, uint32_t len);
static void ex_message_parse(struct vnc_session *session, uint8_t etype, uint16_t len);

void vnc_session_main_task(char *ip, uint16_t port)
{
	struct vnc_session session;
	session.status = 0;
	session.fd = conn_open(ip, port);
	/* Protocol Version Handshake */
	{
		uint8_t buf[12];
		conn_read(session.fd, buf, 12);
		if (0 != memcmp(buf, "RFB 003.008\n", 12)) {
			conn_close(session.fd);
			return;
		} else {
			conn_write(session.fd, buf, 12);
		}
	}
	printf("Protocol Version Handshake Finished.\n");
	/* Security Handshake */
	{
		uint8_t num = 0;
		uint8_t *buf = 0;
		conn_read(session.fd, &num, 1);
		if (0 == num) {
			uint32_t n;
			conn_read(session.fd, (uint8_t *)&n, 4);
			buf = (uint8_t *)malloc(n);
			conn_read(session.fd, buf, n);
			free(buf);
			conn_close(session.fd);
			return;
		} else {
			uint8_t i;
			uint8_t flag = 0;
			buf = (uint8_t *)malloc(num);
			conn_read(session.fd, buf, num);
			for (i = 0; i < num; i++) {
				if (1 == buf[i]) {
					flag = 1;
				}
			}
			free(buf);
			if (flag) {
				conn_write(session.fd, &flag, 1);
			} else {
				conn_close(session.fd);
				return;
			}
		}
	}
	printf("Security Handshake Finished.\n");
	/* Security Result Handshake */
	{
		uint32_t r;
		uint32_t rlen;
		uint8_t *buf = 0;
		conn_read(session.fd, (uint8_t *)&r, 4);
		if (1 == r) {
			conn_read(session.fd, (uint8_t *)&rlen, 4);
			buf = (uint8_t *)malloc(rlen);
			conn_read(session.fd, buf, rlen);
			free(buf);
			conn_close(session.fd);
			return;
		} else if (0 != r) {
			conn_close(session.fd);
			return;
		}
	}
	printf("Security Result Handshake Finished.\n");
	/* Initialization Messages */
	{
		uint8_t val = 0;
		struct buffer buf;
		uint8_t *info = 0;
		uint32_t len = 0;
		conn_write(session.fd, &val, 1);
		buffer_init(&buf, 24);
		conn_read(session.fd, buf.buf, 24);
		buf.len += 24;
		len = ntohl(*(uint32_t *)(buf.buf + 20));
		buffer_append(&buf, len);
		conn_read(session.fd, buf.buf + 24, len);
		/* only remote width and height is needed.
 		   pixel format is indicated in Server Configuration Message instead. */
		session.rfb_width = ((uint16_t)buf.buf[0] << 8) | buf.buf[1];
		session.rfb_height = ((uint16_t)buf.buf[2] << 8) | buf.buf[3];
		buffer_clear(&buf);
	}
	printf("Initialization Messages Finished.\n");
	/* Client to Server Messages */
	{
		uint8_t data[16] = {2, 0, 0, 3};
		data[4] = 0xff;
		data[5] = 0xff;
		data[6] = 0xfd;
		data[7] = 0xf5;
		data[8] = 0xff;
		data[9] = 0xff;
		data[10] = 0xfd;
		data[11] = 0xf4;
		data[12] = 0xff;
		data[13] = 0xff;
		data[14] = 0xff;
		data[15] = 0x21;
		/* Set Encoding: MirrorLink, ContextInfo, DesktopSize */
		conn_write(session.fd, data, 16);
	}
	session.quit = 0;
	printf("vnc handshake finished\n");
	while (!session.quit) {
		uint8_t msg_type = 0xff;
		conn_read(session.fd, &msg_type, 1);
		switch (msg_type) {
			case 0: /* Framebuffer Update */
				{
					uint8_t header[3] = {0};
					uint16_t num;
					conn_read(session.fd, header, 3);
					num = ((uint16_t)header[1] << 8) | header[2];
					fb_update_parse(&session, num);
				}
				break;
			case 1: /* Set Colour Map Entries */
				/* MUST NOT be used during a MirrorLink Session */
				{
					uint8_t header[6] = {0};
					uint16_t num;
					uint8_t *dummy;
					conn_read(session.fd, header, 6);
					num = ((uint16_t)header[4] << 8) | header[5];
					dummy = (uint8_t *)calloc(1, num);
					conn_read(session.fd, dummy, num * 6);
					/* read out and split it away */
					free(dummy);
				}
				break;
			case 2: /* Bell */
				break;
			case 3: /* Server Cut Text */
				{
					uint8_t header[7] = {0};
					uint32_t len;
					conn_read(session.fd, header, 7);
					len = ((uint32_t)header[3] << 24) | ((uint32_t)header[4] << 16) | ((uint32_t)header[5] << 8) | header[6];
					server_cut_text_parse(&session, len);
				}
				break;
			case 128: /* MirrorLink Extension Message */
				{
					uint8_t header[3] = {0};
					uint16_t len;
					conn_read(session.fd, header, 3);
					len = ((uint16_t)header[1] << 8) | header[2];
					ex_message_parse(&session, header[0], len);
				}
				break;
			default:
				break;
		}
	}
}

void ex_message_parse(struct vnc_session *session, uint8_t etype, uint16_t len)
{
	uint8_t *buf;
	buf = (uint8_t *)calloc(1, len);
	conn_read(session->fd, buf, len);
	
	switch (etype) {
		case 0: /* ByeBye */
			{
				uint8_t data[4] = {0x80, 0x00, 0x00, 0x00};
				/* response byebye message with byebye. */
				conn_write(session->fd, data, 4);
				conn_close(session->fd);
				session->quit = 1;
			}
			break;
		case 1: /* Server Display Configuration */
			{
				if (session->status) {
					break;
				} else {
					session->status = 1;
				}
				{
					uint8_t data[26] = {128, 2, 0, 22};
					memcpy(&(session->sdinfo), buf, len);
					data[4] = 1;
					data[5] = 1;
					data[6] = 0;
					data[7] = 0;
					data[8] = (uint16_t)800 >> 8;
					data[9] = 800 & 0xffU;
					data[10] = (uint16_t)480 >> 8;
					data[11] = 480 & 0xffU;
					data[12] = 0;
					data[13] = 0;
					data[14] = 0;
					data[15] = 0;
					data[16] = 0;
					data[17] = 0;
					data[18] = 0;
					data[19] = 1;
					data[20] = 0;
					data[21] = 0;
					data[22] = 0;
					data[23] = 0;
					data[24] = 0;
					data[25] = 1;
					conn_write(session->fd, data, 26);
					printf("client display configuration is sent\n");
				}
				{
					uint8_t data[20] = {0, 0, 0, 0};
					data[4] = 16;
					data[5] = 16;
					data[6] = 1;
					data[7] = 1;
					data[8] = 0;
					data[9] = 31;
					data[10] = 0;
					data[11] = 63;
					data[12] = 0;
					data[13] = 31;
					data[14] = 11;
					data[15] = 5;
					data[16] = 0;
					data[17] = 0;
					data[18] = 0;
					data[19] = 0;
					/* Set Pixel Format: ARGB 888, RGB 565 */
					printf("set pixel format is sent %d\n", conn_write(session->fd, data, 20));
				}
			}
			break;
		case 3: /* Server Event Configuration */
			{
				uint8_t data[32] = {128, 4, 0, 28};
				if (1 != session->status) {
					break;
				}
				memcpy(&(session->seinfo), buf, len);
				data[28] = 0;
				data[29] = 0;
				data[30] = 1;
				data[31] = 1;
				conn_write(session->fd, data, 32);
				session->status = 2;
				printf("client event configuration is sent\n");
				{
					uint8_t data[10];
					data[0] = 3;
					data[1] = 0;
					data[2] = 0;
					data[3] = 0;
					data[4] = 0;
					data[5] = 0;
					data[6] = session->rfb_width >> 8;
					data[7] = session->rfb_width & 0xffU;
					data[8] = session->rfb_height >> 8;
					data[9] = session->rfb_height & 0xffU;
					conn_write(session->fd, data, 10);
					printf("framebuffer request sent %d, %d\n", session->rfb_width, session->rfb_height);
				}
			}
			break;
		case 5: /* Event Mapping */
			{

			}
			break;
		case 7: /* Key Event Listing */
			{

			}
			break;
		case 9: /* Virtual Keyboard Trigger */
			{

			}
			break;
		case 11: /* Device Status */
			{

			}
			break;
		case 13: /* Content Attestation Response */
			{

			}
			break;
		case 21: /* Framebuffer Alternative Text */
			{

			}
			break;
		default: /* Unknown MirrorLink Extension Message */
			{

			}
			break;
	}
	free(buf);
}

void fb_update_parse(struct vnc_session *session, uint16_t num)
{
	uint16_t i;
	uint8_t fb_update = 0;
	uint8_t inc = 1;
	uint8_t buf[12];
	for (i = 0; i < num; i++) {
		uint16_t px;
		uint16_t py;
		uint16_t w;
		uint16_t h;
		int32_t etype;
		conn_read(session->fd, buf, 12);
		px = ((uint16_t)buf[0] << 8) | buf[1];
		py = ((uint16_t)buf[2] << 8) | buf[3];
		w = ((uint16_t)buf[4] << 8) | buf[5];
		h = ((uint16_t)buf[6] << 8) | buf[7];
		etype = ((uint32_t)buf[8] << 24) | ((uint32_t)buf[9] << 16) | ((uint32_t)buf[10] << 8) | buf[11];
		switch (etype) {
			case 0: /* Raw Encoding */
				{
					if (0 == w || 0 == h) {
						break;
					}
					if ((px + w > session->rfb_width) || (py + h > session->rfb_height)) {
						break;
					}
					{
						uint8_t *fb = (uint8_t *)malloc(w * h * 2);
						conn_read(session->fd, fb, w * h * 2);
						free(fb);
					}
					fb_update = 1;
					printf("rectangle (%d, %d, %d, %d) updated\n", px, py, w, h);
				}
				break;
			case -524: /* Context Information */
				{
					{
						struct context_info info;
						conn_read(session->fd, (char *)&info, 20);
					}
					printf("context information received\n");
				}
				break;
			case -223: /* Desktop Size */
				{
					session->rfb_width = w;
					session->rfb_height = h;
					inc = 0;
					fb_update = 1;
					printf("Desktop Size Encoding received\n");
				}
				break;
			case -525: /* Run Length Encoding */
				{

				}
				break;
			case -526: /* Transform Encoding */
				{

				}
				break;
			default:
				break;
		}
	}
	if (fb_update) {
		uint8_t data[10];
		data[0] = 3;
		data[1] = inc;
		data[2] = 0;
		data[3] = 0;
		data[4] = 0;
		data[5] = 0;
		data[6] = session->rfb_width >> 8;
		data[7] = session->rfb_width & 0xffU;
		data[8] = session->rfb_height >> 8;
		data[9] = session->rfb_height & 0xffU;
		conn_write(session->fd, data, 10);
		printf("framebuffer request sent %d, %d\n", session->rfb_width, session->rfb_height);
	}
}

void server_cut_text_parse(struct vnc_session *session, uint32_t len)
{
	uint8_t *buf;
	buf = (uint8_t *)malloc(len);
	conn_read(session->fd, buf, len);
	free(buf);
}

