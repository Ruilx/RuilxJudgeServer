#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include <QtWidgets>
#include "core/dockerdaemon.h"
#include "core/network.h"
#include "core/dockerrest.h"

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
	QAction *startDockerRestAct;
	QAction *stopDockerRestAct;

	QAction *exitAct;

	QTabWidget *mainWidget;

	DockerDaemon *dockerDaemon;
	Network *network;
	DockerRest *dockerRest;

	void closeEvent(QCloseEvent *e);

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

	void dockerRestStartStateSlot(){
		this->startDockerRestAct->setEnabled(false);
		this->stopDockerRestAct->setEnabled(true);
	}

	void dockerRestStopStateSlot(){
		this->startDockerRestAct->setEnabled(true);
		this->stopDockerRestAct->setEnabled(false);
	}
};

#endif // MAINW_H
