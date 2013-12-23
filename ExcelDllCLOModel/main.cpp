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
	double test=TempWaterfall.GetTranche(7)->GetDiscountMargin();
	
	/*Mortgage TempLoan;
	TempLoan.SetAnnuity("N");
	TempLoan.SetSize(1000000.0);
	TempLoan.SetInterest("5");
	TempLoan.SetMaturityDate(QDate(2020,6,30));
	TempLoan.SetPaymentFreq(3);
	TempLoan.CalculateCashFlows("0","0","0",QDate(2013,5,30));*/
	return a.exec();
}
