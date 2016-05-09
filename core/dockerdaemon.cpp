#include "dockerdaemon.h"

bool DockerDaemon::killProcess(){
	if(this->dockerProcess->state() != QProcess::NotRunning){
		//LogSystem::writeDebugLog(LogSystem::Warning, STR(process), "Process is running, attempt to terminate.");
		this->dockerProcess->terminate();
		this->initiativeStopped = true;
		bool reply = this->dockerProcess->waitForFinished(this->waitingTime);
		if(reply == false){
			if(this->dockerProcess->state() != QProcess::NotRunning){
				//LogSystem::writeDebugLog(LogSystem::Warning, STR(process), "Process is still running, force kill.");
				this->dockerProcess->kill();
			}
		}
	}
	if(this->dockerProcess->state() != QProcess::NotRunning){
		initiativeStopped = false;
		return false;
	}else{
		return true;
	}
}

DockerDaemon::DockerDaemon(QString dockerPath, QWidget *parent) : QWidget(parent), waitingTime(3000){
	this->outputEdit = new QTextEdit(this);
	this->dockerProcess = new QProcess(this);

	this->outputEdit->setFont(QFont("courier new, courier, monospace", 9));
	this->outputEdit->setReadOnly(true);
	this->outputEdit->setWordWrapMode(QTextOption::NoWrap);

	this->dockerSocketPath = "/var/run/docker.sock";

	if(dockerPath.isEmpty()){
		this->dockerPath = "/usr/bin/docker";
	}else{
		this->dockerPath = dockerPath;
	}
	this->dockerArguments.append("daemon");

	this->dockerProcess->setProgram(this->dockerPath);
	this->dockerProcess->setArguments(this->dockerArguments);

#if QT_VERSION >= 0x050600 //if QT Version is 5.6.0 or later, QProcess::error signal called errorOccurred
	this->connect(this->dockerProcess, &QProcess::errorOccurred, this, &DockerDaemon::errorOccurred);
#else
	//this->connect(this->dockerProcess, &QProcess::error, this, &DockerDaemon::errorOccurred);
	this->connect(this->dockerProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorOccurred(QProcess::ProcessError)));
#endif
	//this->connect(this->dockerProcess, &QProcess::finished, this, &DockerDaemon::finished);
	this->connect(this->dockerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
	this->connect(this->dockerProcess, &QProcess::readyReadStandardError, this, &DockerDaemon::readStandardError);
	this->connect(this->dockerProcess, &QProcess::readyReadStandardOutput, this, &DockerDaemon::readStandardOutput);
	this->connect(this->dockerProcess, &QProcess::stateChanged, this, &DockerDaemon::stateChanged);

	QHBoxLayout *lay = new QHBoxLayout;
	this->setLayout(lay);
	lay->addWidget(this->outputEdit);

	if(QFile::exists(this->dockerSocketPath)){
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Docker Daemon", "Docker Daemon has started on another handle, it is already opened."));
		emit this->dockerStarted();
	}else{
		this->outputEdit->insertPlainText("Please use system -> start docker daemon to start docker daemon.\n");
	}
}

void DockerDaemon::openDockerDaemon(){
	this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Docker Daemon", QString("run docker: %1").arg(this->dockerPath)));
	if(this->dockerProcess->state() == QProcess::NotRunning){
		if(QFile::exists(this->dockerSocketPath)){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Docker Daemon", "Docker Daemon has started on another handle, it is already opened."));
			emit this->dockerStarted();
		}else{
			this->dockerProcess->start();
		}
	}
}

void DockerDaemon::closeDockerDaemon(){
	this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Docker Daemon", QString("stop docker daemon")));
	if(this->dockerProcess->state() != QProcess::NotRunning){
		this->stopCurrentProgram();
	}else{
		if(QFile::exists(this->dockerSocketPath)){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Docker Daemon", "Docker Daemon has started on another handle, it can't close here."));
		}
	}
}

