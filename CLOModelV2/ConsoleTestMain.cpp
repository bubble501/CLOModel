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
#include "StressViewer.h"
#include "SummaryView.h"
#include "WaterfallStepHelperDialog.h"
#include "TriggerStructHelperWidget.h"
#include "TriggerHelperDialog.h"
#include "FloorCapVector.h"
int main(int argc, char *argv[]) {

	/*QApplication a(argc, argv);
	WaterfallStepHelperDialog b;
	if (b.exec()==QDialog::Accepted) {
		qDebug() << b.GetParameters();
	}
	return a.exec();*/


	/*QApplication a(argc, argv);
	StressViewer b;
	b.show();
	b.LoadStress("C:/Temp/StressResult.fcsr");
	return a.exec();*/

	/*Waterfall TempWtf, TempCallWaterfall;
	QFile file("Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/BaseCase.clo");
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
	TempWtf.CalculateTranchesCashFlows();*/

	/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QApplication a(argc, argv);
	RegisterAsMetaType<Waterfall>();
	RegisterAsMetaType<MtgCashFlow>();

	Tranche TeResult = *( StressTest::GetScenarioFromFile(
		"Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/StressResult.fcsr"
		, "20", "0.5", "0", "0", "0", "0"
		).GetTranche("HARVT 10X A"));
	return a.exec();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*
	
	Waterfall TempWtf, TempCallWaterfall;
	MtgCalculator TempMtg;
	QFile file("Z:/24AM/Monitoring/Model Results/HARVT 10X.clom");
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
	TempMtg.SetLoadProtocolVersion(VersionChecker);
	out >> TempMtg;
	file.close();
	

	Mortgage TempMtg1;
	TempMtg1.SetFloatingRateBase("EUR003M[3,5]");
	TempMtg1.SetAnnuity("I");
	TempMtg1.SetInterest("5");
	TempMtg1.SetPaymentFreq("1");
	TempMtg1.SetMaturityDate(QDate(2020, 5, 30));
	TempMtg1.SetSize(1000000.0);
	TempMtg1.SetHaircutVector("0 24S 50");
	TempMtg.AddLoan(TempMtg1,TempMtg.GetLoans().size()+1);
	TempMtg.ReadyToCalculate();
	TempMtg.StartCalculation();
	TempWtf.CalculateTranchesCashFlows();
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QApplication a(argc, argv);
	//QFile file("C:/Temp/Wrong 20#,#8#,#100#,#0#,#0#,#0.csw");
	QFile file("C:/Temp/20#,#8#,#100#,#0#,#0#,#0.csw");
	//QFile file("Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/BaseCase.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
	file.close();
	return 1;
	}
	//{bool Junk; out >> Junk; }
	Waterfall TempWtf;
	Tranche TempTr;
	TempWtf.AddTranche(TempTr);
// 	TempWtf.SetLoadProtocolVersion(VersionChecker);
// 	out >> TempWtf;
// 	file.close();
	//TempWtf.CalculateTranchesCashFlows();
	SummaryView Check;
	Check.show();
	Check.SetStructure(TempWtf);
	return a.exec();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

	Mortgage TempMtg1;
	TempMtg1.SetProperty("PrepaymentFee", "5");
	TempMtg1.SetAnnuity("I");
	TempMtg1.SetInterest("5");
	TempMtg1.SetPaymentFreq("1");
	TempMtg1.SetMaturityDate(QDate(2020, 5, 30));
	TempMtg1.SetSize(1000000.0);
	TempMtg1.SetHaircutVector("0 24S 50");
	TempMtg1.CalculateCashFlows(QDate(2014, 5, 30), "0", "0", "0");

	/*TempMtg1.SetProperty("PrepaymentFee", "10");
	TempMtg1.SetAnnuity("A");
	TempMtg1.SetInterest("2.5");
	TempMtg1.SetPaymentFreq("6");
	TempMtg1.SetMaturityDate(QDate(2018, 5, 30));
	TempMtg1.SetSize(500000.0);
	TempMtg1.CalculateCashFlows(QDate(2014, 5, 30), "0", "0", "0");*/


	Mortgage TempMtg2;
	TempMtg2.SetProperty("PrepaymentFee", "10");
	TempMtg2.SetAnnuity("A");
	TempMtg2.SetInterest("2.5");
	TempMtg2.SetPaymentFreq("6");
	TempMtg2.SetMaturityDate(QDate(2018, 5, 30));
	TempMtg2.SetSize(500000.0);
	TempMtg2.CalculateCashFlows(QDate(2014, 5, 30), "0", "0", "0");

	MtgCashFlow BaseFlows = TempMtg1.GetCashFlow();
	BaseFlows += TempMtg2.GetCashFlow();

	MtgCashFlow LegacyFlows = BaseFlows.ApplyScenario("10", "0", "50");
	TempMtg1.CalculateCashFlows(QDate(2014, 5, 30), "10", "0", "50");
	TempMtg2.CalculateCashFlows(QDate(2014, 5, 30), "10", "0", "50");

	MtgCashFlow TempMtgsFlows = TempMtg1.GetCashFlow();
	TempMtgsFlows += TempMtg2.GetCashFlow();
	
	QFile file1("C:/Temp/Scenario.log");
	QFile file2("C:/Temp/Model.log");
	QFile file3("C:/Temp/Base.log");
	if (file1.exists()) file1.remove();
	if (file2.exists()) file2.remove();
	if (file3.exists()) file3.remove();
	if (/*true  ||*/ TempMtgsFlows != LegacyFlows) {
		PrintToTempFile("Model", TempMtgsFlows.ToPlainText(true), false);
		PrintToTempFile("Scenario", LegacyFlows.ToPlainText(true), false);
		PrintToTempFile("Base", BaseFlows.ToPlainText(true), false);
	}
	else {
		qDebug() << "Success, Cash Flows are identical";
	}
	return 0;
}
