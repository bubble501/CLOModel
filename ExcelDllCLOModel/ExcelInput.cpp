#include "ExcelCommons.h"
#include "CentralUnit.h"
#include "WatFalPrior.h"
#include "ExcelOutput.h"
#include "SummaryView.h"
#include "WaterfallViewer.h"
#include "CommonFunctions.h"
#include <QDate>
#include <QString>
#include <QFile>
//#define  DebuggungInputs//TODO Comment me
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
		QString lossm,prem,Ann,Hairc;
		int frq;
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
			prem=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Prepay Multiplier OK",QString("Prepay Mult: %1").arg(prem));
#endif
			lossm=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Loss Multiplier OK",QString("Loss Mult: %1").arg(lossm));
#endif
			Hairc=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Haircut Vector OK",QString("Haircut: %1").arg(Hairc));
#endif
			if(sze>0.0) TempUnit.AddLoan(Matur,sze,Intr,Ann,frq,0.0,lossm,prem,Hairc);
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Loan OK",QString("Loan %1 Aggiunto").arg(i+1));
#endif
		}
	}
	{//Tranches
		QString TrName, Curr, RefRt, BasRt ,coup, TrancheISIN,IPDfrq;
		int ProRat,IntrTpe;
		double origOut,currOut,RefRtVal,OClim,IClim,Price,Exchan,accruedIntr/*,coup*/;
		QDate PrevIPD,SettDate;
		NumElements=pdFreq++->intVal;
#ifdef DebuggungInputs
		QMessageBox::information(0,"Tranches",QString("Numero Tranches: %1").arg(NumElements));
#endif
		for(int i=0;i<NumElements;i++){
			//TmpWstr=pdFreq->bstrVal;pdFreq++;
			//TrName=QString(std::string(TmpWstr.begin(),TmpWstr.end()).c_str());
			TrName=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"name Ok","Name: "+TrName);
#endif
			TrancheISIN=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"ISIN Ok","ISIN: "+TrancheISIN);
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
			//coup=pdFreq->dblVal;pdFreq++;
			coup=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
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
			//IPDfrq=pdFreq->intVal; pdFreq++;
			IPDfrq=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
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
			SettDate=QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Settlement Date Ok","Settlement Date: "+SettDate.toString("dd-MM-yyyy"));
#endif
			accruedIntr=(pdFreq->dblVal)/100.0;pdFreq++;
#ifdef DebuggungInputs
			if(i==0) QMessageBox::information(0,"Accrued Interest Ok",QString("Accrued Interest: %1").arg(accruedIntr));
#endif
			TempUnit.AddTranche(TrName,TrancheISIN,ProRat,origOut,Curr,currOut,Tranche::TrancheInterestType(IntrTpe),coup,RefRt,PrevIPD,BasRt,IPDfrq,SettDate,accruedIntr,RefRtVal,OClim,IClim,Price,Exchan);
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
		TempUnit.SetPaymentFrequency(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
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
		{//Reserve Fund
			QString RFtarget,RFmultiple,RFfloor;
			double RFcurrent;
			int RFfree;
			bool RFtoIntres;
			for(int RFiter=0;RFiter<NumReserves;RFiter++){
				RFtarget=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
				RFmultiple=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
				RFfloor=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
				RFcurrent=pdFreq->dblVal; pdFreq++;
				RFfree=pdFreq->intVal; pdFreq++;
				RFtoIntres=pdFreq->boolVal; pdFreq++;
#ifdef DebuggungInputs
				QMessageBox::information(0,QString("Reserve %1").arg(RFiter+1),QString("Target: %1\nMultiple: %2\nFloor: %3\nCurrent: %4\nFree: %5\nTo Interest: %6").arg(RFtarget).arg(RFmultiple).arg(RFfloor).arg(RFcurrent).arg(RFfree).arg(RFtoIntres));
#endif
				TempUnit.SetReserveFund(RFiter,RFtarget,RFmultiple,RFfloor,RFcurrent,RFfree,RFtoIntres);
			}
			bool CumRes=pdFreq->boolVal; pdFreq++;
			TempUnit.SetCumulativeReserves(CumRes);
		}
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
		TempUnit.SetPlotIndexes(pdFreq->intVal,1);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,2);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,3);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,4);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,5);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,6);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,7);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,8);pdFreq++;
#ifdef DebuggungInputs
		QMessageBox::information(0,"Plots Ok","All Plots OK");
#endif
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
			TempUnit.SetStressToCall(pdFreq->boolVal);pdFreq++;
			TempUnit.SetupStress(ConstPar,XSpann,YSpann,StressTest::StressVariability(XVar),StressTest::StressVariability(YVar));
			
		}
	}
	SafeArrayUnaccessData(*ArrayData);
