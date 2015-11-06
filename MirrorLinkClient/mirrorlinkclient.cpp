#include "mirrorlinkclient.h"

#include "../CoreStack/remote_server.h"

MirrorLinkClient::MirrorLinkClient(QObject *parent) :
	QObject(parent)
{
	m_server = 0;
}

MirrorLinkClient::~MirrorLinkClient()
{
	remote_server_destory(m_server);
}

void MirrorLinkClient::onStart(QString ip, quint16 port, QString path)
{
	if (!m_server) {
		m_server = remote_server_create(ip.toStdString().c_str(), port, path.toStdString().c_str());
		/* remote_server_set_client_profile(m_server, 0); */
		remote_server_get_application_list(m_server, 0, "*");
	}
}

void MirrorLinkClient::onStop()
{
	remote_server_destory(m_server);
	m_server = 0;
}

void MirrorLinkClient::onLaunch(quint32 appid)
{
	if (m_server) {
		if (!remote_server_launch_application(m_server, appid, 0)) {
			process_app_launched(m_server, appid);
		}
	}
}

void MirrorLinkClient::process_app_launched(remote_server *server, quint32 appid)
{

}
