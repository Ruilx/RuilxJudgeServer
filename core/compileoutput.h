#ifndef COMPILEOUTPUT_H
#define COMPILEOUTPUT_H

#include <QWidget>
#include <QtCore>
#include <QtWidgets>
#include "../utils/logsystem.h"

#define Log LogSystem
#define FileNameTimeFormat "yyyyMMddhhmmsszzz"

class CompileOutput : public QWidget
{
	Q_OBJECT
public:
	enum LangType{
		Unknown = 0,
		C,
		Cpp,
		Java
	};
private:
	QProcess *complier;
	QTextEdit *outputEdit;
	QMap<QString, LangType> extMap;
	QMap<LangType, QString> cmdMap;
	bool hasWarning;
	int exitCode;

	QString sourcePath;
	LangType type;
public:
	explicit CompileOutput(QWidget *parent = 0);

	int getExitCode(){
		return this->exitCode;
	}

	bool getHasWarning(){
		return this->hasWarning;
	}

	void setSourceCodePath(QString sourceCodePath){
		this->sourcePath = sourceCodePath;
	}

	void setLanguageType(LangType type){
		this->type = type;
	}

signals:
	void finished();
public slots:
	void startCompile(){
		if(!QFile::exists(sourcePath)){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Source", QString("No such source file: %1").arg(sourcePath)));
			return;
		}
		QFileInfo info(sourcePath);
		QString ext = info.suffix();
		LangType type;
		if((type = this->extMap.value(ext, Unknown)) == Unknown){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Source", QString("Cannot resolve file which type is: %1").arg(sourcePath)));
			return;
		}
		if(type != this->type){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Source", QString("Program verify suffix of file error: %1").arg(sourcePath)));
			return;
		}
		QString compileCmd;
//		if((compileCmd = cmdMap.value(type, QString())).isEmpty()){
//			this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Source", QString("Program suffix has no compile execute: %1").arg(sourcePath)));
//			return;
//		}

//		QString dirPath = QDateTime::currentDateTime().toString(FileNameTimeFormat);
//		QDir currentDir = QDir::current();
//		QDir dir = QDir(currentDir.path().append("/").append(dirPath));
//		if(!dir.exists()){
//			currentDir.mkdir(dirPath);
//		}
//		QDir currentDir = QDir::current();
//		QDir dir = QDir(currentDir.path().append("/").append(dirPath));

		QFileInfo source(sourcePath);
		QString compilePath = source.dir().path();
		switch(type){
			case C:
				compileCmd = QString("gcc -O2 -o ") + compilePath + QString("/") + QString("code.run") + QString(" -DONLINE_JUDGE -Wall -lm --static --std=c99 -fno-asm ") + sourcePath;
				break;
			case Cpp:
				//compileCmd = compileCmd.arg(compilePath + QString("/") + QString("code.run")).arg(sourcePath);
				compileCmd = QString("g++ -O2 -o ") + compilePath + QString("/") + QString("code.run") + QString(" -DONLINE_JUDGE -Wall -lm --static --std=c99 -fno-asm ") + sourcePath;
				break;
			case Java:
				compileCmd = QString("javac ") + sourcePath;;
				break;
			case Unknown:
				this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Complier", QString("Unknown file types, check your source file type.")));
				return;
		}
		this->complier->start(compileCmd);
		if(!this->complier->waitForStarted(3000)){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Critical, "Complier", QString("Complier cannot started, please check your cc options.")));
			return;
		}
		this->complier->closeWriteChannel();
		if(!this->complier->waitForFinished(10000)){
			return;
		}
		this->complier->setReadChannel(QProcess::StandardError);
		QByteArray result = this->complier->readAll();
		this->complier->setReadChannel(QProcess::StandardOutput);
		qDebug() << "[DEBUG][Stdout]:" << this->complier->readAll();
		this->exitCode = this->complier->exitCode();
		if(result.isEmpty() && this->exitCode == 0){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Source", QString("Program has been complied: %1").arg(sourcePath)));
			emit this->finished();
		}else if(result.isEmpty() && this->exitCode != 0){
			this->outputEdit->insertPlainText(Log::getLogString(Log::Warning, "Source", QString("CC returns abnormal, but no output.")));
			emit this->finished();
		}else if(!result.isEmpty()/* && this->exitCode == 0*/){
//			this->outputEdit->insertPlainText(Log::getLogString(Log::Warning, "Source", QString("CC returns normal, and infomations:")));
//			qDebug() << "Complier:" << result;
//			QList<QByteArray> information = result.split('\n');
//			foreach(auto p, information){
//				this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Complier", QString(p)));
//			}
//			emit this->finished();
//		}else if(!result.isEmpty() && this->exitCode != 0){
			QString errStr = QString(result);
			QStringList errorList = errStr.split("\n", QString::SkipEmptyParts);
			QString tempStr;
			QStringList tempStrList;
			short errorCount = 0;
			short warningCount = 0;

			for(int i=0; i<errorList.length(); i++){
				errStr = errorList.at(i);
				//errStr.remove(0, (this->filename.length()+1));
				if((errStr.contains("error", Qt::CaseInsensitive) || errStr.contains("错误", Qt::CaseInsensitive)) && errStr.length() > 10){
					tempStr.append(errStr);
					errorCount++;
				}else if((errStr.contains("warning", Qt::CaseInsensitive) || errStr.contains("警告", Qt::CaseInsensitive)) && errStr.length() > 10){
					tempStr.append(errStr);
					this->hasWarning = true;
					warningCount++;
				}else{
					tempStr.append(errStr);
				}
			}
			tempStrList = tempStr.split('\n');
			foreach (auto p, tempStrList) {
				this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Compile", p));
			}
			this->outputEdit->insertPlainText(Log::getLogString(Log::Info, "Compile", QString("Total ") + QString::number(errorCount) +
						   QString(" Error(s) and ") +
						   QString::number(warningCount) + QString(" Warning(s).\n")));
			emit this->finished();
		}

	}
};

#endif // COMPILEOUTPUT_H
