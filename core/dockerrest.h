#ifndef DOCKERREST_H
#define DOCKERREST_H

#include <QWidget>
#include <QtWidgets>
#include <QLocalSocket>
#include <QMessageBox>
#include "../utils/logsystem.h"

#define Log LogSystem

class DockerRest : public QWidget
{
	Q_OBJECT
	QLocalSocket *dockerSocket;
	QTextEdit *outputEdit;
	QString socketPath;

	void closeEvent(QCloseEvent *){
		if(this->dockerSocket->state() != QLocalSocket::UnconnectedState){
			this->closeSocket();
		}
	}

public:
	explicit DockerRest(QString socketPath, QWidget *parent = 0);
	bool isRunning(){
		return this->dockerSocket->state() != QLocalSocket::UnconnectedState;
	}

signals:
	void socketOpened();
	void socketClosed();
	void received(QJsonDocument);
public slots:
	void send(QString cmd){
		//GET /images/json HTTP/1.1
		QString cmdStr = QString("GET ").append(cmd).append(" HTTP/1.1\n\n");
		this->dockerSocket->write(cmdStr.toUtf8());
		if(!this->dockerSocket->waitForBytesWritten(3000)){
			qDebug() << "[ERROR][DockerRest]send: write failed:" << this->dockerSocket->errorString();
		}
	}

	void openSocket(){
		if(this->dockerSocket->state() == QLocalSocket::UnconnectedState){
			this->dockerSocket->connectToServer(this->socketPath);
			QMessageBox box(QMessageBox::Information, "Ruilx Judge Server", "Connect docker REST... Please wait...");
			box.show();
			if(this->dockerSocket->waitForConnected(5000)){
				box.hide();
				this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "REST", "Connected."));
			}else{
				box.hide();
				this->outputEdit->insertPlainText(Log::getLogString(Log::Error, "REST", QString("An error occurred while connecting to %1").arg(this->socketPath)));
				this->outputEdit->insertPlainText(Log::getLogString(Log::Error, "REST", this->dockerSocket->errorString().append(". Are you opened docker daemon?")));
				return;
			}
		}else{
			this->outputEdit->insertPlainText(Log::getLogString(Log::Warning, "REST", "Socket already open!"));
		}
	}

	void closeSocket(){
		if(this->dockerSocket->state() != QLocalSocket::UnconnectedState){
			this->dockerSocket->disconnectFromServer();
//			QMessageBox box(QMessageBox::Information, "Ruilx Judge Server", "Disconnect from docker REST... Please wait...");
//			box.show();
//			if(this->dockerSocket->waitForDisconnected(5000)){
				//box.hide();
				this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "REST", "Disconnected."));
//			}else{
				//box.hide();
//				this->outputEdit->insertPlainText(Log::getLogString(Log::Error, "REST", QString("An error occurred while disconnect from %1").arg(this->socketPath)));
//				this->outputEdit->insertPlainText(Log::getLogString(Log::Error, "REST", this->dockerSocket->errorString()));
//				return;
//			}
		}else{
			this->outputEdit->insertPlainText(Log::getLogString(Log::Warning, "REST", "Socket already closed!"));
		}
	}

private slots:
	void connected(){
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "REST", "Socket returns connected."));
		emit this->socketOpened();
	}

	void disconnected(){
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "REST", "Socket return disconnected."));
		emit this->socketClosed();
	}

	void receive(){
		qint64 size = 0;
		do{
			size = this->dockerSocket->bytesAvailable();
			QThread::currentThread()->msleep(10);
		}while(size != this->dockerSocket->bytesAvailable());
		QByteArray data = this->dockerSocket->readAll();
		QList<QByteArray> list = data.split('\n');
		QJsonDocument doc;
		QJsonParseError error;
		bool isLoadJson = false;
		foreach(auto p, list){
			if(p.startsWith('[') && isLoadJson){
				doc = QJsonDocument::fromJson(p, &error);
				this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "REST", p));
				if(error.error == QJsonParseError::NoError){
					isLoadJson = true;
				}else{
					this->outputEdit->insertPlainText(Log::getLogString(Log::Error, "Json Error", p));
				}
			}else{
				qDebug() << "Dirty Data:" << p;
				this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "REST", p));
			}
		}
		if(doc.isNull()){
			//Read None?
			return;
		}
		emit this->received(doc);
	}

	void error(QLocalSocket::LocalSocketError socketError){
		switch(socketError){
			case QLocalSocket::ConnectionRefusedError:
				/* The connection was refused by the peer (or timed out). */
				qDebug() << "[ERROR][socketError]" << "The connection was refused by the peer (or timed out).";
				break;
			case QLocalSocket::PeerClosedError:
				/* The remote socket closed the connection.
				 * Note that the client socket (i.e., this socket)
				 * will be closed after the remote close notification
				 * has been sent. */
				qDebug() << "[ERROR][SockerError]" << "The remote socket closed the connection.";
				break;
			case QLocalSocket::ServerNotFoundError:
				/* The local socket name was not found. */
				qDebug() << "[ERROR][SocketError]" << "The local socket name was not found.";
				break;
			case QLocalSocket::SocketAccessError:
				/* The socket operation failed
				 * because the application lacked the required privileges. */
				qDebug() << "[ERROR][SocketError]" << "The socket operation failed.";
				break;
			case QLocalSocket::SocketResourceError:
				/* The local system ran out of resources
				 * (e.g., too many sockets). */
				qDebug() << "[ERROR][SockerError]" << "The local system ran out of resources.";
				break;
			case QLocalSocket::SocketTimeoutError:
				/* The socket operation timed out. */
				qDebug() << "[ERROR][SocketError]" << "The socket operation timed out.";
				break;
			case QLocalSocket::DatagramTooLargeError:
				/* The datagram was larger than the operating system's limit
				 * (which can be as low as 8192 bytes). */
				qDebug() << "[ERROR][SocketError]" << "The datagram was larger than the operating system's limit.";
				break;
			case QLocalSocket::ConnectionError:
				/* An error occurred with the connection. */
				qDebug() << "[ERROR][SocketError]" << "An error occurred with the connection.";
				break;
			case QLocalSocket::UnsupportedSocketOperationError:
				/* The requested socket operation is
				 * not supported by the local operating system. */
				qDebug() << "[ERROR][SocketError]" << "The requested socket operation is not supported.";
				break;
			case QLocalSocket::OperationError:
				/* An operation was attempted while the socket was in a state
				 * that did not permit it. */
				qDebug() << "[ERROR][SocketError]" << "An operation was attempted while the socket was in a state that did not permit it.";
				break;
			case QLocalSocket::UnknownSocketError:
				/* An unidentified error occurred. */
				qDebug() << "[ERROR][SocketError]" << "Unknown error occurred.";
				break;
		}
	}

	void stateChanged(QLocalSocket::LocalSocketState socketState){
		switch(socketState){
			case QLocalSocket::UnconnectedState:
				/* The socket is not connected. */
				break;
			case QLocalSocket::ConnectingState:
				/* The socket has started establishing a connection. */
				break;
			case QLocalSocket::ConnectedState:
				/* A connection is established. */
				break;
			case QLocalSocket::ClosingState:
				/* The socket is about to close
				 * (data may still be waiting to be written).*/
				break;
		}
	}
};

#endif // DOCKERREST_H
