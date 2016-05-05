#ifndef DOCKERRUN_H
#define DOCKERRUN_H

#include <QtWidgets>
#include <QtCore>
#include "../utils/logsystem.h"

#define Log LogSystem

class DockerRun : public QWidget
{
	Q_OBJECT
	QProcess *process;
	QTextEdit *outputEdit;

	QString errorString;
	int exitCode;
	QString result;
public:
	explicit DockerRun(QWidget *parent = 0);
	QString getErrorString(){
		return this->errorString;
	}
	int getExitCode(){
		return this->exitCode;
	}

	QString getResult(){
		return this->result;
	}

signals:
	void finished();
public slots:
	void execute(QString cmd);
};

#endif // DOCKERRUN_H
