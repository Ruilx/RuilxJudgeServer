#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include <QtWidgets>
#include "core/dockerdaemon.h"
#include "core/network.h"

class MainW : public QMainWindow
{
	Q_OBJECT
	QMenu *systemMenu;
	QMenu *questionMenu;
	QMenu *helpMenu;

	QAction *startServerAct;
	QAction *stopServerAct;
	QAction *startDockerDaemonAct;
	QAction *stopDockerDaemonAct;
	QAction *exitAct;

	QTabWidget *mainWidget;

	DockerDaemon *dockerDaemon;
	Network *network;
public:
	MainW(QWidget *parent = 0);
	~MainW();
private slots:
	void dockerDaemonOpenStateSlot(){
		this->startDockerDaemonAct->setEnabled(false);
		this->stopDockerDaemonAct->setEnabled(true);
	}

	void dockerDaemonCloseStateSlot(){
		this->startDockerDaemonAct->setEnabled(true);
		this->stopDockerDaemonAct->setEnabled(false);
	}

	void serverOpenStateSlot(){
		this->startServerAct->setEnabled(false);
		this->stopServerAct->setEnabled(true);
	}

	void serverCloseStateSlot(){
		this->startServerAct->setEnabled(true);
		this->stopServerAct->setEnabled(false);
	}
};

#endif // MAINW_H
