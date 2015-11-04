#ifndef MIRRORLINKCLIENT_H
#define MIRRORLINKCLIENT_H

#include <QObject>

#include <stdint.h>

class MirrorLinkClient : public QObject
{
	Q_OBJECT
public:
	explicit MirrorLinkClient(QObject *parent = 0);
	~MirrorLinkClient();

signals:

public slots:
	void start(QString ip, qint16 port, QString path);
	void stop();
	void launch(uint32_t appid);

private:
	struct remote_server *m_server;
};

#endif // MIRRORLINKCLIENT_H
