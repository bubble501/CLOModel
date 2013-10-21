#include "ExcelCommons.h"
#include "CentralUnit.h"
#include "WatFalPrior.h"
#include "ExcelOutput.h"
#include "SummaryView.h"
#include <QDate>
#include <QString>
#include <QFile>
//#define DebuggungInputs //TODO Comment me
#include <QApplication>
#ifdef DebuggungInputs
#include <QMessageBox>
#endif
void __stdcall RunModel(LPSAFEARRAY *ArrayData){
#ifdef DebuggungInputs
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	QMessageBox::information(0,"Partito",QString("Dimensione Vettore: %1").arg((*ArrayData)->rgsabound->cElements));
#endif
	bool RunStress;
	CentralUnit TempUnit;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr))return;
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
			Matur=QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Data Ok","Prima Data: "+Matur.toString("dd/MM/yy"));
#endif
			sze=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Ammontare Mutuo OK",QString("Outstanding: %1").arg(sze));
#endif
			Intr=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Interest Ok","Interest Vector: "+Intr);
#endif
			Ann=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
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
			//TmpWstr=pdFreq->bstrVal;pdFreq++;
			//TrName=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			TrName=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
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
			Curr=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
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
			RefRt=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Ref Rate OK",QString("Ref Rate: %1").arg(RefRt));
#endif
			PrevIPD=QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Prev IPD Ok","Prev IPD: "+PrevIPD.toString("dd/MM/yyyy"));
