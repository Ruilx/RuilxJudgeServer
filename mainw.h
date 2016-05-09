#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtCore>
#include "core/dockerdaemon.h"
#include "core/network.h"
#include "core/dockerrest.h"
#include "core/compileoutput.h"
#include "core/dockerrun.h"
#include "core/output.h"

class ProgramStatus{
public:
	/* Status
	 * 0 Unknown
	 * 1 Accepted
	 * 2 Presentation Error
	 * 3 Wrong Answer
	 * 4 Output Limit Excedded
	 * 5 Validator Error
	 * 6 Memory Limit Excedded
	 * 7 Time Limit Excedded
	 * 8 Runtime Error
	 * 9 Compile Error
	 * 10 System Error
	 * 11 Running
	 * 12 Hidden
	 * 13 Restricted Function
	 */
	enum Status{
		Unknown = 0,
		Accepted,
		PresentationError,
		WrongAnswer,
		OutputLimitExcedded,
		ValidatorError,
		MemoryLimitExcedded,
		TimeLimitExcedded,
		RuntimeError,
		CompileError,
		SystemError,
		Running,
		Hidden,
		RestrictedFunction
	};
	static QString getString(Status status){
		switch (status){
			case Unknown:
				return QString("Unknown");
			case Accepted:
				return QString("Accepted");
			case PresentationError:
				return QString("Presentation Error");
			case WrongAnswer:
				return QString("Wrong Error");
			case OutputLimitExcedded:
				return QString("Output Limit Excedded");
			case ValidatorError:
				return QString("Validator Error");
			case MemoryLimitExcedded:
				return QString("Memory Limit Excedded");
			case TimeLimitExcedded:
				return QString("Time Limit Excedded");
			case RuntimeError:
				return QString("Runtime Error");
			case CompileError:
				return QString("Compile Error");
			case SystemError:
				return QString("System Error");
			case Running:
				return QString("Running");
			case Hidden:
				return QString("Hidden");
			case RestrictedFunction:
				return QString("Restricted Function");
		}
		return QString("Unknown");
	}
};

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
	Output *output;
	QMap<QWidget*, int> tabObjectList;
	QMap<qint64, quint64> nameThreadMap;

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

	void activeOutputTabSlot(){
		this->mainWidget->tabBar()->setCurrentIndex(this->tabObjectList.value(this->output));
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
		this->nameThreadMap.insert(ContainerName, 0);
		//part C, C++ and Java
		this->dockerRun->execute(QString("docker create -P --name %1 -t run_common /home/Runcode -r /home/code.run -n 172.17.0.1:23333 -t 1000 -m 65536 -o 40").arg(ContainerName));
		this->dockerRun->execute(QString("docker cp %1 %2:/home").arg(filename + QString("/code.run").arg(ContainerName)));
		this->dockerRun->execute(QString("docker start %1").arg(ContainerName));
	}

	void networkHandle(quint64 tid, QJsonDocument doc){
		/* MESSAGE
		 * Type: 0: name upload, 1: status
		 * Name: this vm's name
		 * Status: Status
		 * Exitcode: exit code
		 * TimeUsed: time used
		 * MemoryUsed: memory used
		 * OutputString: result
		 */
		qDebug() << "[Debug][MainW] networkHandle: handle id:" << tid << "data:" << doc;
		QVariantMap result = doc.toVariant().toMap();
		//TODO: This code is issue!
		int type = result.value("Type", -1).toInt();
		QVariantList info;
		QVariantMap infoMap;
		QJsonDocument docWillSend;
		QString name;

		ProgramStatus::Status status;
		int exitCode;
		int timeUsed;
		int memoryUsed;
		QString outputString;
		switch(type){
			case 0:
				qDebug() << "[Debug][MainW] networkHandle: receive type: 0";
				name = result.value("Name", QString("-1")).toString();
				this->nameThreadMap.take(name.toInt());
				this->nameThreadMap.insert(name.toInt(), tid);
				infoMap.insert("Name", name);
				infoMap.insert("Message", 1); //Start Working

				info << infoMap;

				docWillSend = QJsonDocument::fromVariant(info);
				if(docWillSend.isNull()){
					qDebug() << "[ERROR][jsonTake]: info JSON is Empty";
					return;
				}
				qDebug() << "[DEBUG][jsonTake]: JSON:" << docWillSend;
				this->network->send(tid, docWillSend);
				break;
			case 1:
				qDebug() << "[Debug][MainW] networkHandle: receive type: 1";
				name = result.value("Name", QString("-1")).toString();
				status = (ProgramStatus::Status)result.value("Status", ProgramStatus::Unknown).toInt();
				exitCode = result.value("Exitcode", -1).toInt();
				timeUsed = result.value("TimeUsed", -1).toInt();
				memoryUsed = result.value("MemoryUsed", -1).toInt();
				outputString = result.value("OutputString", QString()).toString();

				if(status == ProgramStatus::RuntimeError ||
						status == ProgramStatus::TimeLimitExcedded ||
						status == ProgramStatus::MemoryLimitExcedded ||
						status == ProgramStatus::OutputLimitExcedded ||
						status == ProgramStatus::Unknown){
					//output Window
					this->activeOutputTabSlot();
					this->output->insert(ProgramStatus::getString(status));
					this->output->insert("Finished.");
					return;
				}
				this->output->insert("Finished.");
				break;
			default:
				qDebug() << "[Debug][MainW] networkHandle: receive Type:" << type;
				qDebug() << "[Debug][MainW] networkHandle: inavild type.";
		}
	}
};

#endif // MAINW_H
