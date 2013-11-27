#include <QtGui/QApplication>
#include "StandaloneViewer.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	StandaloneViewer w;
	if(a.arguments().size()>1) w.LoadFile(a.arguments().at(1));
	w.show();
	w.AdjustTableSizes();
	return a.exec();
}
