#include "Waterfall.h"
#include <QApplication>
#include <QFile>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Waterfall w;
	QFile TargetFile("C:/Temp/.BaseCase.clo");
	TargetFile.open(QIODevice::ReadOnly);
	QDataStream out(&TargetFile);
	out.setVersion(QDataStream::Qt_4_8);
	out >> w;
	TargetFile.close();
	return a.exec();
}
