#include <QtGui/QApplication>
#include "StandaloneViewer.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("CLO Viewer");
	StandaloneViewer w;
	if(a.arguments().size()>1) w.LoadFile(a.arguments().at(1));
	w.show();
	w.AdjustTableSizes();
	return a.exec();
}
