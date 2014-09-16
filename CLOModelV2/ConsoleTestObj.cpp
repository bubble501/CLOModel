#include "ConsoleTestObj.h"

ConsoleTestObj::ConsoleTestObj(QObject *parent)
	: QObject(parent)
{
	connect(&TempUnit, &CentralUnit::CalculationFinished, this, &ConsoleTestObj::PrintOutput);
	BBVALTest();
}

void ConsoleTestObj::BBVALTest() {
	

	//Lease Pool
	Mortgage SingleLease;
	SingleLease.setFixedRate();
	SingleLease.SetAnnuity("A");
	SingleLease.SetPaymentFreq("1");

	SingleLease.SetMaturityDate(QDate(2014, 12, 1));
	SingleLease.SetSize(1025910.70);
	SingleLease.SetInterest("1.4");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2015, 7, 1));
	SingleLease.SetSize(10435759.24);
	SingleLease.SetInterest("1.29");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2016, 7, 1));
	SingleLease.SetSize(23285099.73);
	SingleLease.SetInterest("1.19");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2017, 7, 1));
	SingleLease.SetSize(26545001.28);
	SingleLease.SetInterest("1.28");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2018, 7, 1));
	SingleLease.SetSize(25773419.87);
	SingleLease.SetInterest("1.32");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2019, 7, 1));
	SingleLease.SetSize(28252193.86);
	SingleLease.SetInterest("1.28");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2020, 7, 1));
	SingleLease.SetSize(35064107.08);
	SingleLease.SetInterest("1.41");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2021, 7, 1));
	SingleLease.SetSize(33341995.25);
	SingleLease.SetInterest("1.25");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2022, 7, 1));
	SingleLease.SetSize(9311296.88);
	SingleLease.SetInterest("1.1");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2023, 7, 1));
	SingleLease.SetSize(857504.59);
	SingleLease.SetInterest("1.4");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 7, 1));
	SingleLease.SetSize(2375350.94);
	SingleLease.SetInterest("1.4");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2025, 7, 1));
	SingleLease.SetSize(6840708.42);
	SingleLease.SetInterest("1.23");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 1));
	SingleLease.SetSize(6560930.02);
	SingleLease.SetInterest("1.3");
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 7, 1));
	SingleLease.SetSize(735423.53);
	SingleLease.SetInterest("1.26");
	TempUnit.AddLoan(SingleLease);
	//Recoveries
	SingleLease.SetAnnuity("I");
	SingleLease.SetInterest("0");
	SingleLease.SetSize(835096.0);
	SingleLease.SetPrepayMultiplier("0");
	SingleLease.SetLossMultiplier("0");

	SingleLease.SetMaturityDate(QDate(2014, 10, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2015, 1, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2015, 4, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2015, 7, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2015, 10, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2016, 1, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2016, 4, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2016, 7, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2016, 10, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2017, 1, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2017, 4, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2017, 7, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2017, 10, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2018, 1, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2018, 4, 26));
	TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2018, 7, 26));
	TempUnit.AddLoan(SingleLease);

	//Tranches
	QList<Tranche::TrancheInterestType> TempFlowType; TempFlowType << Tranche::TrancheInterestType::FloatingInterest;
	QList<QString> TempRef; TempRef << "EUR003M";

	QList<QString> TempCoup; TempCoup << "15";
	TempUnit.AddTranche(
		"BBVAL 2007-1 A1"
		, "ES0314209000"
		, 1
		, 750000000.0
		, "EUR"
		, 34083075.00
		, TempFlowType
		, TempCoup
		, TempRef
		, QDate(2014, 8, 26)
		, "EUR003M"
		, "3"
		, QDate(2014, 9, 15)
		, 0
		, 80000000.0
		, 0.0
		, 100.0
		, 1.0
		, "Mtge"
		, static_cast<DayCountConvention>(102)
		);
	TempCoup.clear(); TempCoup << "20";
	TempUnit.AddTranche(
		"BBVAL 2007-1 A2"
		, "ES0314209018"
		, 1
		, 1606200000.0
		, "EUR"
		, 72992313.42
		, TempFlowType
		, TempCoup
		, TempRef
		, QDate(2014, 8, 26)
		, "EUR003M"
		, "3"
		, QDate(2014, 9, 15)
		, 0
		, 80000000.0
		, 0.0
		, 100.0
		, 1.0
		, "Mtge"
		, static_cast<DayCountConvention>(102)
		);
	TempCoup.clear(); TempCoup << "41";
	TempUnit.AddTranche(
		"BBVAL 2007-1 B"
		, "ES0314209026"
		, 2
		, 82500000.0
		, "EUR"
		, 82500000.0
		, TempFlowType
		, TempCoup
		, TempRef
		, QDate(2014, 8, 26)
		, "EUR003M"
		, "3"
		, QDate(2014, 9, 15)
		, 0
		, 80000000.0
		, 0.0
		, 100.0
		, 1.0
		, "Mtge"
		, static_cast<DayCountConvention>(102)
		);
	TempCoup.clear(); TempCoup << "80";
	TempUnit.AddTranche(
		"BBVAL 2007-1 C"
		, "ES0314209034"
		, 3
		, 61300000.0
		, "EUR"
		, 61300000.0
		, TempFlowType
		, TempCoup
		, TempRef
		, QDate(2014, 8, 26)
		, "EUR003M"
		, "3"
		, QDate(2014, 9, 15)
		, 0
		, 1.0
		, 0.0
		, 100.0
		, 1.0
		, "Mtge"
		, static_cast<DayCountConvention>(102)
		);
	TempUnit.GetBaseRatesDatabase(ConstantBaseRateTable());
	//Waterfall
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_SeniorExpenses, 0, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_SeniorFees, 0, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, 1, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, 1, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTestPrinc, 1, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, 2, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredInterest, 2, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredPrinc, 2, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, 2, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTestPrinc, 2, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, 3, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredInterest, 3, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredPrinc, 3, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, 3, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTestPrinc, 3, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_ReserveReplenish, 1, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_ReserveReplenish, 1, 2, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Excess, 0, 0, 0.0);


	//Other Inputs
	TempUnit.SetDealName("BBVAL 2007-1");
	TempUnit.SetStartingDeferredJunFees(0);
	TempUnit.SetGICinterest("0");
	TempUnit.SetSeniorExpenses("0");
	TempUnit.SetSeniorFees("0");
	TempUnit.SetJuniorFees("0");
	TempUnit.SetSeniorExpensesFixed("0");
	TempUnit.SetSeniorFeesFixed("0");
	TempUnit.SetJuniorFeesFixed("0");
	TempUnit.SetJuniorFeesCoupon(0.0);
	TempUnit.SetPaymentFrequency("3");
	TempUnit.SetFirstIPDdate(QDate(2014, 11, 26));
	TempUnit.SetLastIPDdate(QDate(2014, 8, 26));
	TempUnit.SetUseTurbo(true);
	TempUnit.SetCCCcurve("0");
	TempUnit.SetCCChaircut(0.5);
	TempUnit.SetCCCTestLimit(0.1);
	TempUnit.SetPoolValueAtCall(100.0);

	//Reserve Fund
	TempUnit.AddReserveFund("2500000000", "0.0165", "0", "0", 0.0, 0, true);
	TempUnit.SetCumulativeReserves(false);

	TempUnit.SetBaseCaseToCall(false);
	TempUnit.SetSaveBaseCase(true);
	TempUnit.SetupReinvestmentTest(QDate(2009, 5, 26), 1.0, 0.0, 0.0, 0.0, 0.0);
	TempUnit.SetupReinvBond("1", "4.79", "5", "80", "5", "1", "I", "100", "0", "EUR003M", "0", "0", "0");
	TempUnit.SetSchedPrincAvailable(/*16018445.0*/0.0);
	TempUnit.SetInterestAvailable(0.0);
	TempUnit.SetPoolCutOff(QDate(2014, 7, 31));
	TempUnit.SetRunCall(false);
	TempUnit.SetFolderPath("C:/Temp");

	TempUnit.Calculate();
}

void ConsoleTestObj::PrintOutput() {
	PrintToTempFile("Loans",TempUnit.GetStructure().GetCalculatedMtgPayments().ToString());
	for (int i = 0; i < TempUnit.GetStructure().GetTranchesCount(); i++) {
		PrintToTempFile(TempUnit.GetStructure().GetTranche(i)->GetTrancheName(), TempUnit.GetStructure().GetTranche(i)->GetCashFlow().ToString());
	}
}
