#include "dockerrun.h"

DockerRun::DockerRun(QWidget *parent) : QWidget(parent)
{
	this->process = new QProcess(this);
	this->outputEdit = new QTextEdit(this);

	this->outputEdit->setReadOnly(true);

	QHBoxLayout *lay = new QHBoxLayout;
	this->setLayout(lay);
	lay->addWidget(this->outputEdit);
}

void DockerRun::execute(QString cmd){
	this->process->start(cmd);
	if(!this->process->waitForStarted(3000)){
		this->outputEdit->insertPlainText(Log::getLogString(Log::Error, "Execute", QString("Program cannot started: %1, reason: %2").arg(cmd).arg(this->process->errorString())));
		return;
	}
	this->process->closeWriteChannel();
	if(!this->process->waitForFinished(10000)){
		this->process->terminate();
		this->outputEdit->insertPlainText(Log::getLogString(Log::Error, "Execute", QString("Program has no result in 10s: %1, reason: %2").arg(cmd).arg(this->process->errorString())));
		return;
	}
	emit this->finished();
	this->process->setReadChannel(QProcess::StandardError);
	QByteArray resultError = this->process->readAll();
	this->process->setReadChannel(QProcess::StandardOutput);
	QByteArray result = this->process->readAll();
	this->exitCode = this->process->exitCode();
	QList<QByteArray> resultList = result.split('\n');
	foreach(auto p, resultList){
		this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Result", QString(p)));
	}
	resultList = resultError.split('\n');
	foreach (auto p, resultList) {
		this->outputEdit->insertPlainText(Log::getLogString(Log::Error, "Stderr", QString(p)));
	}
	this->result = QString(result);
	this->errorString = QString(resultError.isEmpty()?"":resultError);
}

