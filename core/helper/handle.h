#ifndef HANDLE_H
#define HANDLE_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QtCore>

class Handle : public QObject{
	Q_OBJECT
	int timerId;
	qint64 bytesAvailable;
	QTcpSocket *socket;
public:
	explicit Handle(QTcpSocket *s, QObject *parent = 0) : QObject(parent), socket(s) {}
public slots:
	void start();
	void write(QJsonDocument doc);
protected:
	void timerEvent(QTimerEvent *e);

signals:
	void received(quint64 threadId, QJsonDocument doc);
	void finished(QThread *);
	void information(QString);
private slots:
	void disconnected();
	void read();
};

#endif // HANDLE_H
