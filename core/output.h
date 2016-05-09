#ifndef OUTPUT_H
#define OUTPUT_H

#include <QtWidgets>

class Output : public QWidget
{
	Q_OBJECT
	QTextEdit *outputEdit;
public:
	explicit Output(QWidget *parent = 0);
	void insert(const QString &str){
		this->outputEdit->insertPlainText(str);
	}

signals:

public slots:
	void insertText(QString str){
		this->insert(str);
	}
};

#endif // OUTPUT_H
