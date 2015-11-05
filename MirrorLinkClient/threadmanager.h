#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QThread>

#include "mirrorlinkclient.h"

class ThreadManager : public QObject
{
	Q_OBJECT
public:
	explicit ThreadManager(QObject *parent = 0);
	~ThreadManager();

signals:
	void start(QString ip, quint16 port, QString path);
	void stop();
	void launch(quint32 appid);

public slots:
	void startMirrorLinkClient(QString ip, quint16 port, QString path);
	void stopMirrorLinkClient();
	void launchApp(quint32 appid);

private slots:
	void onThreadStart();

private:
	QThread m_thread;
	MirrorLinkClient m_client;
};

#endif // THREADMANAGER_H
