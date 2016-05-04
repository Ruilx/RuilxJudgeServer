#ifndef NETWORK_H
#define NETWORK_H

#include <QWidget>
#include <QtWidgets>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QHostAddress>
#include "helper/handle.h"
#include "../utils/logsystem.h"

#define Log LogSystem

class Client{
public:
	QThread *clientThread;
	QString clientName;
	Handle *clientHandle;
};


class Network : public QWidget
{
	Q_OBJECT
	int port;
	QTcpServer *server;
	QTextEdit *outputEdit;

	QMap<int, Client*> clientMap;
public:
	explicit Network(int port, QWidget *parent = 0);
	~Network();

signals:
	void serverStarted();
	void serverStoped();
	void received(int, QJsonDocument);
public slots:
	void send(int tid, QJsonDocument doc);
	void startServer();
	void stopServer();

private slots:
	void newConnection();
	void join(QThread *thread);
	void receive(int tid, QJsonDocument doc){
		qDebug() << "[DEBUG][Network] receive id:" << tid << "doc:" << doc;
		emit this->received(tid, doc);
	}
};

#endif // NETWORK_H
