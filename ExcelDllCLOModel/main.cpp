#include "CommonFunctions.h"
#include "Waterfall.h"
#include "ExcelOutput.h"
#include "StressTest.h"
#include "StandaloneStress.h"
#include <QtGui>
#include <QtCore>
int main(int argc, char *argv[])
{
	
	QApplication a(argc, argv);
	StandaloneStress TempObj;
	TempObj.show();
	return a.exec();
/*
	Mortgage TempLoan;
	TempLoan.SetAnnuity("Y");
	TempLoan.SetSize(1000000.0);
	TempLoan.SetInterest("5");
	TempLoan.SetMaturityDate(QDate(2014,6,1));
	TempLoan.SetPaymentFreq(3);
	TempLoan.CalculateCashFlows("0","0","0",QDate(2014,1,1));
	*/
}
