
#include <QCoreApplication>
#include "Tranche.h"
#include "Mortgage.h"
#include "Waterfall.h"
#include "ManCentralUnit.h"
#include "CentralUnitOverlay.h"
#include <QDate>

using namespace ManagedCLO;
int main(int argc, char *argv[])
{
	//QCoreApplication a(argc, argv);
	ManCentralUnit^ TempManaged=gcnew ManCentralUnit();
	CentralUnitOverlay* LabRat=TempManaged->Unmanaged;
	{
		Mortgage TempLoan;
		TempLoan.SetAnnuity("N");
		TempLoan.SetInterest("5");
		TempLoan.SetMaturityDate(QDate(2020,5,30));
		TempLoan.SetPaymentFreq(3);
		TempLoan.SetSize(100000);
		LabRat->AddLoan(TempLoan);
	}
	{
		Waterfall TempWaterfall;
		{
			Tranche TempTranche;
			TempTranche.SetAccruedInterest(0);
			TempTranche.SetCoupon(50);
			TempTranche.SetDefaultRefRate("EUR003M");
			TempTranche.SetInterestType(Tranche::FloatingInterest);
			TempTranche.SetLastPaymentDate(QDate(2014,2,1));
			TempTranche.SetOriginalAmount(50000);
			TempTranche.SetOutstandingAmt(50000);
			TempTranche.SetPaymentFrequency(3);
			TempTranche.SetPrice(100.0);
			TempTranche.SetProrataGroup(1);
			TempTranche.SetReferenceRate("EUR003M");
			TempTranche.SetReferenceRateValue(0.00287);
			TempTranche.SetSettlementDate(QDate(2014,3,7));
			TempTranche.SetTrancheName("FakeTranche A");
			TempWaterfall.AddTranche(TempTranche);
			TempTranche.SetTrancheName("FakeTranche B");
			TempTranche.SetProrataGroup(2);
			TempTranche.SetCoupon(70);
			TempWaterfall.AddTranche(TempTranche);
		}
		{
			WatFalPrior TempStep;
			TempStep.SetPriorityType(WatFalPrior::wst_Interest);
			TempStep.SetGroupTarget(1);
			TempWaterfall.AddStep(TempStep);
			TempStep.SetGroupTarget(2);
			TempWaterfall.AddStep(TempStep);
			TempStep.SetPriorityType(WatFalPrior::wst_Principal);
			TempStep.SetGroupTarget(1);
			TempWaterfall.AddStep(TempStep);
			TempStep.SetGroupTarget(2);
			TempWaterfall.AddStep(TempStep);
		}
		TempWaterfall.SetPaymentFrequency(3);
		TempWaterfall.SetPrincipalAvailable(0);
		TempWaterfall.SetSeniorExpenses(0);
		TempWaterfall.SetSeniorFees(0);
		TempWaterfall.SetupReinvBond("5","0","0","0","5");
		TempWaterfall.SetupReinvestmentTest(QDate(2001,2,2),0.01,0,0,0,0);
		TempWaterfall.SetUseCall(false);
		TempWaterfall.SetCCCcurve("0");
		TempWaterfall.SetCCChaircut(0.5);
		TempWaterfall.SetCCCTestLimit(0.05);
		TempWaterfall.SetFirstIPDdate(QDate(2014,3,7));
		TempWaterfall.SetInterestAvailable(0);
		TempWaterfall.SetJuniorFees(0);
		TempWaterfall.SetJuniorFeesCoupon(0);
		TempWaterfall.SetLastIPDdate(QDate(2013,12,7));
		LabRat->SetStructure(TempWaterfall);		
	}
	LabRat->Calculate();

	return 0;//return a.exec();
}
