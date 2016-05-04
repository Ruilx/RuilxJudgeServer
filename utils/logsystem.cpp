#include "logsystem.h"

QString LogSystem::getInfoLevelStr(LogSystem::InfoLevel level){
	switch(level){
		case None:
			return QString("[NOMSG]");
			break;
		case Log:
			return QString("[ LOG ]");
			break;
		case Note:
			return QString("[ NOTE]");
			break;
		case Debug:
			return QString("[DEBUG]");
			break;
		case Info:
			return QString("[INFOR]");
			break;
		case Warning:
			return QString("[WARNI]");
			break;
		case Error:
			return QString("[ERROR]");
			break;
		case Critical:
			return QString("[CRTCL]");
			break;
		case Fatal:
			return QString("[FATAL]");
			break;
		default:
			return QString("[UKNOW]");
	}
}

QString LogSystem::logStringBuilder(LogSystem::InfoLevel level, const QString &valueName, const QString &msg){
	QString message;
	message.append(getInfoLevelStr(level));
	message.append("[").append(valueName).append("]: ");
	message.append(msg);
	message.append("\n");
	return message;
}

LogSystem::LogSystem(QObject *parent) : QObject(parent){
}

LogSystem::LogSystem(const QString &filename, QObject *parent): QObject(parent){
	if(QFile::exists(filename)){
		QFile::rename(filename, QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + filename);
	}
	this->outFile = new QFile(filename);
	if(!outFile->open(QIODevice::WriteOnly | QIODevice::Append)){
		emit this->openFailedSig();
		return;
	}
}

LogSystem::~LogSystem()
{
	if(this->outFile->isOpen()){
		this->outFile->close();
	}
}

QString LogSystem::getLogString(LogSystem::InfoLevel level, const QString &valueName, const QString &msg){
	return logStringBuilder(level, valueName, msg);
}

void LogSystem::writeDebugLog(LogSystem::InfoLevel level, const QString &valueName, const QString &msg){
	qDebug() << logStringBuilder(level, valueName, msg).remove("\n");
}

//void LogSystem::writeOutputLog(QTextEdit *dest, LogSystem::InfoLevel level, const QString &valueName, const QString &msg){
//	if(dest == nullptr){
//		return;
//	}
//	dest->insertPlainText(logStringBuilder(level, valueName, msg));
//}

//void LogSystem::writeOutputLog(QPlainTextEdit *dest, LogSystem::InfoLevel level, const QString &valueName, const QString &msg){
//	if(dest == nullptr){
//		return;
//	}
//	dest->insertPlainText(logStringBuilder(level, valueName, msg));
//}

void LogSystem::writeFileLog(LogSystem::InfoLevel level, const QString &valueName, const QString &msg){
	QTextStream stream(this->outFile);
	stream.setCodec("UTF-8");
	stream << logStringBuilder(level, valueName, msg);
}

