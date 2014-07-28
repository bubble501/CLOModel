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
#endif
int main(int argc, char *argv[]) {


#ifndef _DEBUG	
	QApplication a(argc, argv);
	StandaloneStress TempObj;
	TempObj.show();
	return a.exec();
#else


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
	TempWtf.SetLoadProtocolVersion(VersionChecker);
	out >> TempWtf;
	TempCallWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempCallWaterfall;
	file.close();
	ConstantBaseRateTable ResultingValues;
	for (int i = 0; i < TempWtf.GetTranchesCount();i++) {
		TempWtf.GetTranche(i)->GetBaseRatesDatabase(ResultingValues);
	}

	/*Tranche TempTrnch;
	TempTrnch.SetSettlementDate(QDate(2014, 6, 1));
	TempTrnch.SetOutstandingAmt(1000000.0);
	TempTrnch.SetAccruedInterest(0.0);
	TempTrnch.SetReferenceRateValue("0.5");
	TempTrnch.AddCashFlow(QDate(2015, 6, 1), 50000.0, TrancheCashFlow::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2016, 6, 1), 50000.0, TrancheCashFlow::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2017, 6, 1), 50000.0, TrancheCashFlow::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2018, 6, 1), 50000.0, TrancheCashFlow::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2019, 6, 1), 50000.0, TrancheCashFlow::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2020, 6, 1), 50000.0, TrancheCashFlow::InterestFlow);
	TempTrnch.AddCashFlow(QDate(2020, 6, 1), 1000000.0, TrancheCashFlow::PrincipalFlow);
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

	



	/*Mortgage TempMtg;
	TempMtg.SetAnnuity("I");
	TempMtg.SetInterest("5");
	TempMtg.SetPaymentFreq("3");
	TempMtg.SetMaturityDate(QDate(2020, 5, 30));
	TempMtg.SetSize(1000000.0);
	//TempMtg.SetHaircutVector("0 24S 50");
	TempMtg.CalculateCashFlows(QDate(2014, 5, 30), "0", "0", "0");
	MtgCashFlow BaseFlows = TempMtg.GetCashFlow();
	MtgCashFlow LegacyFlows = BaseFlows.ApplyScenario("10", "40", "50");
	TempMtg.CalculateCashFlows(QDate(2014, 5, 30), "10", "40", "50");
// 	MtgCashFlow LegacyFlows = BaseFlows.ApplyScenario("0", "0 4S 50 0", "100");
// 	TempMtg.CalculateCashFlows(QDate(2014, 5, 30), "0", "0 4S 50 0", "100");
	bool Testing =  TempMtg.GetCashFlow() == LegacyFlows;
	if (!Testing) {
		
				
		QString ResultString("");
		for (int i = 0; i < TempMtg.GetCashFlow().Count(); i++) {
			ResultString += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10\t%11\n")
				.arg(TempMtg.GetCashFlow().GetAmountOut(i))
				.arg(TempMtg.GetCashFlow().GetCoupTimeOut(i))
				.arg(TempMtg.GetCashFlow().GetAccruedInterest(i))
				.arg(TempMtg.GetCashFlow().GetInterest(i))
				.arg(TempMtg.GetCashFlow().GetScheduled(i))
				.arg(TempMtg.GetCashFlow().GetPrepay(i))
				.arg(TempMtg.GetCashFlow().GetDefaults(i))
				.arg(TempMtg.GetCashFlow().GetLoss(i))
				.arg(TempMtg.GetCashFlow().GetRecoveries(i))
				.arg(TempMtg.GetCashFlow().GetLossOnInterest(i))
				.arg(TempMtg.GetCashFlow().GetInterestRecoveries(i))
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
			ResultString += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10\t%11\n")
				.arg(LegacyFlows.GetAmountOut(i))
				.arg(LegacyFlows.GetCoupTimeOut(i))
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
			ResultString += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10\t%11\n")
				.arg(BaseFlows.GetAmountOut(i))
				.arg(BaseFlows.GetCoupTimeOut(i))
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
