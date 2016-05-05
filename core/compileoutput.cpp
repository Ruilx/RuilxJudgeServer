#include "compileoutput.h"

CompileOutput::CompileOutput(QWidget *parent) : QWidget(parent)
{
	this->type = Unknown;

	this->complier = new QProcess(this);
	this->outputEdit = new QTextEdit(this);

	this->outputEdit->setReadOnly(true);
	this->outputEdit->setFont(QFont("courier new, courier, monospace", 9));
	this->outputEdit->setWordWrapMode(QTextOption::NoWrap);


	this->extMap.insert("c", C);
	this->extMap.insert("cc", C);
	this->extMap.insert("cpp", Cpp);
	this->extMap.insert("c++", Cpp);
	this->extMap.insert("j", Java);
	this->extMap.insert("java", Java);

	this->cmdMap.insert(C, "gcc -O2 -o %1 -DONLINE_JUDGE -Wall -lm --static --std=c99 -fno-asm %2");
	this->cmdMap.insert(Cpp, "g++ -O2 -o %1 -DONLINE_JUDGE -Wall -lm --static --std=c++99 -fno-asm %2");
	this->cmdMap.insert(Java, "javac %1");

	QHBoxLayout *lay = new QHBoxLayout;
	this->setLayout(lay);
	lay->addWidget(this->outputEdit);

}
