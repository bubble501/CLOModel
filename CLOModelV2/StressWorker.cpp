#include "StressWorker.h"
StressWorker::StressWorker(int IDx,int IDy, const QString& Xs,const QString& Ys,const QString& Cp,const QList<Mortgage*>& Mtgs,const Waterfall& Stct,const QDate& StDt,StressTest::StressVariability Xv,StressTest::StressVariability Yv,QObject* parent)
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
	for(QList<Mortgage*>::const_iterator i=Mtgs.begin();i!=Mtgs.end();i++)
		Loans.append(**i);
}
StressWorker::StressWorker(int IDx, int IDy, const QString& Xs, const QString& Ys, const QString& Cp, const MtgCashFlow& Mtgs, const Waterfall& Stct, const QDate& StDt, StressTest::StressVariability Xv, StressTest::StressVariability Yv, QObject* parent)
:QObject(parent)
, XSpann(Xs)
, YSpann(Ys)
, ConstantPar(Cp)
, LocalStructure(Stct)
, StartDate(StDt)
, BaseCashFlows(Mtgs)
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
	MtgCashFlow TotalFlow;
	for(QList<Mortgage>::iterator i=Loans.begin();i!=Loans.end();i++){
		if(StressDimension[0]==StressTest::ChangingCDR){
			if(StressDimension[1]==StressTest::ChangingLS){
				i->CalculateCashFlows(StartDate,ConstantPar, XSpann, YSpann );
				if(i==Loans.begin()){
					LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
					LocalStructure.GetReinvestmentTest().SetCDR(XSpann);
					LocalStructure.GetReinvestmentTest().SetLS(YSpann);
				}
			}
			else {
				i->CalculateCashFlows(StartDate,YSpann, XSpann, ConstantPar );
				if(i==Loans.begin()){
					LocalStructure.GetReinvestmentTest().SetCPR(YSpann);
					LocalStructure.GetReinvestmentTest().SetCDR(XSpann);
					LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
				}
			}

		}
		else if(StressDimension[0]==StressTest::ChangingLS){
			if(StressDimension[1]==StressTest::ChangingCDR){
				i->CalculateCashFlows(StartDate,ConstantPar,YSpann,XSpann);
				if(i==Loans.begin()){
					LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
					LocalStructure.GetReinvestmentTest().SetCDR(YSpann);
					LocalStructure.GetReinvestmentTest().SetLS(XSpann);
				}
			}
			else {
				i->CalculateCashFlows(StartDate,YSpann, ConstantPar, XSpann );
				if(i==Loans.begin()){
					LocalStructure.GetReinvestmentTest().SetCPR(YSpann);
					LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
					LocalStructure.GetReinvestmentTest().SetLS(XSpann);
				}
			}
		}
		else if(StressDimension[0]==StressTest::ChangingCPR){
			if(StressDimension[1]==StressTest::ChangingCDR){
				i->CalculateCashFlows(StartDate,XSpann,YSpann,ConstantPar);
				if(i==Loans.begin()){
					LocalStructure.GetReinvestmentTest().SetCPR(XSpann);
					LocalStructure.GetReinvestmentTest().SetCDR(YSpann);
					LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
				}
			}
			else{
				i->CalculateCashFlows(StartDate,XSpann, ConstantPar, YSpann );
				if(i==Loans.begin()){
					LocalStructure.GetReinvestmentTest().SetCPR(XSpann);
					LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
					LocalStructure.GetReinvestmentTest().SetLS(YSpann);
				}
			}
		}
		TotalFlow+=i->GetCashFlow();
	}
	LocalStructure.ResetMtgFlows();
	LocalStructure.AddMortgagesFlows(TotalFlow);
	if(LocalStructure.CalculateTranchesCashFlows())
		emit ScenarioCalculated(Identity[0],Identity[1],LocalStructure);
	else
		emit ScenarioCalculated(Identity[0], Identity[1], LocalStructure);
}
