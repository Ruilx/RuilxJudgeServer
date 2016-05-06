#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include <QtWidgets>
#include "core/dockerdaemon.h"
#include "core/network.h"
#include "core/dockerrest.h"
#include "core/compileoutput.h"
#include "core/dockerrun.h"

class MainW : public QMainWindow
{
	Q_OBJECT
	QMenu *systemMenu;
	QMenu *questionMenu;
	QMenu *helpMenu;
	//QMenu *dockerRestTest;
	QMenu *dockerCmdMenu;

	QAction *startServerAct;
	QAction *stopServerAct;
	QAction *startDockerDaemonAct;
	QAction *stopDockerDaemonAct;
	QAction *startDockerRestAct;
	QAction *stopDockerRestAct;
	//QAction *writeRestMessageAct;

	//QList<QAction*> questionListActGroup;
	QAction *problemHelloWorldAct;
	QAction *problemAPlusBAct;

	QAction *exitAct;

	QTabWidget *mainWidget;

	DockerDaemon *dockerDaemon;
	Network *network;
	DockerRest *dockerRest;
	CompileOutput *complieOutput;
	DockerRun *dockerRun;
	QMap<QWidget*, int> tabObjectList;

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

	void activeDockerDaemonTabSlot(){
		this->mainWidget->tabBar()->setCurrentIndex(this->tabObjectList.value(this->dockerDaemon));
	}

	void activeServerTabSlot(){
		this->mainWidget->tabBar()->setCurrentIndex(this->tabObjectList.value(this->network));
	}

	void activeRestTabSlot(){
		this->mainWidget->tabBar()->setCurrentIndex(this->tabObjectList.value(this->dockerRest));
	}

	void activeCompileTabSlot(){
		this->mainWidget->tabBar()->setCurrentIndex(this->tabObjectList.value(this->complieOutput));
	}

	void activeDockerRunTabSlot(){
		this->mainWidget->tabBar()->setCurrentIndex(this->tabObjectList.value(this->dockerRun));
	}

	void problemHelloworldSlot(){
		QString filename = QFileDialog::getOpenFileName(this, "Open source file", QDir::currentPath(), "C File(*.c);;C++ File(*.cpp);;Java File(*.java)");
		if(filename.isEmpty()){
			return;
		}

		this->complieOutput->setSourceCodePath(filename);
		QFileInfo fInfo(filename);
//		switch(fInfo.suffix()){
//			case QString("c"):
//				this->complieOutput->setLanguageType(CompileOutput::C);
//				break;
//			case QString("cpp"):
//				this->complieOutput->setLanguageType(CompileOutput::Cpp);
//				break;
//			case QString("java"):
//				this->complieOutput->setLanguageType(CompileOutput::Java);
//				break;
//		}
		QString suffix = fInfo.suffix();
		if(suffix == "c" || suffix == "C"){
			this->complieOutput->setLanguageType(CompileOutput::C);
		}else if(QString::compare(suffix, "cpp", Qt::CaseInsensitive)){
			this->complieOutput->setLanguageType(CompileOutput::Cpp);
		}else if(QString::compare(suffix, "java", Qt::CaseInsensitive)){
			this->complieOutput->setLanguageType(CompileOutput::Java);
		}

		this->complieOutput->startCompile();
		if(this->complieOutput->getExitCode() != 0){
			if(this->complieOutput->getHasWarning() == 0){
				qDebug() << "[DEBUG][MainW]: Compile Failed?";
				return;
			}
		}

		//RunCode:
		qDebug() << "Ready Run Code";

		if(!this->dockerDaemon->isRunning()){
			qDebug() << "Docker Daemon is not running";
			return;
		}
		if(!this->network->isRunning()){
			qDebug() << "Network is not running.";
			return;
		}

		//execute docker cmd
		qDebug() << "Read exec docker";
		if(!this->dockerDaemon->isRunning()){
			qDebug() << "Docker Daemon is not run.";
			return;
		}
		qint64 ContainerName = QDateTime::currentDateTime().toMSecsSinceEpoch();
		//part C, C++ and Java
		this->dockerRun->execute(QString("docker create -P --name %1 -t run_common /home/Runcode -r /home/code.run -n 172.17.0.1:23333 -t 1000 -m 65536 -o 40").arg(ContainerName));
		this->dockerRun->execute(QString("docker cp %1 %2:/home").arg(filename + QString("/code.run").arg(ContainerName)));
		this->dockerRun->execute(QString("docker start %1").arg(ContainerName));


	}
};

#endif // MAINW_H