#endif
			BasRt=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
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
		TempUnit.SetFirstIPDdate(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd"));pdFreq++;
		TempUnit.SetLastIPDdate(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd"));pdFreq++;
		TempUnit.SetUseTurbo(pdFreq->boolVal);pdFreq++;
		TempUnit.SetCCCcurve(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetCCChaircut(pdFreq->dblVal);pdFreq++;
		TempUnit.SetCCCTestLimit(pdFreq->dblVal);pdFreq++;
		TempUnit.SetCallDate(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd"));pdFreq++;
		TempUnit.SetCallMultiple(pdFreq->dblVal);pdFreq++;
		TempUnit.SetCallReserve(pdFreq->dblVal);pdFreq++;
		TempUnit.SetPoolValueAtCall(pdFreq->dblVal);pdFreq++;
		{// Reinvestment Test
			QDate ReinPer;
			double ReinvLim,IISha,IRsha,OIsha,ORsha;
			ReinPer=QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
			ReinvLim=pdFreq->dblVal;pdFreq++;
			IRsha=pdFreq->dblVal;pdFreq++;
			IISha=pdFreq->dblVal;pdFreq++;
			ORsha=pdFreq->dblVal;pdFreq++;
			OIsha=pdFreq->dblVal;pdFreq++;
			TempUnit.SetupReinvestmentTest(ReinPer,ReinvLim,IISha,IRsha,OIsha,ORsha);
			QString Intr, CPR,CDR,LS,WAL;
			int Frq;
			double BaseVal;
			Intr=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			CPR=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			CDR=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			LS=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			WAL=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			Frq=pdFreq->intVal; pdFreq++;
			BaseVal=pdFreq->dblVal;pdFreq++;
			TempUnit.SetupReinvBond(Intr,CPR,CDR,LS,WAL,Frq,"N",BaseVal);
		}
		TempUnit.SetPrincipalAvailable(pdFreq->dblVal);pdFreq++;
		TempUnit.SetInterestAvailable(pdFreq->dblVal);pdFreq++;
		TempUnit.SetPoolCutOff(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd"));pdFreq++;
		TempUnit.SetRunCall(pdFreq->boolVal);pdFreq++;
#ifdef DebuggungInputs
		QMessageBox::information(0,"Call",QString("Use Call: %1").arg(TempUnit.GetRunCall()));
#endif
		TempUnit.SetMtgOutputAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetTranchesOutputAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetPlotsSheet(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,0);pdFreq++;
		TempUnit.SetLossOutputAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetLossOnCallOutputAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetCreditEnanAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetFolderPath(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		RunStress=pdFreq->boolVal;pdFreq++;
		if(RunStress){// Stress Test
			QString ConstPar=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
			QMessageBox::information(0,"Stress Constant Parameter",QString("Stress Constant Parameter: "+ConstPar));
#endif
			int XVar=pdFreq->intVal;pdFreq++;
			int YVar=pdFreq->intVal;pdFreq++;
			int NumElem=pdFreq->intVal;pdFreq++;
			QList<QString> XSpann;
			for(int i=0;i<NumElem;i++){
				XSpann.append(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
#ifdef DebuggungInputs
				QMessageBox::information(0,"Stress XSpann Parameter",QString("Stress XSpann[%1]: ").arg(i) +XSpann.last());
#endif
			}
			NumElem=pdFreq->intVal;pdFreq++;
			QList<QString> YSpann;
			for(int i=0;i<NumElem;i++){
				YSpann.append(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
#ifdef DebuggungInputs
				QMessageBox::information(0,"Stress YSpann Parameter",QString("Stress YSpann[%1]: ").arg(i) +YSpann.last());
#endif
			}
			TempUnit.SetupStress(ConstPar,XSpann,YSpann,StressTest::StressVariability(XVar),StressTest::StressVariability(YVar));
			TempUnit.SetStressToCall(pdFreq->boolVal);pdFreq++;
		}
	}
	SafeArrayUnaccessData(*ArrayData);
#ifndef DebuggungInputs
	if(RunStress) TempUnit.CalculateStress();
	else TempUnit.Calculate();
#endif
}

double __stdcall CLODiscountMargin(LPSAFEARRAY *ArrayData){
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return 0.0;
	QString FolderPath=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString TrancheName=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	bool ToCall=pdFreq->boolVal;pdFreq++;
	double NewPrice=pdFreq->dblVal;pdFreq++;
	SafeArrayUnaccessData(*ArrayData);
#ifdef DebuggungInputs
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	QMessageBox::information(0,"Arguments OK","Arguments are right");
#endif
	Waterfall TempWaterfall;
	QString Filename=FolderPath+"\\.BaseCase.clo";
#ifndef Q_WS_WIN
	Filename.prepend('.');
#endif
	QFile file(Filename);
	if(!file.exists()){
#ifdef DebuggungInputs
		QMessageBox::information(0,"File Non Esiste",QString("Il File "+ Filename +" Non Esiste"));
#endif
		return 0.0;
	}
#ifdef DebuggungInputs
	QMessageBox::information(0,"File Esiste",QString("Il File "+ Filename +" Esiste"));
#endif
	if (!file.open(QIODevice::ReadOnly)){
#ifdef DebuggungInputs
		QMessageBox::information(0,"Impossibile Aprire",QString("Il File "+ Filename +" Non puo' essere aperto"));
#endif
		return 0.0;
	}
#ifdef DebuggungInputs
	QMessageBox::information(0,"Aperto",QString("Il File "+ Filename +" e' stato aperto"));
#endif
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> TempWaterfall;
	if(ToCall) out >> TempWaterfall;
	file.close();
	const Tranche* TranchPoint=TempWaterfall.GetTranche(TrancheName);
	if(!TranchPoint){
#ifdef DebuggungInputs
		QString TranchesNames;
		for(int i=0;i<TempWaterfall.GetTranchesCount();i++) TranchesNames+='\n'+TempWaterfall.GetTranche(i)->GetTrancheName();
		QMessageBox::information(0,"Tranche non Trovata",QString("La Tranche "+TrancheName+" non e' Stata Trovata\nTranches Disponibili:"+TranchesNames));
#endif
		return 0.0;
	}
#ifdef DebuggungInputs
	QMessageBox::information(0,"Tranche Trovata",QString("La Tranche "+TrancheName+" e' Stata Trovata"));
#endif
	Tranche TempTranche(*TranchPoint);
	TempTranche.SetPrice(NewPrice);
	return TempTranche.GetDiscountMargin();
}
double __stdcall CLOWALife(LPSAFEARRAY *ArrayData){
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return 0.0;
	QString FolderPath=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString TrancheName=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QDate StartDate=QDate::fromString(QString::fromStdWString(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
	bool ToCall=pdFreq->boolVal;pdFreq++;
	double NewPrice=pdFreq->dblVal;pdFreq++;
	SafeArrayUnaccessData(*ArrayData);
	Waterfall TempWaterfall;
	QString Filename=FolderPath+"\\.BaseCase.clo";
	QFile file(Filename);
	if(!file.exists())return 0.0;
	if (!file.open(QIODevice::ReadOnly))return 0.0;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> TempWaterfall;
	if(ToCall) out >> TempWaterfall;
	file.close();
	const Tranche* TranchPoint=TempWaterfall.GetTranche(TrancheName);
	if(!TranchPoint) return 0.0;
	Tranche TempTranche(*TranchPoint);
	TempTranche.SetPrice(NewPrice);
	return TempTranche.GetWALife(StartDate);
}
void __stdcall StressTargetChanged(LPSAFEARRAY *ArrayData){
	StressTest TempStress;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return;
	QString FolderPath=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString TrancheName=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString TargetCell=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	int XVar=pdFreq->intVal;pdFreq++;
	int YVar=pdFreq->intVal;pdFreq++;
	QString Filename=(FolderPath+"\\.StressResult%1%2.fcsr").arg(XVar).arg(YVar);
	TempStress.LoadResultsFromFile(Filename);
	/*QFile file(Filename);
	if(!file.exists())return ;
	if (!file.open(QIODevice::ReadOnly))return;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> TempStress;*/
	ExcelOutput::PrintStressTest(TempStress,TrancheName,TargetCell,true);
}
void __stdcall InspectStress(LPSAFEARRAY *ArrayData){
	Waterfall TempStructure;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return;
	QString FolderPath=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString RowHead=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString ColHead=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	int XVar=pdFreq->intVal;pdFreq++;
	int YVar=pdFreq->intVal;pdFreq++;
	TempStructure=StressTest::GetScenarioFromFile((FolderPath+"\\.StressResult%1%2.fcsr").arg(XVar).arg(YVar),RowHead,ColHead);
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	SummaryView SitRep;
	SitRep.show();
	SitRep.SetStructure(TempStructure);
	ComputationLoop.exec();	
}

#ifdef DebuggungInputs
void __stdcall TestingInput(LPBSTR a){
	BSTR b=SysAllocString(*a);
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	QMessageBox::information(0,"Stringa letta",QString::fromWCharArray(b));
	SysFreeString(b);
}
#endif