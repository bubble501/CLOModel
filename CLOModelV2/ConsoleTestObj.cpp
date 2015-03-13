#include "ConsoleTestObj.h"
#include "StressTest.h"
#include <QSharedPointer>
#include "ScenarioApplier.h"
ConsoleTestObj::ConsoleTestObj(QObject *parent)
	: QObject(parent)
	, TempStress(nullptr)
	, Tested(nullptr)
{
	connect(&TempUnit, &CentralUnit::CalculationFinished, this, &ConsoleTestObj::PrintOutput);
	BBVALTest();
	//SlateTest();
	//TestApplier();
	//TestStressTest();
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
	SingleLease.SetProperty("PrepayMultiplier","0");
	SingleLease.SetProperty("LossMultiplier", "0");

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
		, "1"
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
        , QList<QString>() << "102"
		);
	TempCoup.clear(); TempCoup << "20";
	TempUnit.AddTranche(
		"BBVAL 2007-1 A2"
		, "ES0314209018"
		, "1"
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
        , QList<QString>() << "102"
		);
	TempCoup.clear(); TempCoup << "41";
	TempUnit.AddTranche(
		"BBVAL 2007-1 B"
		, "ES0314209026"
		, "2"
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
        , QList<QString>() << "102"
		);
	TempCoup.clear(); TempCoup << "80";
	TempUnit.AddTranche(
		"BBVAL 2007-1 C"
		, "ES0314209034"
		, "3"
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
		, QList<QString>() << "102"
		);
	TempUnit.GetBaseRatesDatabase(ConstantBaseRateTable());
	//Waterfall
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_SeniorExpenses,"","","","","","","1","","","","","",static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_SeniorFees, "", "", "", "", "", "", "1", "", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, "1", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
	//TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, "1", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, "2", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredInterest, "2", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredInterest, "2", "1", "", "", "", "", "2", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	//TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, 2, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, "3", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredInterest, "3", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredInterest, "3", "1", "", "", "", "", "2", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	//TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, 3, 0, 0.0);
	/*TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_ReserveReplenish, 1, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_ReserveReplenish, 1, 2, 0.0);*/
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Principal, "1", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Principal, "2", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Principal, "3", "1", "", "", "", "", "1", "1", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Excess, "", "", "", "", "", "", "3", "", "", "", "", "", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Invalid));


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
	TempUnit.SetCCCcurve("0");
	TempUnit.SetCCChaircut(0.5);
	TempUnit.SetCCCTestLimit(0.1);
	TempUnit.SetPoolValueAtCall("100");

	//Reserve Fund
	TempUnit.AddReserveFund("2500000000", "0.0165", "0", "0", 0.0, 0, true);
	TempUnit.SetCumulativeReserves(false);

	TempUnit.SetBaseCaseToCall(false);
	TempUnit.SetSaveBaseCase(true);
	TempUnit.SetReinvestementPeriod(QDate(2009, 5, 26));
	TempUnit.SetupReinvBond("1", "4.79", "5", "80", "5", "1", "I", "100", "0","1", "EUR003M", "0", "0", "0");
	TempUnit.SetSchedPrincAvailable(/*16018445.0*/0.0);
	TempUnit.SetInterestAvailable(0.0);
	TempUnit.SetPoolCutOff(QDate(2014, 7, 31));
	TempUnit.SetRunCall(false);
	TempUnit.SetFolderPath("C:/Temp");

	TempUnit.Calculate();
}

void ConsoleTestObj::PrintOutput() {
	if (!Tested) return;
	const QHash<qint32, MtgCashFlow*>& res = Tested->GetResults();
	MtgCashFlow TotRes;
	for (QHash<qint32, MtgCashFlow*>::const_iterator i = res.constBegin(); i != res.constEnd(); ++i) {
		TotRes.AddFlow(*(i.value()));
	}
	PrintToTempFile("ApplierRes", TotRes.ToString(), false);
}


