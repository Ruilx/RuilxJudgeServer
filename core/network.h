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

	QMap<quint64, Client*> clientMap;
public:
	explicit Network(int port, QWidget *parent = 0);
	~Network();
	bool isRunning(){
		return this->server->isListening();
	}

signals:
	void serverStarted();
	void serverStoped();
	void received(quint64, QJsonDocument);
public slots:
	void send(quint64 tid, QJsonDocument doc);
	void startServer();
	void stopServer();

private slots:
	void newConnection();
	void join(QThread *thread);
	void receive(quint64 tid, QJsonDocument doc){
		qDebug() << "[DEBUG][Network] receive id:" << tid << "doc:" << doc;
		emit this->received(tid, doc);
	}
	void showInformation(QString str){
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Handle", str));
	}
};

#endif // NETWORK_H
