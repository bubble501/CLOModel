#include "StandaloneStress.h"
#include <QApplication>
#ifdef _DEBUG
#include <QFile>
#include "MtgCalculator.h"
#include "CommonFunctions.h"
#include "Waterfall.h"
#include "StressTest.h"
#endif
int main(int argc, char *argv[])
{
#ifndef _DEBUG	
	QApplication a(argc, argv);
	StandaloneStress TempObj;
	TempObj.show();
	return a.exec();
#else
	/*Mortgage TempLoan;
	TempLoan.SetAnnuity("Y");
	TempLoan.SetSize(245009.0);
	TempLoan.SetInterest("1.52");
	TempLoan.SetMaturityDate(QDate(2040,8,15));
	TempLoan.SetPaymentFreq(1);
	TempLoan.CalculateCashFlows("A 11/30/2013 5.68 1R 14.67 1R 5.63 1R 5.58 1R 5.64 1R 5.60 1R 5.74 1R 6.09 1R 5.60 1R 5.62 1R 5.89 1R 5.59 1R 5.70 1R 6.07 1R 5.89 1R 6.16 1R 9.13 1R 6.66 1R 6.31 1R 7.00 1R 6.09 1R 5.81 1R 6.40 1R 5.91 1R 6.83 1R 8.04 1R 7.05 1R 8.07 1R 9.61 1R 9.73 1R 10.17 1R 8.52 1R 7.69 1R 7.19 1R 7.40 1R 8.01 1R 6.73 1R 7.63 1R 6.64 1R 6.68 1R 7.29 1R 7.48 1R 6.59 1R 6.47 1R 7.20 1R 6.18 1R 6.70 1R 6.16 1R 5.97 1R 6.20 1R 6.34 1R 6.13 1R 6.80 1R 5.90 1R 6.10 1R 6.77 1R 5.96 1R 5.66 1R 5.61 1R 5.72 1R 5.60 1R 5.73 1R 5.62 1R 5.65 1R 5.80 1R 5.60 1R 5.66 1R 5.60 1R 5.59 2R 5.60 1R 5.63 1R 5.65 1R 5.61 1R 5.70 1R 5.70 1R 5.60 1R 5.67 1R 5.69 2R 5.66 1R 5.63 1R 5.61 3R 5.62 1R 5.72 1R 5.86 1R 5.87 1R 5.67 1R 5.68 1R 5.74 1R 5.63 1R 5.62 2R 5.82 1R 5.79 1R 5.96 1R 6.02 2R 5.74 1R 5.60 1R 5.85 1R 5.68 1R 5.61 1R 5.67 1R 5.79 2R 5.59 1R 5.59 1R 5.62 1R 5.76 1R 5.85 1R 5.62 16R 5.63 2R 5.67 1R 5.60 3R 5.74 2R 5.60 7R 5.59 17R 5.69 1R 5.69 1R 5.64 3R 5.62 1R 5.77 2R 5.70 1R 5.63 1R 5.60 2R 5.76 3R 5.69 2R 5.67 1R 5.60 1R 5.63 1R 5.65 2R 5.59 1R 5.61 1R 5.86 1R 5.79 1R 5.69 1R 5.79 1R 5.82 1R 6.04 1R 5.96 1R 5.71 1R 5.88 1R 5.66 1R 5.70 1R 5.91 1R 5.73 1R 5.92 1R 5.72 1R 5.62 1R 5.62 2R 5.68 1R 5.78 1R 5.80 1R 5.67 1R 5.69 1R 5.67 1R 6.03 1R 6.03 1R 5.66 1R 5.66 1R 5.84 1R 5.69 1R 6.40 1R 5.57 1R 5.89 1R 5.73 1R 5.80 1R 5.96 1R 6.00 1R 5.59 1R 5.76 1R 5.85 1R 5.73 1R 5.98 1R 5.90 1R 5.72 1R 5.87 2R 5.75 1R 5.79 1R 5.70 1R 5.62 1R 5.91 1R 5.70 1R 5.76 1R 5.74 1R 5.57 1R 5.63 1R 5.61 1R 5.58 1R 5.58 1R 5.58 1R 5.62 1R 5.60 1R 5.58 1R 5.57 1R 5.58 1R 5.58 1R 5.58 2R 5.59 2R 5.59 1R 5.58 1R 5.59 1R 5.59 1R 5.59 1R 5.59 1R 5.64 3R 5.58 1R 5.57 1R 5.58 2R 5.60 1R 5.58 1R 5.58 3R 5.59 3R 5.58 1R 5.58 2R 5.58 1R 5.58 4R 5.58 1R 5.58 1R 5.57 1R 5.58 1R 5.59 1R 5.58 2R 5.58 8R 5.57 1R 5.58","0.5","15",QDate(2014,4,3));
	*//*QFile file("C:\\Temp\\.Loans.clp");
	MtgCalculator TempCalc;
	TempCalc.AddLoan(TempLoan);
	TempCalc.SetCPR("A 6/15/2013 2 12S 20");
	TempCalc.SetCDR("1");
	TempCalc.SetLS("30");
	TempCalc.SetStartDate(QDate(2014,1,1));
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_4_8);
		out << qint32(ModelVersionNumber) << TempCalc;
		file.close();
	}
	MtgCalculator TempRead;
	if (file.open(QIODevice::ReadOnly)) {
		qint32 Test;
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_4_8);
		out >> Test;
		if(Test==qint32(ModelVersionNumber))
			out >> TempRead;
		file.close();
	}*/
	
	Waterfall TempWtf,TempCallWaterfall;
	QFile file("C:/Temp/.BaseCase.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> VersionChecker;
	if(VersionChecker<qint32(MinimumSupportedVersion)){
		file.close();
		return 1;
	}
	TempWtf.SetLoadProtocolVersion(VersionChecker);
	out >> TempWtf;
	TempCallWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempCallWaterfall;
	file.close();
	TempWtf.SetPaymentFrequency("3 47S 6");
	TempWtf.CalculateTranchesCashFlows();


#endif
}
