#include "ExcelCommons.h"
#include "CentralUnit.h"
#include "WatFalPrior.h"
#include <QDate>
#include <QString>

#include <QApplication>//TODO delete me
#include <QMessageBox> //TODO delete me
void __stdcall RunModel(LPSAFEARRAY *ArrayData){
	CentralUnit TempUnit;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr))return;
	std::wstring TmpWstr;
	int NumElements;
	{ //Loans
		QDate Matur;
		double sze;
		QString Intr;
		QString Ann;
		int lossm,prem,frq;
		NumElements=pdFreq++->lVal;
		for(int i=0;i<NumElements;i++){
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Matur=QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd");
			sze=pdFreq->dblVal;pdFreq++;
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Intr=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Ann=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			frq=pdFreq->lVal; pdFreq++;
			prem=pdFreq->lVal; pdFreq++;
			lossm=pdFreq->lVal; pdFreq++;
			TempUnit.AddLoan(Matur,sze,Intr,Ann,frq,0.0,lossm,prem);
		}
	}
	{//Tranches
		QString TrName, Curr, RefRt, BasRt;
		int ProRat,IntrTpe,IPDfrq;
		double origOut,currOut,coup,RefRtVal,OClim,IClim,Price,Exchan;
		QDate PrevIPD;
		NumElements=pdFreq++->lVal;
		for(int i=0;i<NumElements;i++){
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			TrName=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			ProRat=pdFreq->lVal; pdFreq++;
			origOut=pdFreq->dblVal;pdFreq++;
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Curr=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			currOut=pdFreq->dblVal;pdFreq++;
			IntrTpe=pdFreq->lVal; pdFreq++;
			coup=pdFreq->dblVal;pdFreq++;
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			RefRt=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			PrevIPD=QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd");
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			BasRt=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			IPDfrq=pdFreq->lVal; pdFreq++;
			RefRtVal=pdFreq->dblVal;pdFreq++;
			OClim=pdFreq->dblVal;pdFreq++;
			IClim=pdFreq->dblVal;pdFreq++;
			Price=pdFreq->dblVal;pdFreq++;
			Exchan=pdFreq->dblVal;pdFreq++;
			TempUnit.AddTranche(TrName,ProRat,origOut,Curr,currOut,Tranche::TrancheInterestType(IntrTpe),coup,RefRt,PrevIPD,BasRt,IPDfrq,RefRtVal,OClim,IClim,Price,Exchan);
		}
	}
	{ //Waterfall Steps
		int Prior,GrpTg, RedTg;
		double RedSh;
		NumElements=pdFreq++->lVal;
		for(int i=0;i<NumElements;i++){
			Prior=pdFreq->lVal; pdFreq++;
			GrpTg=pdFreq->lVal; pdFreq++;
			RedTg=pdFreq->lVal; pdFreq++;
			RedSh=pdFreq->dblVal;pdFreq++;
			TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType(Prior),GrpTg,RedTg,RedSh);
		}
	}
	{ //General Inputs
		TempUnit.SetSeniorExpenses(pdFreq->dblVal);pdFreq++;
		TempUnit.SetSeniorFees(pdFreq->dblVal);pdFreq++;
		TempUnit.SetJuniorFees(pdFreq->dblVal);pdFreq++;
		TempUnit.SetJuniorFeesCoupon(pdFreq->dblVal);pdFreq++;
		TempUnit.SetPaymentFrequency(pdFreq->lVal);pdFreq++;
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetFirstIPDdate(QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd"));
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetLastIPDdate(QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd"));
		TempUnit.SetUseTurbo(pdFreq->boolVal);pdFreq++;
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetCCCcurve(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()));
		TempUnit.SetCCChaircut(pdFreq->dblVal);pdFreq++;
		TempUnit.SetCCCTestLimit(pdFreq->dblVal);pdFreq++;
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetCallDate(QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd"));
		TempUnit.SetCallMultiple(pdFreq->dblVal);pdFreq++;
		TempUnit.SetCallReserve(pdFreq->dblVal);pdFreq++;
		TempUnit.SetPoolValueAtCall(pdFreq->dblVal);pdFreq++;
		{// Reinvestment Test
			QDate ReinPer;
			double ReinvLim,IISha,IRsha,OIsha,ORsha;
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			ReinPer=QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd");
			ReinvLim=pdFreq->dblVal;pdFreq++;
			IRsha=pdFreq->dblVal;pdFreq++;
			IISha=pdFreq->dblVal;pdFreq++;
			ORsha=pdFreq->dblVal;pdFreq++;
			OIsha=pdFreq->dblVal;pdFreq++;
			TempUnit.SetupReinvestmentTest(ReinPer,ReinvLim,IISha,IRsha,OIsha,ORsha);
			QString Intr, CPR,CDR,LS,WAL;
			int Frq;
			double BaseVal;
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Intr=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			CPR=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			CDR=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			LS=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			WAL=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			Frq=pdFreq->lVal; pdFreq++;
			BaseVal=pdFreq->dblVal;pdFreq++;
			TempUnit.SetupReinvBond(Intr,CPR,CDR,LS,WAL,Frq,"N",BaseVal);
		}
		TempUnit.SetPrincipalAvailable(pdFreq->dblVal);pdFreq++;
		TempUnit.SetInterestAvailable(pdFreq->dblVal);pdFreq++;
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetPoolCutOff(QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd"));
		TempUnit.SetRunCall(pdFreq->boolVal);pdFreq++;
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetMtgOutputAddress(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()));
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetTranchesOutputAddress(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()));
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetFolderPath(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()));
	}
	TempUnit.Calculate();
}