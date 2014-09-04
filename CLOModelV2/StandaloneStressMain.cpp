#include "StandaloneStress.h"
#include <QApplication>
#ifdef _DEBUG
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
#include <QProgressBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTime>
#include <QTextStream>
#include <QDomDocument>
#endif


int main(int argc, char *argv[]) {


#ifndef _DEBUG	
	QApplication a(argc, argv);
	StandaloneStress TempObj;
	TempObj.show();
	return a.exec();
#else
	/*QApplication a(argc, argv);
	StandaloneStress TempObj;
	TempObj.show();
	return a.exec();*/

	/*QFile file("C:\\Temp\\.Loans.clp");
	MtgCalculator TempCalc;
	TempCalc.AddLoan(TempLoan);
	TempCalc.SetCPR("A 6/15/2013 2 12S 20");
	TempCalc.SetCDR("1");
	TempCalc.SetLS("30");
	TempCalc.SetStartDate(QDate(2014,1,1));
	if (file.open(QIODevice::WriteOnly)) {
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out << qint32(ModelVersionNumber) << TempCalc;
	file.close();
	}
	MtgCalculator TempRead;
	if (file.open(QIODevice::ReadOnly)) {
	qint32 Test;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> Test;
	if(Test==qint32(ModelVersionNumber))
	out >> TempRead;
	file.close();
	}*/
	MtgCalculator TempCalucator;
	Mortgage TestMtg;
	TestMtg.SetProperty("Issuer", "PARGN 17");
	TestMtg.SetProperty("Facility", "XS0843326314");
	TestMtg.SetSize(10000000.0);
	TempCalucator.AddLoan(TestMtg);
	TempCalucator.StartCalculation(true);


/*
	Waterfall TempWtf, TempCallWaterfall;
	QFile file("Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/.BaseCase.clo");
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
	//TempWtf.CalculateTranchesCashFlows();
	double Testing=TempWtf.GetTranche(0)->GetDiscountMargin(100.0);*/
	

	/*Tranche TempTrnch;
	TempTrnch.SetSettlementDate(QDate(2014, 6, 1));
	TempTrnch.SetOutstandingAmt(1000000.0);
	TempTrnch.SetAccruedInterest(0.0);
	TempTrnch.SetReferenceRate("BP00ON");
	//TempTrnch.SetDefaultRefRate("BP00ON");
	TempTrnch.AddCashFlow(QDate(2015, 6, 1), 50000.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2016, 6, 1), 50000.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2017, 6, 1), 50000.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2018, 6, 1), 50000.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2019, 6, 1), 50000.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2020, 6, 1), 50000.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2020, 6, 1), 1000000.0, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
	double currentDM = TempTrnch.GetDiscountMargin(100.0);
	double NewPrice = TempTrnch.GetPrice(currentDM - 1.0);
	double NewDM = TempTrnch.GetDiscountMargin(NewPrice);*/

	/*QApplication a(argc, argv);
	StressTest SlowResult;
	SlowResult.LoadResultsFromFile("Z:\\24AM\\Personal Folders\\LB\\CLO 2.0\\RMFE IV Stress Slow.fcsr");
	StressTest FastResult;
	FastResult.LoadResultsFromFile("Z:\\24AM\\Personal Folders\\LB\\CLO 2.0\\RMFE IV Stress Fast.fcsr");
	const QList<QString>& XRef = FastResult.GetXSpann();
	const QList<QString>& YRef = FastResult.GetYSpann();
	for (int i = 0; i < FastResult.GetStructure().GetTranchesCount(); i++) {
		foreach(const QString& TempX, XRef) {
			foreach(const QString& TempY, YRef) {
				const Tranche* currentTRF = FastResult.GetResults().value(TempX).value(TempY).GetTranche(i);
				const Tranche* currentTRS = SlowResult.GetResults().value(TempX).value(TempY).GetTranche(i);
				if (currentTRF->GetCashFlow().Count() != currentTRS->GetCashFlow().Count()) {
					QMessageBox::critical(0, "Error", "Cash Flows are NOT identical");
					return 1;
				}
				for (int j = 0; j < currentTRF->GetCashFlow().Count(); j++) {
					if (
						qAbs(currentTRF->GetCashFlow().GetAmountOutstanding(j) - currentTRS->GetCashFlow().GetAmountOutstanding(j))>=0.01
						|| qAbs(currentTRF->GetCashFlow().GetDeferred(j) - currentTRS->GetCashFlow().GetDeferred(j)) >= 0.01
						|| qAbs(currentTRF->GetCashFlow().GetICTest(j) - currentTRS->GetCashFlow().GetICTest(j)) >= 0.01
						|| qAbs(currentTRF->GetCashFlow().GetOCTest(j) - currentTRS->GetCashFlow().GetOCTest(j)) >= 0.01
						|| qAbs(currentTRF->GetCashFlow().GetInterest(j) - currentTRS->GetCashFlow().GetInterest(j)) >= 0.01
						|| qAbs(currentTRF->GetCashFlow().GetPrincipal(j) - currentTRS->GetCashFlow().GetPrincipal(j)) >= 0.01
					) {
						QMessageBox::critical(0, "Error", "Cash Flows are NOT identical");
						return 1;
					}
				}
			}
		}
	}
	QMessageBox::information(0, "Success", "Cash Flows are identical");*/

	



	/*Mortgage TempMtg1;
	TempMtg1.SetAnnuity("I");
	TempMtg1.SetInterest("5");
	TempMtg1.SetPaymentFreq("1");
	TempMtg1.SetMaturityDate(QDate(2020, 5, 30));
	TempMtg1.SetSize(1000000.0);
	TempMtg1.SetHaircutVector("0 24S 50");
	TempMtg1.CalculateCashFlows(QDate(2014, 5, 30), "0", "0", "0");

	Mortgage TempMtg2;
	TempMtg2.SetAnnuity("A");
	TempMtg2.SetInterest("2.5");
	TempMtg2.SetPaymentFreq("6");
	TempMtg2.SetMaturityDate(QDate(2018, 5, 30));
	TempMtg2.SetSize(500000.0);
	//TempMtg2.SetPrepayMultiplier("50");
	TempMtg2.CalculateCashFlows(QDate(2014, 5, 30), "0", "0", "0");

	MtgCashFlow BaseFlows = TempMtg1.GetCashFlow();
	BaseFlows += TempMtg2.GetCashFlow();

	MtgCashFlow LegacyFlows = BaseFlows.ApplyScenario("10", "0", "50");
	//LegacyFlows.Aggregate(GenericCashFlow::Quarterly);
	TempMtg1.CalculateCashFlows(QDate(2014, 5, 30), "10", "0", "50");
	TempMtg2.CalculateCashFlows(QDate(2014, 5, 30), "10", "0", "50");

	MtgCashFlow TempMtgsFlows = TempMtg1.GetCashFlow();
	TempMtgsFlows += TempMtg2.GetCashFlow();
	bool Testing = TempMtgsFlows == LegacyFlows;
	if (true) {
		QString ResultString("");
		for (int i = 0; i < TempMtgsFlows.Count(); i++) {
			ResultString += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10\t%11\t%12\t%13\n")
				.arg(TempMtgsFlows.GetAmountOut(i))
				.arg(TempMtgsFlows.GetCoupTimeOut(i))
				.arg(TempMtgsFlows.GetFlow(i,MtgCashFlow::WAPrepayMult))
				.arg(TempMtgsFlows.GetFlow(i, MtgCashFlow::WALossMult))
				.arg(TempMtgsFlows.GetAccruedInterest(i))
				.arg(TempMtgsFlows.GetInterest(i))
				.arg(TempMtgsFlows.GetScheduled(i))
				.arg(TempMtgsFlows.GetPrepay(i))
				.arg(TempMtgsFlows.GetDefaults(i))
				.arg(TempMtgsFlows.GetLoss(i))
				.arg(TempMtgsFlows.GetRecoveries(i))
				.arg(TempMtgsFlows.GetLossOnInterest(i))
				.arg(TempMtgsFlows.GetInterestRecoveries(i))
				;
		}
		{
			QFile file("C://Temp//Model.txt");
			file.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream out(&file);
			out << ResultString;
			file.close();
		}

		ResultString="";
		for (int i = 0; i < LegacyFlows.Count(); i++) {
			ResultString += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10\t%11\t%12\t%13\n")
				.arg(LegacyFlows.GetAmountOut(i))
				.arg(LegacyFlows.GetCoupTimeOut(i))
				.arg(LegacyFlows.GetFlow(i, MtgCashFlow::WAPrepayMult))
				.arg(LegacyFlows.GetFlow(i, MtgCashFlow::WALossMult))
				.arg(LegacyFlows.GetAccruedInterest(i))
				.arg(LegacyFlows.GetInterest(i))
				.arg(LegacyFlows.GetScheduled(i))
				.arg(LegacyFlows.GetPrepay(i))
				.arg(LegacyFlows.GetDefaults(i))
				.arg(LegacyFlows.GetLoss(i))
				.arg(LegacyFlows.GetRecoveries(i))
				.arg(LegacyFlows.GetLossOnInterest(i))
				.arg(LegacyFlows.GetInterestRecoveries(i))
				;
		}
		{
			QFile file("C://Temp//Scenario.txt");
			file.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream out(&file);
			out << ResultString;
			file.close();
		}
		ResultString = "";
		for (int i = 0; i < BaseFlows.Count(); i++) {
			ResultString += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10\t%11\t%12\t%13\n")
				.arg(BaseFlows.GetAmountOut(i))
				.arg(BaseFlows.GetCoupTimeOut(i))
				.arg(BaseFlows.GetFlow(i, MtgCashFlow::WAPrepayMult))
				.arg(BaseFlows.GetFlow(i, MtgCashFlow::WALossMult))
				.arg(BaseFlows.GetAccruedInterest(i))
				.arg(BaseFlows.GetInterest(i))
				.arg(BaseFlows.GetScheduled(i))
				.arg(BaseFlows.GetPrepay(i))
				.arg(BaseFlows.GetDefaults(i))
				.arg(BaseFlows.GetLoss(i))
				.arg(BaseFlows.GetRecoveries(i))
				.arg(BaseFlows.GetLossOnInterest(i))
				.arg(BaseFlows.GetInterestRecoveries(i))
				;
		}
		{
			QFile file("C://Temp//Base.txt");
			file.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream out(&file);
			out << ResultString;
			file.close();
		}

	}
	else {
		QFile file1("C://Temp//Scenario.txt");
		QFile file2("C://Temp//Model.txt");
		QFile file3("C://Temp//Base.txt");
		if (file1.exists()) file1.remove();
		if (file2.exists()) file2.remove();
		if (file3.exists()) file3.remove();
		QApplication a(argc, argv);
		QMessageBox::information(0,"Success", "Cash Flows are identical");		
	}*/


#endif
}
