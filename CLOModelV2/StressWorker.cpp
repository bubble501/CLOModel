#include "StressWorker.h"
#include "MtgCalculator.h"
StressWorker::StressWorker(int IDx,int IDy, const QString& Xs,const QString& Ys,const QString& Cp,const QHash<qint32,Mortgage*>& Mtgs,const Waterfall& Stct,const QDate& StDt,StressTest::StressVariability Xv,StressTest::StressVariability Yv,QObject* parent)
	:QObject(parent)
	,XSpann(Xs)
	,YSpann(Ys)
	,ConstantPar(Cp)
	,LocalStructure(Stct)
	,StartDate(StDt)
	, UseFastVersion(false)
{
	StressDimension[0]=Xv;
	StressDimension[1]=Yv;
	Identity[0]=IDx;
	Identity[1]=IDy;
	Loans = new MtgCalculator();
	Loans->SetLoans(Mtgs);
	Loans->SetSequentialComputation(true);
	Loans->SetOverrideAssumptions(true);
	Loans->SetStartDate(StartDate);
	connect(Loans, &MtgCalculator::Calculated, this, &StressWorker::LoansCalculated);
}
StressWorker::StressWorker(int IDx, int IDy, const QString& Xs, const QString& Ys, const QString& Cp, const MtgCashFlow& Mtgs, const Waterfall& Stct, const QDate& StDt, StressTest::StressVariability Xv, StressTest::StressVariability Yv, QObject* parent)
:QObject(parent)
, XSpann(Xs)
, YSpann(Ys)
, ConstantPar(Cp)
, LocalStructure(Stct)
, StartDate(StDt)
, BaseCashFlows(Mtgs)
, Loans(NULL)
, UseFastVersion(true)
{
	StressDimension[0] = Xv;
	StressDimension[1] = Yv;
	Identity[0] = IDx;
	Identity[1] = IDy;
}
void StressWorker::Work() {
	if (UseFastVersion) WorkFast();
	else WorkSlow();
}
void StressWorker::WorkFast() {
	if (StressDimension[0] == StressTest::ChangingCDR) {
		if (StressDimension[1] == StressTest::ChangingLS) {
			BaseCashFlows=BaseCashFlows.ApplyScenario(ConstantPar, XSpann, YSpann);
			LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
			LocalStructure.GetReinvestmentTest().SetCDR(XSpann);
			LocalStructure.GetReinvestmentTest().SetLS(YSpann);
		}
		else {
			BaseCashFlows = BaseCashFlows.ApplyScenario(YSpann, XSpann, ConstantPar);
			LocalStructure.GetReinvestmentTest().SetCPR(YSpann);
			LocalStructure.GetReinvestmentTest().SetCDR(XSpann);
			LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
		}
	}
	else if (StressDimension[0] == StressTest::ChangingLS) {
		if (StressDimension[1] == StressTest::ChangingCDR) {
			BaseCashFlows = BaseCashFlows.ApplyScenario(ConstantPar, YSpann, XSpann);
			LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
			LocalStructure.GetReinvestmentTest().SetCDR(YSpann);
			LocalStructure.GetReinvestmentTest().SetLS(XSpann);
		}
		else {
			BaseCashFlows = BaseCashFlows.ApplyScenario(YSpann, ConstantPar, XSpann);
			LocalStructure.GetReinvestmentTest().SetCPR(YSpann);
			LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
			LocalStructure.GetReinvestmentTest().SetLS(XSpann);
		}
	}
	else if (StressDimension[0] == StressTest::ChangingCPR) {
		if (StressDimension[1] == StressTest::ChangingCDR) {
			BaseCashFlows = BaseCashFlows.ApplyScenario(XSpann, YSpann, ConstantPar);
			LocalStructure.GetReinvestmentTest().SetCPR(XSpann);
			LocalStructure.GetReinvestmentTest().SetCDR(YSpann);
			LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
		}
		else {
			BaseCashFlows = BaseCashFlows.ApplyScenario(XSpann, ConstantPar, YSpann);
			LocalStructure.GetReinvestmentTest().SetCPR(XSpann);
			LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
			LocalStructure.GetReinvestmentTest().SetLS(YSpann);
		}
	}
	LocalStructure.ResetMtgFlows();
	LocalStructure.AddMortgagesFlows(BaseCashFlows);
	if(LocalStructure.CalculateTranchesCashFlows())
		emit ScenarioCalculated(Identity[0], Identity[1], LocalStructure);
	else
		emit ScenarioCalculated(Identity[0], Identity[1], Waterfall());
}
void StressWorker::WorkSlow(){
		if(StressDimension[0]==StressTest::ChangingCDR){
			if(StressDimension[1]==StressTest::ChangingLS){
				Loans->SetCPRass(ConstantPar);
				Loans->SetCDRass(XSpann);
				Loans->SetLSass(YSpann);
				LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetCDR(XSpann);
				LocalStructure.GetReinvestmentTest().SetLS(YSpann);
				
			}
			else {
				Loans->SetCPRass(YSpann);
				Loans->SetCDRass(XSpann);
				Loans->SetLSass(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetCPR(YSpann);
				LocalStructure.GetReinvestmentTest().SetCDR(XSpann);
				LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
			}

		}
		else if(StressDimension[0]==StressTest::ChangingLS){
			if(StressDimension[1]==StressTest::ChangingCDR){
				Loans->SetCPRass(ConstantPar);
				Loans->SetCDRass(YSpann);
				Loans->SetLSass(XSpann);
				LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetCDR(YSpann);
				LocalStructure.GetReinvestmentTest().SetLS(XSpann);
			}
			else {
				Loans->SetCPRass(YSpann);
				Loans->SetCDRass(ConstantPar);
				Loans->SetLSass(XSpann);
				LocalStructure.GetReinvestmentTest().SetCPR(YSpann);
				LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetLS(XSpann);
			}
		}
		else if(StressDimension[0]==StressTest::ChangingCPR){
			if(StressDimension[1]==StressTest::ChangingCDR){
				Loans->SetCPRass(XSpann);
				Loans->SetCDRass(YSpann);
				Loans->SetLSass(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetCPR(XSpann);
				LocalStructure.GetReinvestmentTest().SetCDR(YSpann);
				LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
				
			}
			else{
				Loans->SetCPRass(XSpann);
				Loans->SetCDRass(ConstantPar);
				Loans->SetLSass(YSpann);
				LocalStructure.GetReinvestmentTest().SetCPR(XSpann);
				LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetLS(YSpann);
			}
		}
		if(!Loans->StartCalculation(true))
			emit ScenarioCalculated(Identity[0], Identity[1], Waterfall());
}

void StressWorker::LoansCalculated() {
	LocalStructure.ResetMtgFlows();
	LocalStructure.AddMortgagesFlows(Loans->GetResult());
	if (LocalStructure.CalculateTranchesCashFlows())
		emit ScenarioCalculated(Identity[0], Identity[1], LocalStructure);
	else
		emit ScenarioCalculated(Identity[0], Identity[1], Waterfall());
}
