#include "CommonFunctions.h"
#include "Waterfall.h"
#include "ExcelOutput.h"
#include <QtGui>
#include <QtCore>
int main(int argc, char *argv[])
{
	Waterfall TempWaterfall;
	QApplication a(argc, argv);
	QFile file("C:/Temp/.BaseCase.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> VersionChecker;
	if(VersionChecker!=qint32(ModelVersionNumber)) return 0;
	out >> TempWaterfall;
	file.close();
	TempWaterfall.CalculateTranchesCashFlows();
	ExcelOutput::PlotCallToEquity(TempWaterfall,"Graphical Output",6,TempWaterfall.GetCalledPeriod());
	return a.exec();
}