void ConsoleTestObj::SlateTest() {


	//Mtgs Pool
	Mortgage SingleLease;
	SingleLease.SetFloatingRateBase("LIBOR");
	SingleLease.SetPaymentFreq("1");

	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(70035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(87579.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(120995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2023, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(89863.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91350.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(35000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(468685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(137060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(25314.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(72139.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(263715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(445366.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(138055.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(123000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(245995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(91516.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2023, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(57656.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(79966.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(211406.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92903.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.54 31S 4.47"); SingleLease.SetSize(114275.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(89530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(87673.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(121780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(189030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(136030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(389375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(119530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(78750.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(126655.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(118935.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(870929.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76910.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(288995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2020, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(327280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(73405.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(115491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(98435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(122030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(226030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(162245.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(182530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(416830.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(59705.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2019, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(77175.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(93480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(216991.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(333030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(134030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(134894.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(98962.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(97955.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(109162.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(126879.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(140519.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(48755.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(106610.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(63599.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(251030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(228410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(93875.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(76160.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(65344.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108133.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(119191.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(142254.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(94745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(307530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.59 24S 4.47"); SingleLease.SetSize(242435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(135430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(98997.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(265715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(94922.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(131030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(256047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(115347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(97475.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(163010.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(85260.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(57890.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(334441.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(122995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2025, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(111685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(284985.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2020, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(99000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(84597.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92603.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2021, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117500.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(117780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(443530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 24S 4.47"); SingleLease.SetSize(114835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(476530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(165683.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83265.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(101535.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(364745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2022, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(236030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(75973.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(97666.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2023, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(158995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(77491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(40835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(264280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(127164.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(111410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(83060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 32S 4.47"); SingleLease.SetSize(431530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(155130.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(103047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(140995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(346530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2019, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(123375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(114030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(118388.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(383995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(263530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(140815.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108098.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(112091.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(181030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(125641.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2022, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.06 23S 4.47"); SingleLease.SetSize(96995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(145180.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(204015.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(425860.19); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 22S 4.48"); SingleLease.SetSize(449901.52); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 22S 4.47"); SingleLease.SetSize(159081.20); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(121209.18); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(246183.28); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(412285.61); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(357485.07); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(298684.99); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(79810.78); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(85069.04); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(269186.65); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(69246.06); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 22S 4.47"); SingleLease.SetSize(238421.44); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(77975.06); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 6, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(244805.42); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(313062.50); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(110749.23); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(94790.88); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(192703.12); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(111478.28); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(91170.46); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(127995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2041, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(263430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(79769.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(126725.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.54 31S 4.47"); SingleLease.SetSize(97104.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(108655.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(94030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(257175.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190342.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(302855.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(385030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(97020.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(153030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(147280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(468425.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(233315.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(140995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(404230.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(126515.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2022, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(97030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(231430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.10 23S 4.47"); SingleLease.SetSize(233315.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.80 23S 4.47"); SingleLease.SetSize(529530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91385.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(79765.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(126515.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(201030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 16S 4.98"); SingleLease.SetSize(93806.43); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.22 16S 4.98"); SingleLease.SetSize(88331.32); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 17S 4.98"); SingleLease.SetSize(114108.81); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 17S 4.98"); SingleLease.SetSize(92125.37); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 17S 4.98"); SingleLease.SetSize(64001.11); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 17S 4.98"); SingleLease.SetSize(93865.39); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.22 17S 4.98"); SingleLease.SetSize(105876.16); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.62 17S 4.98"); SingleLease.SetSize(71764.22); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 17S 4.98"); SingleLease.SetSize(171771.81); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.39 14S 4.98"); SingleLease.SetSize(91537.70); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 17S 4.98"); SingleLease.SetSize(159630.70); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("4.22 17S 4.98"); SingleLease.SetSize(60526.25); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2042, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 17S 4.98"); SingleLease.SetSize(183107.85); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 17S 4.98"); SingleLease.SetSize(164012.94); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 17S 4.98"); SingleLease.SetSize(145502.44); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 18S 4.98"); SingleLease.SetSize(118519.47); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 18S 4.98"); SingleLease.SetSize(225257.90); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 18S 4.98"); SingleLease.SetSize(210078.70); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 18S 4.98"); SingleLease.SetSize(306466.16); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 18S 4.98"); SingleLease.SetSize(131745.47); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 18S 4.98"); SingleLease.SetSize(57286.11); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 18S 4.98"); SingleLease.SetSize(147291.84); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(131723.34); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 18S 4.98"); SingleLease.SetSize(67577.50); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(270365.59); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2023, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(70197.29); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 18S 4.98"); SingleLease.SetSize(89537.41); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 18S 4.48"); SingleLease.SetSize(225493.21); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 18S 4.98"); SingleLease.SetSize(114861.48); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 18S 4.98"); SingleLease.SetSize(81737.82); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.39 14S 4.98"); SingleLease.SetSize(87419.46); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 19S 4.48"); SingleLease.SetSize(274635.50); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 19S 4.48"); SingleLease.SetSize(163795.75); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 19S 4.98"); SingleLease.SetSize(114188.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(128363.92); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.62 19S 4.98"); SingleLease.SetSize(228853.92); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 19S 4.48"); SingleLease.SetSize(50959.71); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(94607.01); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(122806.36); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 19S 4.48"); SingleLease.SetSize(120777.09); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(211263.05); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("4.02 19S 4.98"); SingleLease.SetSize(79467.28); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 19S 4.98"); SingleLease.SetSize(198248.49); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(152560.06); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2020, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(222841.33); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(118623.66); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(82929.34); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 19S 4.48"); SingleLease.SetSize(107188.28); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 19S 4.48"); SingleLease.SetSize(143038.98); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(206572.51); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(137986.13); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2021, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.22 20S 4.98"); SingleLease.SetSize(73467.95); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 20S 4.48"); SingleLease.SetSize(300982.02); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 20S 4.48"); SingleLease.SetSize(340976.02); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(79719.11); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2021, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(154077.47); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(158452.89); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(233269.08); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(92780.14); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(113905.96); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(96680.63); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(106183.23); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.02 20S 4.98"); SingleLease.SetSize(66831.29); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(206556.72); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 20S 4.48"); SingleLease.SetSize(314477.90); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(114813.36); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.75 19S 4.98"); SingleLease.SetSize(138434.32); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 20S 4.98"); SingleLease.SetSize(140523.53); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("4.22 20S 4.98"); SingleLease.SetSize(60253.32); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(72067.86); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2019, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(199252.55); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("4.02 20S 4.98"); SingleLease.SetSize(77115.84); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.75 19S 4.98"); SingleLease.SetSize(85456.70); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.62 20S 4.98"); SingleLease.SetSize(71036.13); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.75 19S 4.98"); SingleLease.SetSize(85456.84); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(172605.53); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.22 20S 4.98"); SingleLease.SetSize(69593.23); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.22 20S 4.98"); SingleLease.SetSize(65109.99); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 20S 4.48"); SingleLease.SetSize(209007.02); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 20S 4.48"); SingleLease.SetSize(160628.07); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(247828.81); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(240420.53); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.10 20S 4.47"); SingleLease.SetSize(105036.01); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(57208.49); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(101651.75); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(104993.57); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(83091.39); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 21S 4.98"); SingleLease.SetSize(145991.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 21S 4.48"); SingleLease.SetSize(113530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(93995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.62 21S 4.98"); SingleLease.SetSize(71435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2026, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 21S 4.48"); SingleLease.SetSize(444530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(233195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 21S 4.48"); SingleLease.SetSize(258730.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(170520.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.30 21S 4.47"); SingleLease.SetSize(81035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(75280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.10 21S 4.47"); SingleLease.SetSize(252553.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2024, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(261410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 21S 4.48"); SingleLease.SetSize(83530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(187097.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.39 14S 4.98"); SingleLease.SetSize(209135.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.39 14S 4.98"); SingleLease.SetSize(91835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(93995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(266030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.80 21S 4.47"); SingleLease.SetSize(282245.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(61235.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(181030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.54 31S 4.47"); SingleLease.SetSize(73475.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.00 21S 4.47"); SingleLease.SetSize(204035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.15 21S 4.47"); SingleLease.SetSize(102535.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.80 21S 4.47"); SingleLease.SetSize(300995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(116882.02); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2044, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(233530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(201030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(94162.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.21 21S 4.48"); SingleLease.SetSize(99280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.75 19S 4.98"); SingleLease.SetSize(85715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(68530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2023, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 21S 4.48"); SingleLease.SetSize(141030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2040, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(167510.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(136030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2042, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.55 17S 4.98"); SingleLease.SetSize(130030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(125580.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(269315.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(76910.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(164901.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(82936.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.80 21S 4.47"); SingleLease.SetSize(100000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(158530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2025, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(109000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 21S 4.48"); SingleLease.SetSize(158495.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(70035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(87579.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(120995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(89863.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91350.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(35000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(468685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(137060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(25314.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 2, 28)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(72139.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(263715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(445366.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(138055.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(123000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(245995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(91516.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(57656.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(79966.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(211406.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92903.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.54 31S 4.47"); SingleLease.SetSize(114275.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(89530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(87673.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(121780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(189030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(136030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(389375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(119530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(78750.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(126655.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(118935.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(870929.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76910.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(288995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(327280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 12, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(73405.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(115491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(98435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(122030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(226030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(162245.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(182530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(416830.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(59705.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(77175.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(93480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(216991.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(333030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(134030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(134894.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(98962.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(97955.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(109162.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(126879.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(140519.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(48755.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(106610.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(63599.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(251030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(228410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(93875.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(76160.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(65344.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108133.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(119191.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(142254.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(94745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(307530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.59 24S 4.47"); SingleLease.SetSize(242435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(135430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(98997.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(265715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(94922.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(131030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 12, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(256047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(115347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(97475.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(163010.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(85260.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(57890.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(334441.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(122995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(111685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(284985.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(99000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(84597.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92603.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117500.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(117780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(443530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 24S 4.47"); SingleLease.SetSize(114835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(476530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(165683.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83265.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(101535.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(364745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(236030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(75973.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(97666.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(158995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(77491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(40835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(264280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(173930.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(127164.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(111410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(83060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 32S 4.47"); SingleLease.SetSize(431530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(155130.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(103047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(140995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(346530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 10, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(123375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(114030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(118388.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(383995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(263530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(140815.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108098.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(112091.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(181030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(125641.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.06 23S 4.47"); SingleLease.SetSize(96995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(145180.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(204015.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(97030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(231430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.10 23S 4.47"); SingleLease.SetSize(233315.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.80 23S 4.47"); SingleLease.SetSize(529530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91385.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(79765.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(126515.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(201030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(70035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(87579.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(120995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(89863.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91350.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(35000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(468685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(137060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(25314.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(72139.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(263715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(445366.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(138055.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(123000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(245995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(91516.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(57656.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(79966.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(211406.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92903.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.54 31S 4.47"); SingleLease.SetSize(114275.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(89530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(87673.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(121780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(189030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(136030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(389375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(119530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(78750.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(126655.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(118935.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(870929.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76910.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(288995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(327280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(73405.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(115491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(98435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(122030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(226030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(162245.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(182530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(416830.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(59705.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(77175.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(93480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(216991.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(333030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(134030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(134894.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(98962.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(97955.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(109162.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(126879.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(140519.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(48755.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(106610.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(63599.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(251030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(228410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(93875.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(76160.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(65344.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108133.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(119191.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(142254.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(94745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(307530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.59 24S 4.47"); SingleLease.SetSize(242435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(135430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(98997.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(265715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(94922.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(131030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(256047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(115347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(97475.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(163010.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(85260.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(57890.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(334441.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(122995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(111685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(284985.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(99000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(84597.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92603.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117500.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(117780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(443530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 24S 4.47"); SingleLease.SetSize(114835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(476530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(165683.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83265.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(101535.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(364745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(236030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(75973.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(97666.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 12, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(158995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(77491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(40835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(264280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(173930.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(127164.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(111410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(83060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 32S 4.47"); SingleLease.SetSize(431530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(155130.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(103047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(140995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(346530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 10, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(123375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(114030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(118388.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 10, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(383995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(263530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(140815.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108098.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(112091.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(181030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(125641.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.06 23S 4.47"); SingleLease.SetSize(96995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(145180.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(204015.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(97030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(231430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.10 23S 4.47"); SingleLease.SetSize(233315.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.80 23S 4.47"); SingleLease.SetSize(529530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91385.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(79765.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(126515.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(201030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(70035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(87579.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(120995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(89863.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91350.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(35000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(468685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(137060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(25314.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(72139.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(263715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(445366.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(138055.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(123000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(245995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(91516.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(57656.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(79966.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(211406.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92903.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.54 31S 4.47"); SingleLease.SetSize(114275.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(89530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(87673.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(121780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(189030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(136030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(389375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(119530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(78750.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(126655.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(118935.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(870929.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76910.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(288995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(327280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(73405.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(115491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(98435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(122030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(226030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(162245.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 10, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(182530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(416830.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(59705.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(77175.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(93480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(216991.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(333030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(134030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(134894.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(98962.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(97955.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(109162.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(126879.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(140519.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(48755.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(106610.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(63599.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(251030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(228410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(93875.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(76160.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(65344.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108133.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(119191.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(142254.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(94745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(307530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.59 24S 4.47"); SingleLease.SetSize(242435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(135430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(98997.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(265715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(94922.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(131030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 12, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(256047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(115347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(97475.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(163010.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(85260.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(57890.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(334441.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(122995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(111685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(284985.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(99000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(84597.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92603.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117500.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(117780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(443530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 24S 4.47"); SingleLease.SetSize(114835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(476530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(165683.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83265.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(101535.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(364745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(236030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(75973.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(97666.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 9, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(158995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(77491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(40835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(264280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(173930.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(127164.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(111410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(83060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 32S 4.47"); SingleLease.SetSize(431530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(155130.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(103047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(140995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(346530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(123375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(114030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(118388.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(383995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(263530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(140815.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108098.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(112091.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(181030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(125641.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.06 23S 4.47"); SingleLease.SetSize(96995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(145180.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(204015.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(97030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(231430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.10 23S 4.47"); SingleLease.SetSize(233315.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.80 23S 4.47"); SingleLease.SetSize(529530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91385.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(79765.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(126515.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(201030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(70035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(87579.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(120995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(89863.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91350.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(35000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(468685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(137060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(25314.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(72139.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(263715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(445366.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(138055.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(123000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(245995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(91516.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(57656.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(79966.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(211406.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92903.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.54 31S 4.47"); SingleLease.SetSize(114275.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(89530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(87673.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(121780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(189030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(136030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(389375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(119530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(78750.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(126655.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(118935.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(870929.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76910.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(288995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(327280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(73405.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(115491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(98435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(122030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(226030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(162245.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(182530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(416830.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(59705.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(77175.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(93480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(216991.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(333030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(134030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(134894.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(98962.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(97955.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(109162.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(126879.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(140519.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(48755.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(106610.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(63599.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(251030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(228410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(93875.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(76160.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(65344.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108133.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(119191.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(142254.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(94745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(307530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.59 24S 4.47"); SingleLease.SetSize(242435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(135430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(98997.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(265715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(94922.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(131030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(256047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(115347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(97475.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(163010.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(85260.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(57890.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(334441.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(122995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(111685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(284985.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(99000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(84597.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92603.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117500.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(117780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(443530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 24S 4.47"); SingleLease.SetSize(114835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(476530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(165683.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83265.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(101535.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(364745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(236030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(75973.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(97666.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 11, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(158995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(77491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(40835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(264280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(173930.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(127164.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(111410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(83060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 32S 4.47"); SingleLease.SetSize(431530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(155130.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(103047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(140995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(346530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(123375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(114030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(118388.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 8, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(383995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(263530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(140815.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108098.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(112091.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(181030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(125641.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.06 23S 4.47"); SingleLease.SetSize(96995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 10, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(145180.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(204015.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(97030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(231430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.10 23S 4.47"); SingleLease.SetSize(233315.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.80 23S 4.47"); SingleLease.SetSize(529530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91385.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(79765.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(188530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(215280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(126515.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(201030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.21 23S 4.48"); SingleLease.SetSize(57280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(70035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(87579.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(120995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(89863.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(91350.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 2, 28)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(35000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(468685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(137060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(25314.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(72139.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(263715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(445366.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.56 23S 4.47"); SingleLease.SetSize(138055.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(123000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(245995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(91516.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(57656.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(79966.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(211406.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92903.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.54 31S 4.47"); SingleLease.SetSize(114275.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(89530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(87673.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(121780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(189030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(136030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(389375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(119530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(78750.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(126655.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.47"); SingleLease.SetSize(118935.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.01"); SingleLease.SetSize(870929.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(76910.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(207280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(288995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(327280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(73405.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(115491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.89 31S 4.47"); SingleLease.SetSize(98435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.15 19S 4.98"); SingleLease.SetSize(122030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(226030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(162245.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 2, 28)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(182530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(416830.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(59705.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(102035.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(77175.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.31 23S 4.47"); SingleLease.SetSize(93480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(216991.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(333030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(134030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(134894.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(98962.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.49 19S 4.47"); SingleLease.SetSize(97955.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(109162.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(126879.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(140519.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(48755.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(106610.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(63599.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(251030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(228410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.01 23S 4.47"); SingleLease.SetSize(93875.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(76160.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(65344.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108133.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(119191.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(142254.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(94745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(307530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.59 24S 4.47"); SingleLease.SetSize(242435.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(213185.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83773.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(135430.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(98997.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(265715.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(94922.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(131030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(199780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 4, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(256047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(115347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(97475.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 2, 29)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(163010.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(85260.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(57890.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(334441.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(122995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(111685.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(284985.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(173480.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(96094.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 7, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(99000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(84597.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(92603.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117500.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(117780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(443530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 24S 4.47"); SingleLease.SetSize(114835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(117280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(476530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(165683.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(83265.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(101535.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(364745.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.29 31S 4.47"); SingleLease.SetSize(68547.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(236030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2036, 4, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.11 23S 4.47"); SingleLease.SetSize(75973.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(97666.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.22 23S 4.48"); SingleLease.SetSize(72280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 5, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(158995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(92285.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(77491.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(40835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(264280.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 11, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(173930.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(127164.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(111410.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(83060.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.74 32S 4.47"); SingleLease.SetSize(431530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(155130.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("1.64 19S 4.47"); SingleLease.SetSize(103047.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2039, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(140995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(346530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(91030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(123375.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2034, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.59 20S 4.47"); SingleLease.SetSize(114030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.16 23S 4.47"); SingleLease.SetSize(118388.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 9, 30)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.79 17S 4.48"); SingleLease.SetSize(383995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2029, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.81 23S 4.47"); SingleLease.SetSize(263530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(140815.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2035, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(108098.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(112091.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.03 23S 4.98"); SingleLease.SetSize(64295.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 2, 28)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(181030.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 6, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.56 23S 4.47"); SingleLease.SetSize(125641.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2030, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(259780.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("4.06 23S 4.47"); SingleLease.SetSize(96995.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2033, 8, 31)); SingleLease.SetAnnuity("A"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(145180.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 5, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(121835.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2028, 3, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(204015.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2032, 9, 30)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(57129.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 1, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(185000.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2027, 12, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.39 19S 4.47"); SingleLease.SetSize(458530.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2037, 8, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.04 19S 4.47"); SingleLease.SetSize(190347.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2038, 7, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("2.79 19S 4.47"); SingleLease.SetSize(272195.00); TempUnit.AddLoan(SingleLease);
	SingleLease.SetMaturityDate(QDate(2031, 10, 31)); SingleLease.SetAnnuity("I"); SingleLease.SetInterest("3.29 19S 4.98"); SingleLease.SetSize(80360.00); TempUnit.AddLoan(SingleLease);

	
	

	//Tranches
	QList<Tranche::TrancheInterestType> TempFlowType; TempFlowType << Tranche::TrancheInterestType::FloatingInterest;
	QList<QString> TempRef; TempRef << "LIBOR";

	QList<QString> TempCoup;
	TempCoup << "A 08/01/2014 85 60S 170";
	TempUnit.AddTranche(
		"SLATE A"
		, ""
		, "1"
		, 164000352.477
		, "GBP"
		, 164000352.477
		, TempFlowType
		, TempCoup
		, TempRef
		, QDate(2014, 8, 1)
		, "LIBOR"
		, "3"
		, QDate(2014, 8, 1)
		, 0
		, 100.0
		, 0.0
		, 100.0
		, 1.0
		, "Mtge"
        , QList<QString>() << "102"
		);
	TempCoup.clear(); TempCoup << "100";
	TempUnit.AddTranche(
		"SLATE Z"
		, ""
		, "2"
		, 36000077
		, "GBP"
		, 36000077
		, TempFlowType
		, TempCoup
		, TempRef
		, QDate(2014, 8, 1)
		, "LIBOR"
		, "3"
		, QDate(2014, 8, 1)
		, 0
		, 100.0
		, 0.0
		, 100.0
		, 1.0
		, "Mtge"
        , QList<QString>() << "102"
		);
	TempCoup.clear(); TempCoup << "250";
	TempUnit.AddTranche(
		"Sub Loan"
		, ""
		, "3"
		, 4000000.0
		, "EUR"
		, 4000000.0
		, TempFlowType
		, TempCoup
		, TempRef
		, QDate(2014, 8, 1)
		, "LIBOR"
		, "3"
		, QDate(2014, 8, 1)
		, 0
		, 99999999.0
		, 0.0
		, 100.0
		, 1.0
		, "Mtge"
        , QList<QString>() << "102"
		);
	/*ConstantBaseRateTable LiborCurve;
	LiborCurve.SetValue("LIBOR", 0.0053);*/
	ForwardBaseRateTable LiborCurve;
	QList<QDate> LiborDates;
	QList<double> LiborValues;
	LiborDates.append(QDate(2014, 8, 1)); LiborValues.append(0.0053);
	LiborDates.append(QDate(2014, 9, 1)); LiborValues.append(0.0054);
	LiborDates.append(QDate(2014, 10, 1)); LiborValues.append(0.0055);
	LiborDates.append(QDate(2014, 11, 1)); LiborValues.append(0.0057);
	LiborDates.append(QDate(2014, 12, 1)); LiborValues.append(0.0058);
	LiborDates.append(QDate(2015, 1, 1)); LiborValues.append(0.0060);
	LiborDates.append(QDate(2015, 2, 1)); LiborValues.append(0.0061);
	LiborDates.append(QDate(2015, 3, 1)); LiborValues.append(0.0066);
	LiborDates.append(QDate(2015, 4, 1)); LiborValues.append(0.0071);
	LiborDates.append(QDate(2015, 5, 1)); LiborValues.append(0.0075);
	LiborDates.append(QDate(2015, 6, 1)); LiborValues.append(0.0080);
	LiborDates.append(QDate(2015, 7, 1)); LiborValues.append(0.0085);
	LiborDates.append(QDate(2015, 8, 1)); LiborValues.append(0.0090);
	LiborDates.append(QDate(2015, 9, 1)); LiborValues.append(0.0088);
	LiborDates.append(QDate(2015, 10, 1)); LiborValues.append(0.0087);
	LiborDates.append(QDate(2015, 11, 1)); LiborValues.append(0.0086);
	LiborDates.append(QDate(2015, 12, 1)); LiborValues.append(0.0085);
	LiborDates.append(QDate(2016, 1, 1)); LiborValues.append(0.0084);
	LiborDates.append(QDate(2016, 2, 1)); LiborValues.append(0.0083);
	LiborDates.append(QDate(2016, 3, 1)); LiborValues.append(0.0086);
	LiborDates.append(QDate(2016, 4, 1)); LiborValues.append(0.0088);
	LiborDates.append(QDate(2016, 5, 1)); LiborValues.append(0.0091);
	LiborDates.append(QDate(2016, 6, 1)); LiborValues.append(0.0093);
	LiborDates.append(QDate(2016, 7, 1)); LiborValues.append(0.0096);
	LiborDates.append(QDate(2016, 8, 1)); LiborValues.append(0.0099);
	LiborDates.append(QDate(2016, 9, 1)); LiborValues.append(0.0102);
	LiborDates.append(QDate(2016, 10, 1)); LiborValues.append(0.0105);
	LiborDates.append(QDate(2016, 11, 1)); LiborValues.append(0.0108);
	LiborDates.append(QDate(2016, 12, 1)); LiborValues.append(0.0111);
	LiborDates.append(QDate(2017, 1, 1)); LiborValues.append(0.0114);
	LiborDates.append(QDate(2017, 2, 1)); LiborValues.append(0.0117);
	LiborDates.append(QDate(2017, 3, 1)); LiborValues.append(0.0120);
	LiborDates.append(QDate(2017, 4, 1)); LiborValues.append(0.0123);
	LiborDates.append(QDate(2017, 5, 1)); LiborValues.append(0.0126);
	LiborDates.append(QDate(2017, 6, 1)); LiborValues.append(0.0130);
	LiborDates.append(QDate(2017, 7, 1)); LiborValues.append(0.0133);
	LiborDates.append(QDate(2017, 8, 1)); LiborValues.append(0.0136);
	LiborDates.append(QDate(2017, 9, 1)); LiborValues.append(0.0138);
	LiborDates.append(QDate(2017, 10, 1)); LiborValues.append(0.0141);
	LiborDates.append(QDate(2017, 11, 1)); LiborValues.append(0.0144);
	LiborDates.append(QDate(2017, 12, 1)); LiborValues.append(0.0147);
	LiborDates.append(QDate(2018, 1, 1)); LiborValues.append(0.0149);
	LiborDates.append(QDate(2018, 2, 1)); LiborValues.append(0.0152);
	LiborDates.append(QDate(2018, 3, 1)); LiborValues.append(0.0155);
	LiborDates.append(QDate(2018, 4, 1)); LiborValues.append(0.0157);
	LiborDates.append(QDate(2018, 5, 1)); LiborValues.append(0.0160);
	LiborDates.append(QDate(2018, 6, 1)); LiborValues.append(0.0163);
	LiborDates.append(QDate(2018, 7, 1)); LiborValues.append(0.0166);
	LiborDates.append(QDate(2018, 8, 1)); LiborValues.append(0.0168);
	LiborDates.append(QDate(2018, 9, 1)); LiborValues.append(0.0171);
	LiborDates.append(QDate(2018, 10, 1)); LiborValues.append(0.0173);
	LiborDates.append(QDate(2018, 11, 1)); LiborValues.append(0.0175);
	LiborDates.append(QDate(2018, 12, 1)); LiborValues.append(0.0178);
	LiborDates.append(QDate(2019, 1, 1)); LiborValues.append(0.0180);
	LiborDates.append(QDate(2019, 2, 1)); LiborValues.append(0.0182);
	LiborDates.append(QDate(2019, 3, 1)); LiborValues.append(0.0184);
	LiborDates.append(QDate(2019, 4, 1)); LiborValues.append(0.0187);
	LiborDates.append(QDate(2019, 5, 1)); LiborValues.append(0.0189);
	LiborDates.append(QDate(2019, 6, 1)); LiborValues.append(0.0191);
	LiborDates.append(QDate(2019, 7, 1)); LiborValues.append(0.0194);
	LiborDates.append(QDate(2019, 8, 1)); LiborValues.append(0.0196);
	LiborDates.append(QDate(2019, 9, 1)); LiborValues.append(0.0198);
	LiborDates.append(QDate(2019, 10, 1)); LiborValues.append(0.0200);
	LiborDates.append(QDate(2019, 11, 1)); LiborValues.append(0.0202);
	LiborDates.append(QDate(2019, 12, 1)); LiborValues.append(0.0203);
	LiborDates.append(QDate(2020, 1, 1)); LiborValues.append(0.0205);
	LiborDates.append(QDate(2020, 2, 1)); LiborValues.append(0.0207);
	LiborDates.append(QDate(2020, 3, 1)); LiborValues.append(0.0209);
	LiborDates.append(QDate(2020, 4, 1)); LiborValues.append(0.0211);
	LiborDates.append(QDate(2020, 5, 1)); LiborValues.append(0.0213);
	LiborDates.append(QDate(2020, 6, 1)); LiborValues.append(0.0215);
	LiborDates.append(QDate(2020, 7, 1)); LiborValues.append(0.0217);
	LiborDates.append(QDate(2020, 8, 1)); LiborValues.append(0.0219);
	LiborDates.append(QDate(2020, 9, 1)); LiborValues.append(0.0220);
	LiborDates.append(QDate(2020, 10, 1)); LiborValues.append(0.0222);
	LiborDates.append(QDate(2020, 11, 1)); LiborValues.append(0.0223);
	LiborDates.append(QDate(2020, 12, 1)); LiborValues.append(0.0225);
	LiborDates.append(QDate(2021, 1, 1)); LiborValues.append(0.0227);
	LiborDates.append(QDate(2021, 2, 1)); LiborValues.append(0.0228);
	LiborDates.append(QDate(2021, 3, 1)); LiborValues.append(0.0230);
	LiborDates.append(QDate(2021, 4, 1)); LiborValues.append(0.0231);
	LiborDates.append(QDate(2021, 5, 1)); LiborValues.append(0.0233);
	LiborDates.append(QDate(2021, 6, 1)); LiborValues.append(0.0235);
	LiborDates.append(QDate(2021, 7, 1)); LiborValues.append(0.0236);
	LiborDates.append(QDate(2021, 8, 1)); LiborValues.append(0.0238);
	LiborDates.append(QDate(2021, 9, 1)); LiborValues.append(0.0239);
	LiborDates.append(QDate(2021, 10, 1)); LiborValues.append(0.0241);
	LiborDates.append(QDate(2021, 11, 1)); LiborValues.append(0.0242);
	LiborDates.append(QDate(2021, 12, 1)); LiborValues.append(0.0243);
	LiborDates.append(QDate(2022, 1, 1)); LiborValues.append(0.0245);
	LiborDates.append(QDate(2022, 2, 1)); LiborValues.append(0.0246);
	LiborDates.append(QDate(2022, 3, 1)); LiborValues.append(0.0247);
	LiborDates.append(QDate(2022, 4, 1)); LiborValues.append(0.0249);
	LiborDates.append(QDate(2022, 5, 1)); LiborValues.append(0.0250);
	LiborDates.append(QDate(2022, 6, 1)); LiborValues.append(0.0252);
	LiborDates.append(QDate(2022, 7, 1)); LiborValues.append(0.0253);
	LiborDates.append(QDate(2022, 8, 1)); LiborValues.append(0.0254);
	LiborDates.append(QDate(2022, 9, 1)); LiborValues.append(0.0255);
	LiborDates.append(QDate(2022, 10, 1)); LiborValues.append(0.0257);
	LiborDates.append(QDate(2022, 11, 1)); LiborValues.append(0.0258);
	LiborDates.append(QDate(2022, 12, 1)); LiborValues.append(0.0259);
	LiborDates.append(QDate(2023, 1, 1)); LiborValues.append(0.0260);
	LiborDates.append(QDate(2023, 2, 1)); LiborValues.append(0.0261);
	LiborDates.append(QDate(2023, 3, 1)); LiborValues.append(0.0262);
	LiborDates.append(QDate(2023, 4, 1)); LiborValues.append(0.0264);
	LiborDates.append(QDate(2023, 5, 1)); LiborValues.append(0.0265);
	LiborDates.append(QDate(2023, 6, 1)); LiborValues.append(0.0266);
	LiborDates.append(QDate(2023, 7, 1)); LiborValues.append(0.0267);
	LiborDates.append(QDate(2023, 8, 1)); LiborValues.append(0.0268);
	LiborDates.append(QDate(2023, 9, 1)); LiborValues.append(0.0269);
	LiborDates.append(QDate(2023, 10, 1)); LiborValues.append(0.0270);
	LiborDates.append(QDate(2023, 11, 1)); LiborValues.append(0.0271);
	LiborDates.append(QDate(2023, 12, 1)); LiborValues.append(0.0272);
	LiborDates.append(QDate(2024, 1, 1)); LiborValues.append(0.0273);
	LiborDates.append(QDate(2024, 2, 1)); LiborValues.append(0.0274);
	LiborDates.append(QDate(2024, 3, 1)); LiborValues.append(0.0275);
	LiborDates.append(QDate(2024, 4, 1)); LiborValues.append(0.0276);
	LiborDates.append(QDate(2024, 5, 1)); LiborValues.append(0.0277);
	LiborDates.append(QDate(2024, 6, 1)); LiborValues.append(0.0278);
	LiborDates.append(QDate(2024, 7, 1)); LiborValues.append(0.0279);
	LiborDates.append(QDate(2024, 8, 1)); LiborValues.append(0.0279);
	LiborCurve.SetValue("LIBOR", BloombergVector(LiborDates, LiborValues));
	
	//Waterfall
	/*TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_SeniorExpenses, 0, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_SeniorFees, 0, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, 1, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, 1, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_ReserveReplenish, 1, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, 2, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredInterest, 2, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, 2, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Interest, 3, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_DeferredInterest, 3, 1, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_OCTest, 3, 3, 1.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_juniorFees, 0, 1, 0.0);

	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_SeniorExpenses, 0, 2, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_SeniorFees, 0, 2, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_ReserveReplenish, 1, 2, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Principal, 1, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Principal, 2, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Principal, 3, 0, 0.0);
	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_juniorFees, 0, 2, 0.0);

	TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType::wst_Excess, 0, 2, 0.0);*/


	//Other Inputs
	TempUnit.SetDealName("Slate");
	TempUnit.SetStartingDeferredJunFees(0);
	TempUnit.SetGICinterest("0.28");
	TempUnit.SetSeniorExpenses("0");
	TempUnit.SetSeniorFees("0.25");
	TempUnit.SetJuniorFees("0");
	TempUnit.SetSeniorExpensesFixed("0");
	TempUnit.SetSeniorFeesFixed("0");
	TempUnit.SetJuniorFeesFixed("0");
	TempUnit.SetJuniorFeesCoupon(0.0);
	TempUnit.SetPaymentFrequency("3");
	TempUnit.SetFirstIPDdate(QDate(2014, 11, 1));
	TempUnit.SetLastIPDdate(QDate(2014, 8, 1));
	TempUnit.SetCCCcurve("1");
	TempUnit.SetCCChaircut(0.5);
	TempUnit.SetCCCTestLimit(0.5);
	TempUnit.SetPoolValueAtCall("100");
	TempUnit.SetCallDate(QDate(2019, 8, 1));

	//Reserve Fund
	TempUnit.AddReserveFund("200000430", "0.015", "0", "0", 3000006.0,1, true);
	TempUnit.SetCumulativeReserves(false);

	TempUnit.SetBaseCaseToCall(true);
	TempUnit.SetSaveBaseCase(false);
	TempUnit.SetReinvestementPeriod(QDate(2019, 8, 1));
	TempUnit.SetupReinvBond("1", "5", "0.25", "25", "5", "1", "I", "100", "0", "LIBOR", "6", "1.5", "0");
	TempUnit.SetSchedPrincAvailable(0.0);
	TempUnit.SetInterestAvailable(0.0);
	TempUnit.SetPoolCutOff(QDate(2014, 8, 1));
	TempUnit.SetRunCall(false);
	TempUnit.SetFolderPath("C:/Temp");

	TempUnit.GetBaseRatesDatabase(LiborCurve);
	TempUnit.Calculate();
}

void ConsoleTestObj::TestStressTest() {
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
		qDebug() << "Invalid Version\n";
		file.close();
		return;
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

	TempStress = new StressTest(this);
	for (auto i = TempLoanCalc.GetLoans().constBegin(); i != TempLoanCalc.GetLoans().constEnd(); ++i) {
		TempStress->AddLoan(*(i.value()));
	}


	TempStress->SetStructure(TempWtf);
	QList<QString> TempList;
	TempList << "0" << "10";
	//TempList << "20" << "30";// << "40" << "50" << "60" << "70" << "80" << "90" << "100";
	foreach(const QString& tmpstr, TempList)
		TempStress->AddLSscenarios(tmpstr);
	TempList.clear();
	TempList << "0.5" << "1";
	//TempList << "1.5" << "2";// << "2.5" << "3" << "3.5" << "4" << "4.5" << "5" << "5.5" << "6" << "6.5" << "7" << "7.5" << "8" << "8.5" << "9";
	foreach(const QString& tmpstr, TempList)
		TempStress->AddCDRscenarios(tmpstr);
	TempStress->AddCPRscenarios("20 48S 5 12S 10 12S 30");
	TempStress->SetUseFastVersion(true);
	//TempStress.UseMultithread(false);
	TempStress->SetStartDate(TempWtf.GetCalculatedMtgPayments().GetDate(0));
	connect(TempStress, SIGNAL(AllFinished()), this, SLOT(PrintOutput()));
	TempStress->RunStressTest();
}

void ConsoleTestObj::TestApplier() {
	Waterfall TempWtf;
	QFile file("//synserver2/Company Share/24AM/Monitoring/Model Results/HARVT 10X.clom");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return;
	}
	{QDate Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	TempWtf.SetLoadProtocolVersion(VersionChecker);
	out >> TempWtf;
	TempWtf.CalculateTranchesCashFlows();

	/*Tested = new ScenarioApplier(this);

	Tested->SetBaseFlows(TempWtf.GetMortgagesPayments());
	AssumptionSet CurrentAss;
	CurrentAss.SetDelinqScenario("0");
	CurrentAss.SetDelinqLagScenario("0");
	CurrentAss.SetRecLagScenario("0");
	CurrentAss.SetCPRscenario("20");

	CurrentAss.SetLSscenario("10");
	CurrentAss.SetCDRscenario("1");
	Tested->AddAssumption(CurrentAss,0);
	CurrentAss.SetCDRscenario("2");
	Tested->AddAssumption(CurrentAss,1);
	CurrentAss.SetLSscenario("20");
	Tested->AddAssumption(CurrentAss,2);
	CurrentAss.SetCDRscenario("1");
	Tested->AddAssumption(CurrentAss,3);
	connect(Tested, SIGNAL(Calculated()), this, SLOT(PrintOutput()));
	connect(Tested, SIGNAL(Progress(double)), this, SLOT(ConsoleProgress(double)));
	Tested->SetSequentialComputation(true);
	Tested->StartCalculation();*/
}

void ConsoleTestObj::ConsoleProgress(double pr) {
	qDebug() << QString::number(pr*100.0, 'f', 2) << '\n';
}
