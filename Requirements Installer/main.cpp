#include "requirementsinstaller.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RequirementsInstaller w;
	w.show();
	return a.exec();
}