void DockerDaemon::errorOccurred(QProcess::ProcessError error){
	switch(error){
		case QProcess::FailedToStart:
			/* The Process failed to start.
			 * Either the invoked program is missing
			 * or you may have insufficient
			 * permission to invoke the program.
			 */
			this->killProcess();
			if(this->initiativeStopped){
				this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Docker Daemon", "Docker Daemon Failed to Start, User reason?"));
			}else{
				this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Docker Daemon", "Docker Daemon Failed to Start."));
			}
			this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Docker Daemon", this->dockerProcess->errorString()));
			emit this->dockerFinished();
			break;
		case QProcess::Crashed:
			/* The process crashed some time after
			 * started.
			 */
			this->killProcess();
			if(this->initiativeStopped){
				this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Docker Daemon", "Docker Daemon has stopped."));
			}else{
				this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Docker Daemon", "Docker Daemon stopped abnormally."));
				this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Docker Daemon", this->dockerProcess->errorString()));
			}
			break;
		case QProcess::Timedout:
			/* The last waitfor...() function
			 * timed out. the state of QProcess
			 * is unchanged, and you can try calling
			 * waitFor...() again
			 */
			break;
		case QProcess::WriteError:
			/* Error occurred when attempting to
			 * write to the progress.
			 * process closed its input channel
			 */
			//LogSystem::writeDebugLog(LogSystem::Error, "write", "An error occurred while attempting to write");
			break;
		case QProcess::ReadError:
			/* Error occurred when attempting to
			 * read from the process.
			 * process closed or may not ran yet.
			 */
			//LogSystem::writeDebugLog(LogSystem::Error, "read", "An error occurred while attempting to read");
			break;
		case QProcess::UnknownError:
			/* An unknown error occurred
			 * OR IT IS DEFAULT VALUE CALLING error()
			 */
			break;
	}
}

void DockerDaemon::finished(int exitCode, QProcess::ExitStatus exitStatus){
	switch(exitStatus){
		case QProcess::NormalExit:
			/* The process exitted normally */
			break;
		case QProcess::CrashExit:
			/* The process crashed */
			break;
	}
	this->exitCode = exitCode;
	emit this->dockerFinished();
}

void DockerDaemon::readStandardOutput(){
	qint64 size;
	this->dockerProcess->setReadChannel(QProcess::StandardOutput);
	do{
		size = this->dockerProcess->bytesAvailable();
		QThread::currentThread()->msleep(5);
	}while(size != this->dockerProcess->bytesAvailable());
	QByteArray str = this->dockerProcess->readAllStandardOutput();
	if(str.isEmpty()){
		// if reads none? no output
		return;
	}
	QList<QByteArray> list = str.split('\n');
	foreach(auto p, list){
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "StdOut", QString(p)));
		//qDebug() << "Docker Daemon:" << p;
	}
}

void DockerDaemon::readStandardError(){
	qint64 size;
	this->dockerProcess->setReadChannel(QProcess::StandardError);
	do{
		size = this->dockerProcess->bytesAvailable();
		QThread::currentThread()->msleep(5);
	}while(size != this->dockerProcess->bytesAvailable());
	QByteArray str = this->dockerProcess->readAllStandardError();
	if(str.isEmpty()){
		// if reads none? no output
		return;
	}
	QList<QByteArray> list = str.split('\n');
	foreach(auto p, list){
		//[INFOR][StdErr]: time="2016-05-08T23:57:07.056477360+08:00" level=info msg="Loading containers: done."
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "StdErr", QString(p)));
	}
}

void DockerDaemon::stateChanged(QProcess::ProcessState newState){
	switch(newState){
		case QProcess::NotRunning:
			/* The process turned to notrunning */
			break;
		case QProcess::Starting:
			/* The process attempted to start */
			break;
		case QProcess::Running:
			/* The process is running */
			emit this->dockerStarted();
			break;
	}
}

void DockerDaemon::stopCurrentProgram(){
	//qDebug() << "[DEBUG] [processTimer]: detected timerId:" << this->timerId;
	this->initiativeStopped = true;
	this->killProcess();
}

void DockerDaemon::closeEvent(QCloseEvent *){
	if(this->dockerProcess->state() != QProcess::NotRunning){
		this->stopCurrentProgram();
	}
}

