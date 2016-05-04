#include "dockerrest.h"

DockerRest::DockerRest(QString socketPath = QString(), QWidget *parent) : QWidget(parent){
	this->socketPath = socketPath;
	this->dockerSocket = new QLocalSocket(this);
	this->outputEdit = new QTextEdit(this);
	this->outputEdit->setReadOnly(true);
	this->outputEdit->setWordWrapMode(QTextOption::NoWrap);
	this->outputEdit->setFont(QFont("RuilxFixedSys, courier new, Wenquanyi Zen Hei", 12));

	QHBoxLayout *lay = new QHBoxLayout;
	this->setLayout(lay);
	lay->addWidget(this->outputEdit);

	this->outputEdit->insertPlainText("This window shows docker socket REST information.\n");

	connect(this->dockerSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(error(QLocalSocket::LocalSocketError)));
	connect(this->dockerSocket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)), this, SLOT(stateChanged(QLocalSocket::LocalSocketState)));
	connect(this->dockerSocket, SIGNAL(readyRead()), this, SLOT(receive()));
	connect(this->dockerSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(this->dockerSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}


