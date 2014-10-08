#include <QApplication>
#include "BloombergVector.h"
#include "ConsoleTestObj.h"
#include <QtDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QClipboard>
#include "MtgCalculator.h"
#include "CommonFunctions.h"
#include "Waterfall.h"
#include "StressTest.h"
#include "ExcelOutput.h"
#include "BloombergVector.h"
#include "BaseRateVect.h"
#include "BaseRateTable.h"
#include "QBbgRequest.h"
#include "QBbgWorker.h"
#include <QTime>
#include <QTextStream>
#include "DateTrigger.h"
#include "PoolSizeTrigger.h"
int main(int argc, char *argv[]) {
	//QApplication a(argc, argv);
	//ConsoleTestObj a;
	//return a.exec();

	/*Waterfall TempWtf, TempCallWaterfall;
	QFile file("Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/.BaseCase.clo");
	//QFile file("C:/Temp/.SavedInputs.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return 1;
	}
	{bool Junk; out >> Junk; }
	TempWtf.SetLoadProtocolVersion(VersionChecker);
	out >> TempWtf;
	TempCallWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempCallWaterfall;
	file.close();
	TempCallWaterfall.CalculateTranchesCashFlows();*/

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QApplication a(argc, argv);
	if (!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");
	if (!QMetaType::isRegistered(qMetaTypeId<MtgCashFlow>()))
		qRegisterMetaType<MtgCashFlow>("MtgCashFlow");
	Waterfall TempWtf, TempCallWaterfall;
	MtgCalculator TempLoanCalc;
	QFile file("//synserver2/Company Share/24AM/Monitoring/Model Results/HARVT 10X.clom");
	//QFile file("C:/Temp/.SavedInputs.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return 1;
	}
	{QDate Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	TempWtf.SetLoadProtocolVersion(VersionChecker);
	out >> TempWtf;
	TempCallWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempCallWaterfall;
	TempLoanCalc.SetLoadProtocolVersion(VersionChecker);
	out >> TempLoanCalc;
	file.close();
	
	StressTest TempStress;
	for (auto i = TempLoanCalc.GetLoans().constBegin(); i != TempLoanCalc.GetLoans().constEnd(); ++i) {
		TempStress.AddLoan(*(i.value()));
	}
	

	TempStress.SetStructure(TempWtf);
	QList<QString> TempList;
	TempList << "0" << "10";// << "20" << "30" << "40" << "50" << "60" << "70" << "80" << "90" << "100";
	foreach(const QString& tmpstr, TempList)
		TempStress.AddLSscenarios(tmpstr);
	TempList.clear();
	TempList << "0.5" << "1"; //<< "1.5" << "2" << "2.5" << "3" << "3.5" << "4" << "4.5" << "5" << "5.5" << "6" << "6.5" << "7" << "7.5" << "8" << "8.5" << "9";
	foreach(const QString& tmpstr, TempList)
		TempStress.AddCDRscenarios(tmpstr);
	TempStress.AddCPRscenarios("20 48S 5 12S 10 12S 30");
	TempStress.SetUseFastVersion(true);
	//TempStress.UseMultithread(false);
	TempStress.SetStartDate(TempWtf.GetCalculatedMtgPayments().GetDate(0));
	TempStress.RunStressTest();
	return a.exec();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