#ifdef _DEBUG
	QFile file("C:/Temp/UnitaProva");
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out << TempUnit;
	file.close();
#endif
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
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> VersionChecker;
	//if(VersionChecker!=qint32(ModelVersionNumber)) return 0.0;
	if(VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) return 0.0;
	TempWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempWaterfall;
	if(ToCall){
		TempWaterfall.SetLoadProtocolVersion(VersionChecker);
		out >> TempWaterfall;
	}
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
	return TranchPoint->GetDiscountMargin(NewPrice);
}
double __stdcall CLOWALife(LPSAFEARRAY *ArrayData){
	qint32 VersionChecker;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return 0.0;
	QString FolderPath=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString TrancheName=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QDate StartDate=QDate::fromString(QString::fromStdWString(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
	bool ToCall=pdFreq->boolVal;pdFreq++;
	SafeArrayUnaccessData(*ArrayData);
	Waterfall TempWaterfall;
	QString Filename=FolderPath+"\\.BaseCase.clo";
	QFile file(Filename);
	if(!file.exists())return 0.0;
	if (!file.open(QIODevice::ReadOnly))return 0.0;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> VersionChecker;
	if(VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) return 0.0;
	TempWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempWaterfall;
	if(ToCall){
		TempWaterfall.SetLoadProtocolVersion(VersionChecker);
		out >> TempWaterfall;
	}
	file.close();
	const Tranche* TranchPoint=TempWaterfall.GetTranche(TrancheName);
	if(!TranchPoint) return 0.0;
	Tranche TempTranche(*TranchPoint);
	return TempTranche.GetWALife(StartDate);
}
void __stdcall StressTargetChanged(LPSAFEARRAY *ArrayData){
#ifdef DebuggungInputs
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
#endif
	StressTest TempStress;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return;
	QString FolderPath=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
	QMessageBox::information(0,"Folder Path",QString("FolderPath: %1").arg(FolderPath));
#endif
	QString TrancheName=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
	QMessageBox::information(0,"Tranche Name",QString("TrancheName: %1").arg(TrancheName));
#endif
	QString TargetCell=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
	QMessageBox::information(0,"Target Cell",QString("TargetCell: %1").arg(TargetCell));
#endif
	int XVar=pdFreq->intVal;pdFreq++;
#ifdef DebuggungInputs
	QMessageBox::information(0,"XVar",QString("XVar: %1").arg(XVar));
#endif
	int YVar=pdFreq->intVal;pdFreq++;
#ifdef DebuggungInputs
	QMessageBox::information(0,"YVar",QString("YVar: %1").arg(YVar));
#endif
	double NewPrice=pdFreq->dblVal;pdFreq++;
#ifdef DebuggungInputs
	QMessageBox::information(0,"New Price",QString("NewPrice: %1").arg(NewPrice));
#endif
	QString PlotSheet=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
#ifdef DebuggungInputs
	QMessageBox::information(0,"Plot Sheet",QString("PlotSheet: %1").arg(PlotSheet));
#endif
	int PlotIndex=pdFreq->intVal;pdFreq++;
#ifdef DebuggungInputs
	QMessageBox::information(0,"Plot Index",QString("PlotIndex: %1").arg(PlotIndex));
#endif
	SafeArrayUnaccessData(*ArrayData);
	QString Filename=(FolderPath+"\\.StressResult%1%2.fcsr").arg(XVar).arg(YVar);
	if(!QFile::exists(Filename))return;
	TempStress.LoadResultsFromFile(Filename);
#ifdef DebuggungInputs
	QMessageBox::information(0,"Results OK",QString("Results Loaded"));
#endif
	ExcelOutput::PrintStressTest(TempStress,TrancheName,TargetCell,true);
#ifdef DebuggungInputs
	QMessageBox::information(0,"Print OK",QString("Results Printed"));
#endif
	if(!PlotSheet.isEmpty() && PlotIndex>0)
		ExcelOutput::PlotStressMargin(TempStress,PlotSheet,PlotIndex,TrancheName,NewPrice);
#ifdef DebuggungInputs
	QMessageBox::information(0,"Plot OK",QString("Results Plotted"));
#endif
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
	SafeArrayUnaccessData(*ArrayData);
	TempStructure=StressTest::GetScenarioFromFile((FolderPath+"\\.StressResult%1%2.fcsr").arg(XVar).arg(YVar),RowHead,ColHead);
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	SummaryView SitRep;
	SitRep.show();
	SitRep.SetStructure(TempStructure);
	SitRep.ShowCallStructure(false);
	ComputationLoop.exec();	
}
void __stdcall InspectWaterfall(LPSAFEARRAY *ArrayData){
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	WaterfallViewer SitRep;
	SitRep.show();
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return;
	int NumSteps=pdFreq->intVal;pdFreq++;
	for(int i=0;i<NumSteps;i++){
		WatFalPrior TempStep;
		TempStep.SetPriorityType(WatFalPrior::WaterfallStepType(pdFreq->intVal));pdFreq++;
		TempStep.SetGroupTarget(pdFreq->intVal);pdFreq++;
		TempStep.SetRedemptionGroup(pdFreq->intVal);pdFreq++;
		TempStep.SetRedemptionShare(pdFreq->dblVal);pdFreq++;
		SitRep.AddStep(TempStep);
	}
	SafeArrayUnaccessData(*ArrayData);
	ComputationLoop.exec();	
}

/*!
\file ExcelInput.cpp
\brief Excel exported functions
\author Luca Beldi
\date November 2013

\details File containing the definition of the function exported into the dll and available through Excel
*/

/*!
\def DebuggungInputs
  If this name is defined, instead of running the model, you'll get a diagnosis of your inputs.

  Useful for debugging the input.
\warning Must be undefined for the actual model to work
*/
/*! \fn void __stdcall RunModel(LPSAFEARRAY *ArrayData)
\brief Function that runs the cash flow model, and the stress test saves and prints the results in excel
\param ArrayData Excel Variant Array of Inputs
\details This function takes in input all the parameters of the model, runs the cash flows and saves the results. It then calls, when appropriate:
- ExcelOutput::PrintMortgagesRepLines
- ExcelOutput::PlotMortgagesFlows
- ExcelOutput::PrintTrancheFlow
- ExcelOutput::PlotTranchesDynamic
- ExcelOutput::PlotOCTest
- ExcelOutput::PlotICTest
- ExcelOutput::PlotAnnualExcess
- ExcelOutput::PlotCostFunding

To send the output to excel.

ArrayData must contain, in order:
<ol><li>A Long representing the number of loans in the pool</li>
<li>For each loan in the pool:</li>
	<ol type="a"><li>A String containing the maturity date of the loan in the ISO 8601 format (yyyy-mm-dd)</li>
	<li>A Double representing the current amount outstanding of the loan</li>
	<li>A String containing a Bloomberg-like vector that represents the interest rate payed on the loan</li>
	<li>A String containing a Bloomberg-like vector that represent if the loan is an annuity. The values must be either Y or N</li>
	<li>A Long representing the number of months between two loans payments (e.g. if the loan pays quarterly this must be 3)</li>
	<li>A Double representing the multiplier by which the loans is more likely to prepay compared to the base hypothesis. 100 is the base case.</li>
	<li>A Double representing the multiplier by which the loans is more likely to experience losses compared to the base hypothesis. 100 is the base case.</li></ol>
<li>A Long representing the number of tranches of the deal</li>
<li>For each tranche in the deal:</li>
	<ol type="a"><li>A String containing the name of the tranche</li>
	<li>A Long representing the redemption priority level of the note (pro rata group)</li>
	<li>A Double containing the original amount outstanding of the note</li>
	<li>A String containing the Bloomberg code of the note currency (e.g. USD, EUR, GBP)</li>
	<li>A double containing the current amount outstanding of the note</li>
	<li>A Long representing the Tranche::TrancheInterestType code determining if the notes pays fixed or floating interest</li>
	<li>A Double containing the margin or the interest rate paid by the note (in basis points)</li>
	<li>A String containing the Bloomberg ticker of the of the reference rate for the interest (if the note pays fixed interest this will be ignored)</li>
	<li>A String containing the last IPD in the ISO 8601 format (yyyy-mm-dd)</li>
	<li>A String containing the Bloomberg ticker of the base rate on which discount margin will be calculated for fixed rate notes (for floating rate notes this will be ignored)</li>
	<li>A Long containing the the number of months between two IPDs (e.g. if the deal pays quarterly this must be 3)</li>
	<li>A Double containing the value of the reference rate for the note. -1.0 will cause the program to get the value from Bloomberg</li>
	<li>A Double containing the limit of the OC test for the tranche (100% = 1.0)</li>
	<li>A Double containing the limit of the IC test for the tranche (100% = 1.0)</li>
	<li>A Double containing the price for the note in a 100 basis</li>
	<li>A Double containing the exchange rate for the note currency (1.0 for the currency in which the loans are denominated)</li>
	<li>A String containing the settlement date in the ISO 8601 format (yyyy-mm-dd)</li>
	<li>A Double representing the interest accrued on the note from the last IPD</li></ol>
<li>A Long containing the number of steps in the waterfall</li>
<li>For each step in the waterfall:</li>
	<ol type="a"><li>A Long representing the WatFalPrior::WaterfallStepType code for the step</li>
	<li>A Long representing the pro rata group to which the step is referred to</li>
	<li>A Long representing the source or destination of the payments due to this step (e.g. For Senior Expenses a 1 will deduct the expenses from interest while a 2 will deduct them from principal. For OC test this is the tranche that will be redeemed if the test fails)</li>
	<li>A Double, used only in OC test and ignored otherwise, that determines what share of available funds will go toward the redemption of the note specified in the previous argument. The rest will go on top of the principal waterfall.</li></ol>
<li>A Double containing senior expenses rate as a share of loans outstanding</li>
<li>A Double containing senior fees rate as a share of loans outstanding</li>
<li>A Double containing junior fees rate as a share of loans outstanding</li>
<li>A Double containing the interest rate at which unpaid junior fees are compounded</li>
<li>A Long containing the the number of months between two IPDs (e.g. if the deal pays quarterly this must be 3)</li>
<li>A String containing the next IPD in the ISO 8601 format (yyyy-mm-dd)</li>
<li>A String containing the previous IPD in the ISO 8601 format (yyyy-mm-dd)</li>
<li>A Boolean: if set to false, reinvestment test and OC test turbo features will not be used</li>
<li>A String containing a Bloomberg-like vector that represents the evoulution of CCC loans as a share of total loans</li>
<li>A Double representing the haircut inflicted to CCC loans if the CCC test is failed</li>
<li>A Double representing the CCC test limit</li>
<li>A String containing the call date in the ISO 8601 format (yyyy-mm-dd). Empty string ("") if not applicable</li>
<li>A Double by which the call reserve (see below) is multiplied for computing the call event. 0 if not applicable</li>
<li>A Double containing the call reserve. It can be either a seniority level or an absolute amount. When the outstanding amount of the notes gets below this amount the deal will be called. 0 if not applicable</li>
<li>A Double representing the value of the loan pool at the call date (in basis 100)</li>
<li>A String containing the reinvestment period end date in the ISO 8601 format (yyyy-mm-dd).</li>
<li>A Double representing the reinvestment test limit</li>
<li>A Double representing how much interest will be diverted to notes redemption during the reinvestment period in case the reinvestment test is breached</li>
<li>A Double representing how much interest will be diverted to acquire new collateral during the reinvestment period in case the reinvestment test is breached</li>
<li>A Double representing how much interest will be diverted to notes redemption after the reinvestment period in case the reinvestment test is breached</li>
<li>A Double representing how much interest will be diverted to acquire new collateral after the reinvestment period in case the reinvestment test is breached</li>
<li>A String containing a Bloomberg-like vector that represents the spread payed by new collateral coming from reinvestment</li>
<li>A String containing a Bloomberg-like vector that represents the CPR assumption for the model</li>
<li>A String containing a Bloomberg-like vector that represents the CDR assumption for the model</li>
<li>A String containing a Bloomberg-like vector that represents the LS assumption for the model</li>
<li>A String containing a Bloomberg-like vector that represents the WAL (in years) of the reinvestment bond</li>
<li>A Long representing the number of months between two payments made by newly acquired loans (e.g. if the loans pay quarterly this must be 3)</li>
<li>A String containing the Bloomberg ticker of the of the reference rate for the new loans coming from reinvestment</li>
<li>A Double containing the principal available amounts currently in the cash account of the issuer</li>
<li>A Double containing the interest available amounts currently in the cash account of the issuer</li>
<li>A String containing the pool cut off date in the ISO 8601 format (yyyy-mm-dd).</li>
<li>A Boolean that determines if the call hypothesis should be evaluated or not</li>
<li>A String containing the address of the cell where the loans cash flows should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the address of the cell where the tranches cash flows should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the name of the sheet where the charts are located</li>
<li>A Long representing the index of the chart object where the loans cash flow graph should be printed on</li>
<li>A Long representing the index of the chart object where the tranches cash flow graph should be printed on</li>
<li>A Long representing the index of the chart object where the OC test graph should be printed on</li>
<li>A Long representing the index of the chart object where the IC test graph should be printed on</li>
<li>A Long representing the index of the chart object where the excess spread graph should be printed on</li>
<li>A Long representing the index of the chart object where the WA cost of capital graph should be printed on</li>
<li>A String containing the address of the cell where the loss rates of the notes should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the address of the cell where the loss rates of the notes, in the call scenario, should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the address of the cell where the credit enhancement of the notes should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the path to the folder where the results should be saved to</li>
<li>A Boolean representing if the stress test should be run or just the base model</li></ol>

In case the stress test should be run, this additional inputs, in order, are necessary:
<ol start="53"><li>A String containing a Bloomberg-like vector that represents the parameter that will be kept constant in the stress test</li>
<li>A Long representing the StressTest::StressVariability code for the rows variation dimension</li>
<li>A Long representing the StressTest::StressVariability code for the columns variation dimension</li>
<li>A Long containing the number of scenarios on the rows variation dimension</li>
<li>For scenario on the rows variation dimension:</li>
	<ol type="a"><li>A String containing a Bloomberg-like vector that represents the variable stress value</li></ol>
<li>A Long containing the number of scenarios on the columns variation dimension</li>
<li>For scenario on the columns variation dimension:</li>
	<ol type="a"><li>A String containing a Bloomberg-like vector that represents the variable stress value</li></ol>
<li>A Boolean determining if the stress test should consider the call scenario</li>
</ol>
\warning Long refers to 32 bits integers. Using 16 bits one (Integer instead of Long in VBA) will lead to a crash
*/
/*! \fn double __stdcall CLODiscountMargin(LPSAFEARRAY *ArrayData)
\brief Function that gets the discount margin for a tranche from a result file
\param ArrayData Excel Variant Array of Inputs
\return The discount margin of the selected tranche
\details This function loads the results of the model and calculates the discount margin of a tranche associated with the given price

ArrayData must contain, in order:
-# A String containing the path to the stress result file
-# A String containing the name of the tranche for which you want to display the results
-# A Boolean defining if the calculation should consider the call option as exercised
-# A Double containing the price of the tranche
 */
/*! \fn double __stdcall CLOWALife(LPSAFEARRAY *ArrayData)
\brief Function that gets the weighted average life for a tranche from a result file
\param ArrayData Excel Variant Array of Inputs
\return The weighted average life of the selected tranche
\details This function loads the results of the model and calculates the weighted average life of a tranche

ArrayData must contain, in order:
-# A String containing the path to the stress result file
-# A String containing the name of the tranche for which you want to display the results
-# A string containing the date from which the WAL starts. It must be in the ISO 8601 format (yyyy-mm-dd)
-# A Boolean defining if the calculation should consider the call option as exercised
 */
/*! \fn void __stdcall StressTargetChanged(LPSAFEARRAY *ArrayData)
\brief Function that has to be called to get the stress results for a different tranche
\param ArrayData Excel Variant Array of Inputs
\details This function loads the results of the stress test and invokes ExcelOutput::PrintStressTest() to send the stress test results of the appropriate tranche back to excel.

ArrayData must contain, in order:
-# A String containing the path to the stress result file
-# A String containing the name of the tranche for which you want to display the results
-# A string containing the address of the cell where the stress results should be printed
-# A Long representing the StressTest::StressVariability dimention for the X parameter
-# A Long representing the StressTest::StressVariability dimention for the Y parameter
-# A double representing the price of the tranche
-# A String containing the name of the sheet where the discount margin plot should appear
-# A Long containing the index of the Chart where the discount margin plot should be displayed

\sa ExcelOutput::PrintStressTest()
 */
/*! \fn void __stdcall InspectStress(LPSAFEARRAY *ArrayData)
\brief Function that displays the whole waterfall results for a given stress scenario
\param ArrayData Excel Variant Array of Inputs
\details This function creates a Widget containing the waterfall results for the given stress scenario

ArrayData must contain, in order:
-# A String containing the path to the stress result file
-# A String containing the value for the X Parameter
-# A String containing the value for the Y Parameter
-# A Long representing the StressTest::StressVariability dimention for the X parameter
-# A Long representing the StressTest::StressVariability dimention for the Y parameter

\sa SummaryView
 */
/*! \fn void __stdcall InspectWaterfall(LPSAFEARRAY *ArrayData)
\brief Function that displays the two waterfalls
\param ArrayData Excel Variant Array of Inputs
\details This function creates a Widget containing two tables each of them showing one of the waterfalls.

ArrayData must contain, in order:
<ol><li>The number of steps in the aggregated waterfall</li>
<li>For each step in the waterfall</li>
	<li>Long representing the WatFalPrior::WaterfallStepType of the waterfall</li>
	<li>Long representing the target seniority group of the step</li>
	<li>Long representing funds sources or destination for the step</li>
	<li>Double between 0 and 1 representing the share of funds going toward redemption of the destination step (used only in OC Test)</li></ol></ol>

\sa WaterfallViewer
 */