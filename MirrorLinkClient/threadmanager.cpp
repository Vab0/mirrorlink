#include "threadmanager.h"

ThreadManager::ThreadManager(QObject *parent) :
	QObject(parent)
{
	connect(&m_thread, &QThread::started, this, &ThreadManager::onThreadStart);
	m_client.moveToThread(&m_thread);
	m_thread.start();
}

ThreadManager::~ThreadManager()
{
	m_thread.quit();
	m_thread.wait();
}

void ThreadManager::startMirrorLinkClient(QString ip, quint16 port, QString path)
{
	emit start(ip, port, path);
}

void ThreadManager::stopMirrorLinkClient()
{
	emit stop();
}

void ThreadManager::launchApp(quint32 appid)
{
	emit launch(appid);
}

void ThreadManager::onThreadStart()
{
	connect(this, &ThreadManager::start, &m_client, &MirrorLinkClient::onStart);
	connect(this, &ThreadManager::stop, &m_client, &MirrorLinkClient::onStop);
	connect(this, &ThreadManager::launch, &m_client, &MirrorLinkClient::onLaunch);
}

