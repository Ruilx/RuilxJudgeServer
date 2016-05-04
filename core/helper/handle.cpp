#include "handle.h"

void Handle::start(){
	this->bytesAvailable = 0;
	this->timerId = startTimer(10);
	connect(this->socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(this->socket, SIGNAL(readyRead()), this, SLOT(read()));
	qDebug() << "[DEBUG][HANDLE]" << QThread::currentThread() << "New connection";
	emit this->information(QString("New connection: %1").arg((int)QThread::currentThreadId()));
}

void Handle::write(QJsonDocument doc){
	this->socket->write(doc.toBinaryData());
}

void Handle::timerEvent(QTimerEvent *e){
	if(e->timerId() == this->timerId){
		qint64 bytes = this->bytesAvailable;
		if(this->bytesAvailable != bytes){
			return;
		}
		killTimer(this->timerId);
		this->timerId = 0;
		if(bytesAvailable == 0){
			return;
		}
		bytesAvailable = 0;

		QByteArray data = socket->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(data);
		if(doc.isNull()){
			qDebug() << "[WARNI][HANDLE]" << QThread::currentThread() << "Invalid JSON received!" << data;
			return;
		}
		qDebug() << "[DEBUG][HANDLE]" << QThread::currentThread() << "JSON received:" << doc;
		emit this->received((quint64)QThread::currentThreadId(), doc);
	}
}

void Handle::disconnected(){
	qDebug() << "[DEBUG][HANDLE]" << QThread::currentThread() << "Disconnected";
	emit this->information(QString("Disconnect: %1").arg((int)QThread::currentThreadId()));
	emit this->finished(QThread::currentThread());
}

void Handle::read(){
	if(!this->timerId){
		this->timerId = startTimer(10);
	}
}
