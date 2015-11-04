#include "vncsession.h"

#include "../CoreStack/vnc_session.h"

VNCSession::VNCSession(QObject *parent) :
	QObject(parent)
{
	m_session = 0;
}

VNCSession::~VNCSession()
{
	vnc_session_stop(m_session);
}

void VNCSession::onStart(QString ip, quint16 port)
{
	m_session = vnc_session_start(ip.toStdString().c_str(), port);
	if (m_session) {
		connect(this, &VNCSession::doworks, this, &VNCSession::onDoworks);
		emit doworks();
	}
}

void VNCSession::onDoworks()
{
	if (m_session) {
		if (1 == vnc_session_doworks(m_session)) {
			emit doworks();
		} else {
			vnc_session_stop(m_session);
			m_session = 0;
		}
	}
}
