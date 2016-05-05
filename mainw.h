#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include <QtWidgets>
#include "core/dockerdaemon.h"
#include "core/network.h"
#include "core/dockerrest.h"
#include "core/compileoutput.h"

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

	}
};

#endif // MAINW_H
