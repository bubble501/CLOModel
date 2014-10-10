#include "StandaloneStress.h"
#include <QApplication>


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	StandaloneStress TempObj;
	TempObj.show();
	return a.exec();
}
