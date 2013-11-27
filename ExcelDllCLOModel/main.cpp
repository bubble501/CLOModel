#include "CommonFunctions.h"
#include "Waterfall.h"
#include "ExcelOutput.h"
#include <QtGui>
#include <QtCore>
int main(int argc, char *argv[])
{
	Waterfall TempWaterfall;
	QApplication a(argc, argv);
	QFile file("Z:/24AM/ABS Trade Folders/CLO/CADOG 5X/.BaseCase.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> VersionChecker;
	if(VersionChecker!=qint32(ModelVersionNumber)) return 0;
	out >> TempWaterfall;
	file.close();
	QDate TempDate=TempWaterfall.GetCalledPeriod();
	QMessageBox::information(0,"Calculated Date",TempDate.toString("dd-MM-yyyy"));
	return a.exec();
}
