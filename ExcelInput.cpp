#include "ExcelCommons.h"
#include "CentralUnit.h"
#include "WatFalPrior.h"
#include <QDate>
#include <QString>
//#define DebuggungInputs //TODO Comment me
#ifdef DebuggungInputs
#include <QApplication>
#include <QMessageBox>
#endif
void __stdcall RunModel(LPSAFEARRAY *ArrayData){
#ifdef DebuggungInputs
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	QMessageBox::information(0,"Partito",QString("Dimensione Vettore: %1").arg((*ArrayData)->rgsabound->cElements));
#endif

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
		NumElements=pdFreq++->intVal;
#ifdef DebuggungInputs
		QMessageBox::information(0,"Mutui",QString("Numero Mutui: %1").arg(NumElements));
#endif
		for(int i=0;i<NumElements;i++){
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Matur=QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd");
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Data Ok","Prima Data: "+Matur.toString("dd/MM/yy"));
#endif
			sze=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Ammontare Mutuo OK",QString("Outstanding: %1").arg(sze));
#endif
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Intr=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Interest Ok","Interest Vector: "+Intr);
#endif
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Ann=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Annuity Ok","Annuity Vector: "+Ann);
#endif
			frq=pdFreq->intVal; pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Frequency OK",QString("Frequenza: %1").arg(frq));
#endif
			prem=pdFreq->intVal; pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Prepay Multiplier OK",QString("Prepay Mult: %1").arg(prem));
#endif
			lossm=pdFreq->intVal; pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Loss Multiplier OK",QString("Loss Mult: %1").arg(lossm));
#endif
			TempUnit.AddLoan(Matur,sze,Intr,Ann,frq,0.0,lossm,prem);
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Loan OK",QString("Loan Aggiunto"));
#endif
		}
	}
	{//Tranches
		QString TrName, Curr, RefRt, BasRt;
		int ProRat,IntrTpe,IPDfrq;
		double origOut,currOut,coup,RefRtVal,OClim,IClim,Price,Exchan;
		QDate PrevIPD;
		NumElements=pdFreq++->intVal;
#ifdef DebuggungInputs
		QMessageBox::information(0,"Mutui",QString("Numero Tranches: %1").arg(NumElements));
#endif
		for(int i=0;i<NumElements;i++){
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			TrName=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"name Ok","Name: "+TrName);
#endif
			ProRat=pdFreq->intVal; pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Pro Rata OK",QString("Pro Rata: %1").arg(ProRat));
#endif
			origOut=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Original Out OK",QString("Original Out: %1").arg(origOut));
#endif
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			Curr=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Currency Ok","Name: "+Curr);
#endif
			currOut=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Current Out OK",QString("Current Out: %1").arg(currOut));
#endif
			IntrTpe=pdFreq->intVal; pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Intr Type OK",QString("Intr Type: %1").arg(IntrTpe));
#endif
			coup=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Intr OK",QString("Coupon: %1").arg(coup));
#endif
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			RefRt=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Ref Rate OK",QString("Ref Rate: %1").arg(RefRt));
#endif
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			PrevIPD=QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd");
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Prev IPD Ok","Prev IPD: "+PrevIPD.toString("dd/MM/yyyy"));
#endif
			TmpWstr=pdFreq->bstrVal;pdFreq++;
			BasRt=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Base rate Ok","Base rate: "+BasRt);
#endif
			IPDfrq=pdFreq->intVal; pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"IPD Frequ Ok",QString("IPD Frequ: %1").arg(IPDfrq));
#endif
			RefRtVal=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Ref Rate Val Ok",QString("Ref Rate Val: %1").arg(RefRtVal));
#endif
			OClim=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"OClim Ok",QString("OClim: %1").arg(OClim));
#endif
			IClim=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"IClim Ok",QString("IClim: %1").arg(IClim));
#endif
			Price=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Price Ok",QString("Price: %1").arg(Price));
#endif
			Exchan=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Exchan Ok",QString("Exchan: %1").arg(Exchan));
#endif
			TempUnit.AddTranche(TrName,ProRat,origOut,Curr,currOut,Tranche::TrancheInterestType(IntrTpe),coup,RefRt,PrevIPD,BasRt,IPDfrq,RefRtVal,OClim,IClim,Price,Exchan);
		}
	}
	{ //Waterfall Steps
		int Prior,GrpTg, RedTg;
		double RedSh;
		NumElements=pdFreq++->intVal;
#ifdef DebuggungInputs
		QMessageBox::information(0,"Mutui",QString("Numero Steps: %1").arg(NumElements));
#endif
		for(int i=0;i<NumElements;i++){
			Prior=pdFreq->intVal; pdFreq++;
			GrpTg=pdFreq->intVal; pdFreq++;
			RedTg=pdFreq->intVal; pdFreq++;
			RedSh=pdFreq->dblVal;pdFreq++;
			TempUnit.AddWaterfallStep(WatFalPrior::WaterfallStepType(Prior),GrpTg,RedTg,RedSh);
		}
	}
	{ //General Inputs
		TempUnit.SetSeniorExpenses(pdFreq->dblVal);pdFreq++;
		TempUnit.SetSeniorFees(pdFreq->dblVal);pdFreq++;
		TempUnit.SetJuniorFees(pdFreq->dblVal);pdFreq++;
		TempUnit.SetJuniorFeesCoupon(pdFreq->dblVal);pdFreq++;
		TempUnit.SetPaymentFrequency(pdFreq->intVal);pdFreq++;
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
			Frq=pdFreq->intVal; pdFreq++;
			BaseVal=pdFreq->dblVal;pdFreq++;
			TempUnit.SetupReinvBond(Intr,CPR,CDR,LS,WAL,Frq,"N",BaseVal);
		}
		TempUnit.SetPrincipalAvailable(pdFreq->dblVal);pdFreq++;
		TempUnit.SetInterestAvailable(pdFreq->dblVal);pdFreq++;
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetPoolCutOff(QDate::fromString(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()),"yyyy-MM-dd"));
		TempUnit.SetRunCall(pdFreq->boolVal);pdFreq++;
#ifdef DebuggungInputs
		QMessageBox::information(0,"Call",QString("Use Call: %1").arg(TempUnit.GetRunCall()));
#endif
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetMtgOutputAddress(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()));
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetTranchesOutputAddress(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()));
		TmpWstr=pdFreq->bstrVal;pdFreq++;
		TempUnit.SetFolderPath(QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str()));
	}
#ifndef DebuggungInputs
	TempUnit.Calculate();
#endif
}