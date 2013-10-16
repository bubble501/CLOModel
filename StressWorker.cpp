#include "StressWorker.h"
StressWorker::StressWorker(int IDx,int IDy, const QString& Xs,const QString& Ys,const QString& Cp,const QList<Mortgage*>& Mtgs,const Waterfall& Stct,const QDate& StDt,StressTest::StressVariability Xv,StressTest::StressVariability Yv,QObject* parent)
	:QObject(parent)
	,XSpann(Xs)
	,YSpann(Ys)
	,ConstantPar(Cp)
	,LocalStructure(Stct)
	,StartDate(StDt)
{
	StressDimension[0]=Xv;
	StressDimension[1]=Yv;
	Identity[0]=IDx;
	Identity[1]=IDy;
	for(QList<Mortgage*>::const_iterator i=Mtgs.begin();i!=Mtgs.end();i++)
		Loans.append(**i);
}
void StressWorker::Work(){
	MtgCashFlow TotalFlow;
	for(QList<Mortgage>::iterator i=Loans.begin();i!=Loans.end();i++){
		if(StressDimension[0]==StressTest::ChangingCDR){
			if(StressDimension[1]==StressTest::ChangingLS){
				i->CalculateCashFlows(ConstantPar,XSpann,YSpann,StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetCDR(XSpann);
				LocalStructure.GetReinvestmentTest().SetLS(YSpann);
			}
			else {
				i->CalculateCashFlows(YSpann,XSpann,ConstantPar,StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(YSpann);
				LocalStructure.GetReinvestmentTest().SetCDR(XSpann);
				LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
			}

		}
		else if(StressDimension[0]==StressTest::ChangingLS){
			if(StressDimension[1]==StressTest::ChangingCDR){
				i->CalculateCashFlows(ConstantPar,YSpann,XSpann,StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetCDR(YSpann);
				LocalStructure.GetReinvestmentTest().SetLS(XSpann);
			}
			else {
				i->CalculateCashFlows(YSpann,ConstantPar,XSpann,StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(YSpann);
				LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetLS(XSpann);
			}
		}
		else if(StressDimension[0]==StressTest::ChangingCPR){
			if(StressDimension[1]==StressTest::ChangingCDR){
				i->CalculateCashFlows(XSpann,YSpann,ConstantPar,StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(XSpann);
				LocalStructure.GetReinvestmentTest().SetCDR(YSpann);
				LocalStructure.GetReinvestmentTest().SetLS(ConstantPar);
			}
			else{
				i->CalculateCashFlows(XSpann,ConstantPar,YSpann,StartDate);
				LocalStructure.GetReinvestmentTest().SetCPR(XSpann);
				LocalStructure.GetReinvestmentTest().SetCDR(ConstantPar);
				LocalStructure.GetReinvestmentTest().SetLS(YSpann);
			}
		}
		TotalFlow+=i->GetCashFlow();
	}
	LocalStructure.ResetMtgFlows();
	LocalStructure.AddMortgagesFlows(TotalFlow);
	LocalStructure.CalculateTranchesCashFlows();
	emit ScenarioCalculated(Identity[0],Identity[1],LocalStructure);
}