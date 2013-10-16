#include "CentralUnit.h"
#include <QMetaType>
#include <QApplication>
#include <QtConcurrentRun>
#include <QtConcurrentFilter>
#include <QtConcurrentMap>
#include <QTimer>
#include <QFile>
#include "ExcelOutput.h"
#include "WaterfallCalculator.h"


CentralUnit::CentralUnit(QObject* parent)
	:QObject(parent)
	,Stresser(NULL)
	,RunCall(false)
{
	if(!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");
	if(!QMetaType::isRegistered(qMetaTypeId<MtgCashFlow>()))
		qRegisterMetaType<MtgCashFlow>("MtgCashFlow");
	connect(&LoansCalculator,SIGNAL(Calculated()),this,SLOT(CalculationStep2()));
	connect(this,SIGNAL(LoopStarted()),this,SLOT(CalculationStep1()),Qt::QueuedConnection);
	ParallWatFalls=new WaterfallCalculator(this);
	connect(ParallWatFalls,SIGNAL(Calculated()),this,SLOT(CheckCalculationDone()));
}
void CentralUnit::SetPoolCutOff(const QDate& a){PoolCutOff=a; if(Stresser) Stresser->SetStartDate(PoolCutOff);}
void CentralUnit::SetFolderPath(const QString& a){FolderPath=a;}
void CentralUnit::AddLoan(const QDate& Maturity, double Size,  const QString& Interest, const QString& Annuity, int Freq,double floatBase,int LossMult, int PrepayMult){
	Mortgage TempMtg;
	TempMtg.SetMaturityDate(Maturity);
	TempMtg.SetSize(Size);
	TempMtg.SetPrepayMultiplier(PrepayMult);
	TempMtg.SetLossMultiplier(LossMult);
	TempMtg.SetInterest(Interest);
	TempMtg.SetFloatingRateBase(floatBase);
	TempMtg.SetPaymentFreq(Freq);
	TempMtg.SetAnnuity(Annuity);
	LoansCalculator.AddLoan(TempMtg);
	if(Stresser)Stresser->AddLoan(TempMtg);
}
void CentralUnit::AddTranche(const QString& Name,int ProRataGroup, double MinOC, double MinIC, double Price,double FxRate,const QString& BbgExt, int DayCount){
	Tranche TempTrnch;
	TempTrnch.SetTrancheName(Name);
	TempTrnch.SetProrataGroup(ProRataGroup);
	TempTrnch.SetMinIClevel(MinIC);
	TempTrnch.SetMinOClevel(MinOC);
	TempTrnch.SetPrice(Price);
	TempTrnch.SetExchangeRate(FxRate);
	TempTrnch.SetBloombergExtension(BbgExt);
	TempTrnch.SetDayCount(DayCount);
	TempTrnch.GetDataFromBloomberg();
	Structure.AddTranche(TempTrnch);
	if(Stresser)Stresser->SetStructure(Structure);
}
void CentralUnit::AddTranche(
	const QString& Name
	,int ProRataGroup
	,double OrigAmnt
	,const QString& Crncy
	,double OutstandingAmt
	,Tranche::TrancheInterestType IntrTyp
	,double Coupon
	,const QString& RefRte
	,const QDate& LastPayDte
	,const QString& DefRefRte
	,int PayFreq
	,double RefRteValue
	,double MinOC
	,double MinIC
	,double Price
	,double FxRate
	,const QString& BbgExt
	,int DayCount
){
	Tranche TempTrnch;
	TempTrnch.SetTrancheName(Name);
	TempTrnch.SetProrataGroup(ProRataGroup);
	TempTrnch.SetOriginalAmount(OrigAmnt);
	TempTrnch.SetCurrency(Crncy);
	TempTrnch.SetOutstandingAmt(OutstandingAmt);
	TempTrnch.SetInterestType(IntrTyp);
	TempTrnch.SetCoupon(Coupon);
	TempTrnch.SetReferenceRate(RefRte);
	TempTrnch.SetLastPaymentDate(LastPayDte);
	TempTrnch.SetDefaultRefRate(DefRefRte);
	TempTrnch.SetPaymentFrequency(PayFreq);
	TempTrnch.SetReferenceRateValue(RefRteValue);
	TempTrnch.SetMinIClevel(MinIC);
	TempTrnch.SetMinOClevel(MinOC);
	TempTrnch.SetPrice(Price);
	TempTrnch.SetExchangeRate(FxRate);
	TempTrnch.SetBloombergExtension(BbgExt);
	TempTrnch.SetDayCount(DayCount);
	Structure.AddTranche(TempTrnch);
	if(Stresser)Stresser->SetStructure(Structure);
}
void CentralUnit::AddWaterfallStep(WatFalPrior::WaterfallStepType Tpe,int GrpTgt, int RdmpGrp, double RdmpShare){
	WatFalPrior TempStep;
	TempStep.SetGroupTarget(GrpTgt);
	TempStep.SetRedemptionGroup(RdmpGrp);
	TempStep.SetRedemptionShare(RdmpShare);
	TempStep.SetPriorityType(Tpe);
	Structure.AddStep(TempStep);
	if(Stresser)Stresser->SetStructure(Structure);
}
void CentralUnit::Reset(){
	LoansCalculator.Reset();
	Structure.ResetMtgFlows();
	Structure.ResetSteps();
	Structure.ResetTranches();
	if(Stresser){
		Stresser->deleteLater();
		Stresser=NULL;
	}
}
void CentralUnit::SetupStress(const QString& ConstPar,QList<QString> XSpann,QList<QString> YSpann, StressTest::StressVariability Xvar,StressTest::StressVariability Yvar){
	if(!Stresser){
		Stresser=new StressTest(this);
	}
	Stresser->SetConstantPar(ConstPar);
	Stresser->SetXSpann(XSpann);
	Stresser->SetYSpann(YSpann);
	Stresser->SetXVariability(Xvar);
	Stresser->SetYVariability(Yvar);
	Stresser->SetStartDate(PoolCutOff);
	Stresser->SetStructure(Structure);
	for(int i=0;i<LoansCalculator.GetLoans().size();i++)
		Stresser->AddLoan(*LoansCalculator.GetLoans().at(i));
	connect(this,SIGNAL(StressLoopStarted()),Stresser,SLOT(RunStressTest()),Qt::QueuedConnection);
	connect(Stresser,SIGNAL(AllFinished()),this,SLOT(StressFinished()));
}
void CentralUnit::Calculate(){
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	emit LoopStarted();
	ComputationLoop.exec();
}
void CentralUnit::CalculateStress(){
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	emit StressLoopStarted();
	ComputationLoop.exec();
}
void CentralUnit::CalculationStep1(){
	LoansCalculator.SetCPR(Structure.GetReinvestmentTest().GetCPRAssumption());
	LoansCalculator.SetCDR(Structure.GetReinvestmentTest().GetCDRAssumption());
	LoansCalculator.SetLS(Structure.GetReinvestmentTest().GetLSAssumption());
	LoansCalculator.SetStartDate(PoolCutOff);
	LoansCalculator.StartCalculation();
}
void CentralUnit::CalculationStep2(){
	Structure.ResetMtgFlows();
	Structure.AddMortgagesFlows(LoansCalculator.GetResult());
	Structure.SetUseCall(false);
	ParallWatFalls->ResetWaterfalls();
	if(!RunCall){		
		Structure.CalculateTranchesCashFlows();
	}
	else{
		ParallWatFalls->AddWaterfall(Structure);
		CallStructure.ResetMtgFlows();
		CallStructure=Structure;
		CallStructure.SetUseCall(true);
		ParallWatFalls->AddWaterfall(CallStructure);
		ParallWatFalls->StartCalculation();
	}
}
void CentralUnit::CheckCalculationDone()
{
	QString Filename=FolderPath+"\\BaseCase.clo";
	#ifndef Q_WS_WIN
	Filename.prepend('.');
	#endif
	QFile file(Filename);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_4_8);
		out << Structure << CallStructure;
		file.close();
		#ifdef Q_WS_WIN
		SetFileAttributes(Filename.toStdWString().c_str(),FILE_ATTRIBUTE_HIDDEN);
		#endif
	}
	Tranche TempTranche;
	if(!ParallWatFalls->GetWaterfalls().isEmpty()){
		Structure=*(ParallWatFalls->GetWaterfalls().at(0));
		CallStructure=*(ParallWatFalls->GetWaterfalls().at(1));
	}
		ExcelCommons::InitExcelOLE();
		if(!MtgOutputAddress.isEmpty()){
			ExcelOutput::PrintMortgagesRepLines(Structure.GetCalculatedMtgPayments(),ExcelCommons::CellOffset(MtgOutputAddress));
		}
		if(!TranchesOutputAddress.isEmpty()){
			ExcelOutput::PrintMergedCell("Scenario To Maturity",ExcelCommons::CellOffset(TranchesOutputAddress,0,0),1,6+(6*Structure.GetTranchesCount()),QColor(118,147,60));
			for(int i=0;i<Structure.GetTranchesCount();i++)
				ExcelOutput::PrintTrancheFlow(*Structure.GetTranche(i),ExcelCommons::CellOffset(TranchesOutputAddress,1,(i>0 ? 1:0) +(6*i)),i%2==0 ? QColor(235,241,222) : QColor(216,228,188),i==0);
			TempTranche.SetTrancheName("Senior Expenses");
			TempTranche.GetCashFlow().ResetFlows();
			TempTranche.AddCashFlow(Structure.GetTotalSeniorExpenses());
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,1+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			TempTranche.SetTrancheName("Senior Fees");
			TempTranche.GetCashFlow().ResetFlows();
			TempTranche.AddCashFlow(Structure.GetTotalSeniorFees());
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,2+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2!=0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			TempTranche.SetTrancheName("Junior Fees");
			TempTranche.GetCashFlow().ResetFlows();
			TempTranche.AddCashFlow(Structure.GetTotalJuniorFees());
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,3+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			
			//TODO add the other Columns
			if(RunCall){
				ExcelOutput::PrintMergedCell("Scenario To Call",ExcelCommons::CellOffset(TranchesOutputAddress,3+Structure.GetTranche(0)->GetCashFlow().Count(),0),1,6+(6*Structure.GetTranchesCount()),QColor(118,147,60));
				for(int i=0;i<CallStructure.GetTranchesCount();i++)
					ExcelOutput::PrintTrancheFlow(*CallStructure.GetTranche(i),ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(i)->GetCashFlow().Count(),(i>0 ? 1:0) +(6*i)),i%2==0 ? QColor(235,241,222) : QColor(216,228,188),i==0);
			}
		}

		
		QApplication::quit();
}
void CentralUnit::StressFinished(){
	QString Filename=(FolderPath+"\\StressResult%1%2.csr").arg(int(Stresser->GetXVariability())).arg(int(Stresser->GetYVariability()));
	#ifndef Q_WS_WIN
		Filename.prepend('.');
	#endif
	QFile file(Filename);
	if (!file.open(QIODevice::WriteOnly)) {
		QApplication::quit();
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out << Stresser;
	file.close();
	#ifdef Q_WS_WIN
		SetFileAttributes(Filename.toStdWString().c_str(),FILE_ATTRIBUTE_HIDDEN);
	#endif
	QApplication::quit();
}