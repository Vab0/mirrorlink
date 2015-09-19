#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "libxml/parser.h"
#include "libxml/tree.h"

#include "Platform/http_client.h"
#include "Utils/str.h"

enum {
	SERVICE_TYPE_APP,
	SERVICE_TYPE_CLP,
	SERVICE_TYPE_NOT,
	SERVICE_TYPE_MAX
};

struct service_info {
	str_t surl;
	str_t curl;
	str_t eurl;
};

struct remote_server {
	str_t ip;
	str_t uuid;
	uint16_t port;
	struct service_info sinfo[SERVICE_TYPE_MAX];
};

struct remote_server *remote_server_create(char *ip, uint16_t port, char *path)
{
	struct http_req *rq = http_client_make_req("GET", path);
	struct http_rsp *rp = http_client_send(ip, port, rq);
	struct remote_server *server = 0;
	if (rp && (200 == http_client_get_errcode(rp))) {
		xmlDocPtr doc = xmlParseDoc(http_client_get_body(rp));
		xmlNodePtr root = xmlDocGetRootElement(doc);
		xmlNodePtr c1 = xmlFirstElementChild(root);
		printf("get description successfully\n");
		for (; c1 != 0; c1 = xmlNextElementSibling(c1)) {
			if (xmlStrEqual(c1->name, xmlCharStrdup("device"))) {
				xmlNodePtr c2 = xmlFirstElementChild(c1);
				for (; c2 != 0; c2 = xmlNextElementSibling(c2)) {
					if (xmlStrEqual(c2->name, xmlCharStrdup("deviceType"))) {
						printf("this is a mirrorlink server device.\n");
						server = (struct remote_server *)calloc(1, sizeof(*server));
						str_append(&(server->ip), ip);
						server->port = port;
					} else if (xmlStrEqual(c2->name, xmlCharStrdup("UDN"))) {
						str_append(&(server->uuid), xmlNodeGetContent(c2));
						printf("uuid = %s\n", server->uuid);
					} else if (xmlStrEqual(c2->name, xmlCharStrdup("serviceList"))) {
						xmlNodePtr c3 = xmlFirstElementChild(c2);
						for (; c3 != 0; c3 = xmlNextElementSibling(c3)) {
							if (xmlStrEqual(c3->name, xmlCharStrdup("service"))) {
								xmlNodePtr c4 = xmlFirstElementChild(c3);
								uint8_t type = SERVICE_TYPE_MAX;
								for (; c4 != 0; c4 = xmlNextElementSibling(c4)) {
									if (xmlStrEqual(c4->name, xmlCharStrdup("serviceType"))) {
										printf("service type is %s\n", xmlNodeGetContent(c4));
										xmlChar *stype = xmlNodeGetContent(c4);
										if (xmlStrEqual(stype,
											xmlCharStrdup("urn:schemas-upnp-org:service:TmApplicationServer:1"))) {
											type = SERVICE_TYPE_APP;
										} else if (xmlStrEqual(stype,
											xmlCharStrdup("urn:schemas-upnp-org:service:TmClientProfile:1"))) {
											type = SERVICE_TYPE_CLP;
										} else if (xmlStrEqual(stype,
											xmlCharStrdup("urn:schemas-upnp-org:service:TmNotificationServer:1"))) {
											type = SERVICE_TYPE_NOT;
										}
									} else if (xmlStrEqual(c4->name, xmlCharStrdup("SCPDURL"))) {
										str_append(&(server->sinfo[type].surl), xmlNodeGetContent(c4));
										printf("subscribe url is %s\n", server->sinfo[type].surl);
									} else if (xmlStrEqual(c4->name, xmlCharStrdup("controlURL"))) {
										str_append(&(server->sinfo[type].curl), xmlNodeGetContent(c4));
										printf("control url is %s\n", server->sinfo[type].curl);
									} else if (xmlStrEqual(c4->name, xmlCharStrdup("eventSubURL"))) {
										str_append(&(server->sinfo[type].eurl), xmlNodeGetContent(c4));
										printf("event url is %s\n", server->sinfo[type].eurl);
									}
								}
							}
						}
					}
				}
			}
		}
		xmlFreeDoc(doc);
	}
	http_client_free_rsp(rp);
	return server;
}

int remote_server_get_application_list(struct remote_server *server)
{
	struct http_req *rq = http_client_make_req("POST", server->sinfo[SERVICE_TYPE_APP].curl);
	struct http_rsp *rp = 0;
	char buf[100];
	sprintf(buf, "HOST: %s:%d\r\n", server->ip, server->port);
	http_client_add_header(rq, buf);
	http_client_add_header(rq, "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n");
	http_client_add_header(rq, "SOAPACTION: \"urn:schemas-upnp-org:service:TmApplicationServer:1#GetApplicationList\"\r\n");
	http_client_set_body(rq, "<?xml version=\"1.0\"?>"
			"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
			"<s:Body>"
			"<u:GetApplicationList xmlns:u=\"urn:schemas-upnp-org:service:TmApplicationServer:1\">"
			"<AppListingFilter>*</AppListingFilter>"
			"<ProfileID>0</ProfileID>"
			"</u:GetApplicationList>"
			"</s:Body>"
			"</s:Envelope>");
	rp = http_client_send(server->ip, server->port, rq);
	if (rp && (200 == http_client_get_errcode(rp))) {
		printf("get application list successfully.\n");
	}
	http_client_free_rsp(rp);
	return 0;
}

void remote_server_destory(struct remote_server *server)
{
	uint8_t i;
	free(server->ip);
	free(server->uuid);
	for (i = 0; i < SERVICE_TYPE_MAX; i++) {
		free(server->sinfo[i].surl);
		free(server->sinfo[i].curl);
		free(server->sinfo[i].eurl);
	}
	free(server);
}
