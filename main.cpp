#include "mainw.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setFont(QFont("Microsoft YaHei", 9));
	MainW w;
	w.show();

	return a.exec();
}
