#include <QtGui/QApplication>
#include "StandaloneViewer.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	StandaloneViewer w;
	w.show();
	w.AdjustTableSizes();
	return a.exec();
}
