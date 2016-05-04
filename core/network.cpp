#include "network.h"

Network::Network(int port, QWidget *parent) : QWidget(parent){
	this->server = new QTcpServer(this);
	this->outputEdit = new QTextEdit(this);
	this->port = port;

	this->outputEdit->setFont(QFont("RuilxFixedSys, courier new, Wenquanyi Zen Hei", 12));
	this->outputEdit->setReadOnly(true);
	this->outputEdit->setWordWrapMode(QTextOption::NoWrap);

	QHBoxLayout *lay = new QHBoxLayout;
	this->setLayout(lay);
	lay->addWidget(this->outputEdit);

	this->outputEdit->insertPlainText("Please use system -> start server to start network daemon.\n");
}

Network::~Network(){
	foreach (auto p, this->clientMap) {
		p->clientThread->quit();
		p->clientThread->wait();
	}
}

void Network::send(quint64 tid, QJsonDocument doc){
	Client *client = clientMap.value(tid, new Client);
	if(client->clientThread != 0){
		client->clientHandle->write(doc);
	}else{
		qDebug() << "[ERROR][Network] receive signal with no tid.";
	}
}

void Network::startServer(){
	if(!this->server->isListening()){
		if(!this->server->listen(QHostAddress::Any, this->port)){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Network", QString("Cannot listen on port %1").arg(this->port)));
			return;
		}
		connect(this->server, SIGNAL(newConnection()), this, SLOT(newConnection()));
		qDebug() << "[DEBUG][Network] Server started.";
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Network", QString("Server started at host, port: %1").arg(this->port)));
		emit this->serverStarted();
	}
}

void Network::stopServer(){
	if(this->server->isListening()){
		qDebug() << "[DEBUG][Network]Server stopped.";
		this->server->close();
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Network", "Server closed."));
		emit this->serverStoped();
	}
}

void Network::newConnection(){
	QThread *handleThread = new QThread(this);
	Client *client = new Client;
	client->clientThread = handleThread;
	QTcpSocket *socket = server->nextPendingConnection();
	connect(handleThread, SIGNAL(finished()), socket, SLOT(deleteLater()));
	Handle *handle = new Handle(socket);
	connect(handleThread, SIGNAL(started()), handle, SLOT(start()));
	connect(handleThread, SIGNAL(finished()), handle, SLOT(deleteLater()));
	connect(handle, SIGNAL(finished(QThread*)), this, SLOT(join(QThread*)));

	handle->moveToThread(handleThread);
	handleThread->start();
	client->clientHandle = handle;
	client->clientThread = handleThread;
	this->clientMap.insert((quint64)handleThread->currentThreadId(), client);

	connect(handle, SIGNAL(received(quint64,QJsonDocument)), this, SLOT(receive(quin64,QJsonDocument)));
}

void Network::join(QThread *thread){
	thread->quit();
	thread->wait();
	this->clientMap.take((quint64)thread->currentThreadId());
	qDebug() << "[INFOR][HANDLE]" << thread << "deleted.";
	delete thread;
}
