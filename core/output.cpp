#include "output.h"

Output::Output(QWidget *parent) : QWidget(parent)
{
	this->outputEdit = new QTextEdit(this);

	QHBoxLayout *lay = new QHBoxLayout;
	this->setLayout(lay);
	lay->addWidget(this->outputEdit);

}
