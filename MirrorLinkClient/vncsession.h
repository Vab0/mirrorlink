#ifndef VNCSESSION_H
#define VNCSESSION_H

#include <QObject>

class VNCSession : public QObject
{
	Q_OBJECT
public:
	explicit VNCSession(QObject *parent = 0);
	~VNCSession();

signals:
	void doworks();

public slots:
	void onStart(QString ip, quint16 port);
	void onDoworks();

private:
	struct vnc_session *m_session;

};

#endif // VNCSESSION_H
