#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H

#include <QObject>
#include <QtCore>

class LogSystem : public QObject
{
	Q_OBJECT
	QFile *outFile;
public:
	enum InfoLevel{
		None = 0,
		Log,
		Note,
		Debug,
		Info,
		Warning,
		Error,
		Critical,
		Fatal
	};
	explicit LogSystem(QObject *parent = 0);
	explicit LogSystem(const QString &filename, QObject *parent = 0);
	~LogSystem();
	static QString getLogString(InfoLevel level, const QString &valueName, const QString &msg);
	static void writeDebugLog(InfoLevel level, const QString &valueName, const QString &msg);
	//static void writeOutputLog(QTextEdit *dest, InfoLevel level, const QString &valueName, const QString &msg);
	//static void writeOutputLog(QPlainTextEdit *dest, InfoLevel level, const QString &valueName, const QString &msg);

signals:
	void openFailedSig();
public slots:
	void writeFileLog(InfoLevel level, const QString &valueName, const QString &msg);
private:
	static QString getInfoLevelStr(InfoLevel level);
	static QString logStringBuilder(InfoLevel level, const QString &valueName, const QString &msg);
};

#endif // LOGSYSTEM_H
