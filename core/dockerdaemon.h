#ifndef DOCKERDAEMON_H
#define DOCKERDAEMON_H
/*Docker start if it is no sudoers.
 *FATA[0000] Error starting daemon: open /var/run/docker.pid: permission denied
 */

#include <QWidget>
#include <QtWidgets>
#include "../utils/logsystem.h"

#define Log LogSystem

class DockerDaemon : public QWidget
{
	Q_OBJECT
	int waitingTime;
	int exitCode;
	bool initiativeStopped;
	QString dockerPath;
	QStringList dockerArguments;
	QString dockerSocketPath;

	QTextEdit *outputEdit;
	QProcess *dockerProcess;

	bool killProcess();
	void stopCurrentProgram();
	void closeEvent(QCloseEvent *);

public:
	explicit DockerDaemon(QString dockerPath = QString(), QWidget *parent = 0);
	bool isRunning(){
		return (this->dockerProcess->state() != QProcess::NotRunning);
	}

signals:
	void dockerNotFound();
	void dockerStarted();
	void dockerFinished();

public slots:
	void openDockerDaemon();
	void closeDockerDaemon();

private slots:
	void errorOccurred(QProcess::ProcessError);
	void finished(int, QProcess::ExitStatus);
	void readStandardError();
	void readStandardOutput();
	void stateChanged(QProcess::ProcessState);
};

#endif // DOCKERDAEMON_H
