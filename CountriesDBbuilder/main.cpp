#include "countriesdbbuilder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CountriesDBbuilder w;
	w.show();
	return a.exec();
}
