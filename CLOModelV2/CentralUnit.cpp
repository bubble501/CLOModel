#include "CentralUnit.h"
#include <QMetaType>
#include <QApplication>
#include <QTimer>
#include <QFile>
#include "WaterfallCalculator.h"
#include "CommonFunctions.h"
#include <QMessageBox>
#include <QDir>
#include "ProgressWidget.h"
#include "ExcelCommons.h"
#include "ExcelOutput.h"
#include <QSettings>
CentralUnit::CentralUnit(QObject* parent)
	:QObject(parent)
	,Stresser(NULL)
	,MtgsProgress(NULL)
	,RunCall(false)
	,UseFastStress(false)
	, m_UseForwardCurve(false)
	, m_SaveBaseCase(true)
	, m_BaseCaseToCall(true)
{
	for(int i=0;i<NumberOfPlots;i++) PlotIndexes[i]=0;
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
void CentralUnit::AddLoan(const QDate& Maturity, double Size, const QString& Interest, const QString& Annuity, const QString& Freq, const QString& floatBase, const QString& LossMult, const QString& PrepayMult, const QString& HaicutVect) {
	Mortgage TempMtg;
	TempMtg.SetMaturityDate(Maturity);
	TempMtg.SetSize(Size);
	TempMtg.SetPrepayMultiplier(PrepayMult);
	TempMtg.SetLossMultiplier(LossMult);
	TempMtg.SetInterest(Interest);
	TempMtg.SetFloatingRateBase(floatBase);
	TempMtg.SetPaymentFreq(Freq);
	TempMtg.SetAnnuity(Annuity);
	TempMtg.SetHaircutVector(HaicutVect);
	LoansCalculator.AddLoan(TempMtg);
	if(Stresser)Stresser->AddLoan(TempMtg);
}
#ifndef NO_BLOOMBERG
void CentralUnit::AddTranche(const QString& Name,int ProRataGroup, double MinOC, double MinIC, double Price,double FxRate,const QString& BbgExt){
	Tranche TempTrnch;
	TempTrnch.SetTrancheName(Name);
	TempTrnch.SetProrataGroup(ProRataGroup);
	TempTrnch.SetMinIClevel(MinIC);
	TempTrnch.SetMinOClevel(MinOC);
	TempTrnch.SetPrice(Price);
	TempTrnch.SetExchangeRate(FxRate);
	TempTrnch.SetBloombergExtension(BbgExt);
	TempTrnch.GetDataFromBloomberg();
	Structure.AddTranche(TempTrnch);
	if(Stresser)Stresser->SetStructure(Structure);
}
#endif
void CentralUnit::AddTranche(
	const QString& Name
	, const QString& ISIN
	, int ProRataGroup
	, double OrigAmnt
	, const QString& Crncy
	, double OutstandingAmt
	, const QList<Tranche::TrancheInterestType>& IntrTyp
	, const QList< QString>& Coupon
	, const  QList< QString>& RefRte
	, const QDate& LastPayDte
	, const QString& DefRefRte
	, const QString& PayFreq
	, const QDate& SettlementDate
	, double StartingDeferredInterest
	//,const QString& RefRteValue
	, double MinOC
	, double MinIC
	, double Price
	, double FxRate
	, const QString& BbgExt
	, DayCountConvention DayCount
){
	Tranche TempTrnch;
	TempTrnch.SetTrancheName(Name);
	TempTrnch.SetISIN(ISIN);
	TempTrnch.SetProrataGroup(ProRataGroup);
	TempTrnch.SetOriginalAmount(OrigAmnt);
	TempTrnch.SetCurrency(Crncy);
	TempTrnch.SetOutstandingAmt(OutstandingAmt);
	for (int i = 0; i < IntrTyp.size();i++)
		TempTrnch.SetInterestType(IntrTyp.at(i), i);
	for (int i = 0; i < Coupon.size(); i++)
		TempTrnch.SetCoupon(Coupon.at(i), i);
	for (int i = 0; i < RefRte.size(); i++)
		TempTrnch.SetReferenceRate(RefRte.at(i), i);
	TempTrnch.SetLastPaymentDate(LastPayDte);
	TempTrnch.SetDefaultRefRate(DefRefRte);
	TempTrnch.SetPaymentFrequency(PayFreq);
	TempTrnch.SetSettlementDate(SettlementDate);
	//TempTrnch.SetReferenceRateValue(RefRteValue);
	TempTrnch.SetMinIClevel(MinIC);
	TempTrnch.SetMinOClevel(MinOC);
	TempTrnch.SetPrice(Price);
	TempTrnch.SetExchangeRate(FxRate);
	TempTrnch.SetBloombergExtension(BbgExt);
	TempTrnch.SetDayCount(DayCount);
	TempTrnch.SetStartingDeferredInterest(StartingDeferredInterest);
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
	Structure.ResetReserve();
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
	Structure.SetUseCall(StressToCall);
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
	if(Stresser) Stresser->SetUseFastVersion(UseFastStress);
	emit StressLoopStarted();
	ComputationLoop.exec();
}
void CentralUnit::CalculationStep1(){
	LoansCalculator.SetCPRass(Structure.GetReinvestmentTest().GetCPRAssumption());
	LoansCalculator.SetCDRass(Structure.GetReinvestmentTest().GetCDRAssumption());
	LoansCalculator.SetLSass(Structure.GetReinvestmentTest().GetLSAssumption());
	LoansCalculator.SetRecoveryLag(Structure.GetReinvestmentTest().GetRecoveryLag());
	LoansCalculator.SetDelinquency(Structure.GetReinvestmentTest().GetDelinquency());
	LoansCalculator.SetDelinquencyLag(Structure.GetReinvestmentTest().GetDelinquencyLag());
	LoansCalculator.SetStartDate(PoolCutOff);
	QString TmpStr = LoansCalculator.ReadyToCalculate();
	if (!TmpStr.isEmpty()) {
		QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + TmpStr);
		QApplication::quit();
		return;
	}
	if(MtgsProgress) MtgsProgress->deleteLater();
	MtgsProgress=new ProgressWidget;
	MtgsProgress->SetValue(0);
	MtgsProgress->SetTitle("Calculating Loans");
	MtgsProgress->SetMax(LoansCalculator.Count());
	connect(&LoansCalculator,SIGNAL(BeeCalculated(int)),MtgsProgress,SLOT(SetValue(int)));
	MtgsProgress->show();
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	LoansCalculator.StartCalculation();
}
void CentralUnit::CalculationStep2(){
	MtgsProgress->SetValue(0);
	MtgsProgress->SetTitle("Calculating Tranches");
	MtgsProgress->SetMax(1);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	Structure.ResetMtgFlows();
	Structure.AddMortgagesFlows(LoansCalculator.GetResult());
	Structure.SetUseCall(false);
	QString TmpStr=Structure.ReadyToCalculate();
	if(!TmpStr.isEmpty()){
		QMessageBox::critical(0,"Invalid Input","The following Inputs are missing or invalid:\n"+TmpStr);
		QApplication::quit();
		return;
	}
	if(!RunCall){
		CallStructure.ResetMtgFlows();
		CallStructure.ResetTranches();
		CallStructure.ResetSteps();
		if (!Structure.CalculateTranchesCashFlows()) {
			QMessageBox::critical(0, "Error", "Critical error in waterfall calculation");
			QApplication::quit();
			return;
		}
		CheckCalculationDone();
	}
	else{
		ParallWatFalls->ResetWaterfalls();
		ParallWatFalls->AddWaterfall(Structure);
		CallStructure.ResetMtgFlows();
		CallStructure=Structure;
		CallStructure.SetUseCall(true);
		TmpStr=CallStructure.ReadyToCalculate();
		if(!TmpStr.isEmpty()){
			QMessageBox::critical(0,"Invalid Input","The following Inputs are missing or invalid:\n"+TmpStr);
			QApplication::quit();
			return;
		}
		ParallWatFalls->AddWaterfall(CallStructure);
		ParallWatFalls->StartCalculation();
	}

}
void CentralUnit::CheckCalculationDone()
{
	MtgsProgress->SetValue(1);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	if(MtgsProgress) MtgsProgress->deleteLater();
	MtgsProgress=NULL;
	Tranche TempTranche;
	if(RunCall){
		Structure=*(ParallWatFalls->GetWaterfalls().at(0));
		CallStructure=*(ParallWatFalls->GetWaterfalls().at(1));
		if(Structure.GetTranchesCount()==0 || CallStructure.GetTranchesCount()==0){
			QMessageBox::critical(0, "Error", "Critical error in waterfall calculation");
			QApplication::quit();
			return;
		}
	}
	
	#ifdef SaveLoanTape
	{
		QFile file(FolderPath+"\\.Loans.clp");
		if (file.open(QIODevice::WriteOnly)) {
			QDataStream out(&file);
			out.setVersion(QDataStream::Qt_5_3);
			out << qint32(ModelVersionNumber) << LoansCalculator;
			file.close();
			#ifdef Q_WS_WIN
				SetFileAttributes((FolderPath+"\\.Loans.clp").toStdWString().c_str(),FILE_ATTRIBUTE_HIDDEN);
			#endif
		}
	}
	#endif
	QString Filename=FolderPath+"\\.BaseCase.clo";
	QFile file(Filename);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_5_3);
		out << qint32(ModelVersionNumber) << m_BaseCaseToCall << Structure << CallStructure;
		file.close();
	}
	if (m_SaveBaseCase) {
		QSettings ConfigIni(":/Configs/GlobalConfigs.ini", QSettings::IniFormat);
		ConfigIni.beginGroup("Folders");
		QDir UnifiedDir(ConfigIni.value("UnifiedResultsFolder","\\\\synserver2\\Company Share\\24AM\\Monitoring\\Model Results").toString());
		if (UnifiedDir.exists()) {
			QString AdjDealName = Structure.GetDealName();
			if (AdjDealName.isEmpty() && Structure.GetTranchesCount() > 0) {
				AdjDealName = Structure.GetTranche(0)->GetTrancheName();
			}
			QFile UnifiedFile(UnifiedDir.absoluteFilePath(AdjDealName + ".clom"));
			if (UnifiedFile.open(QIODevice::WriteOnly)) {
				QDataStream out(&UnifiedFile);
				out.setVersion(QDataStream::Qt_5_3);
				out << qint32(ModelVersionNumber) << LiborUpdateDate << m_UseForwardCurve << m_BaseCaseToCall << Structure << CallStructure << LoansCalculator;
				UnifiedFile.close();
			}
		}
	}
	if(!MtgOutputAddress.isEmpty()){
		ExcelOutput::PrintMortgagesRepLines(Structure.GetCalculatedMtgPayments(),ExcelCommons::CellOffset(MtgOutputAddress));
	}
	if(!PlotsSheet.isEmpty() && PlotIndexes[0]>0){
		ExcelOutput::PlotMortgagesFlows(Structure.GetCalculatedMtgPayments(),PlotsSheet,PlotIndexes[0]);
	}
	if(!TranchesOutputAddress.isEmpty()){
		int ClolumnCount=0;
		QList<double> TempValList;
		TrancheCashFlow TempTrancheFlow;
		ExcelOutput::PrintMergedCell("Scenario To Maturity",ExcelCommons::CellOffset(TranchesOutputAddress,0,0),1,6+(6*Structure.GetTranchesCount()),QColor(118,147,60));
		for(int i=0;i<Structure.GetTranchesCount();i++)
			ExcelOutput::PrintTrancheFlow(*Structure.GetTranche(i),ExcelCommons::CellOffset(TranchesOutputAddress,1,(i>0 ? 1:0) +(6*i)),i%2==0 ? QColor(235,241,222) : QColor(216,228,188),i==0);
		TempTranche.SetTrancheName("Senior Expenses");
		TempTranche.GetCashFlow().ResetFlows();
		TempTrancheFlow.Clear();
		TempTrancheFlow.AddFlow(Structure.GetTotalSeniorExpenses());
		TempTranche.AddCashFlow(TempTrancheFlow);
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,(++ClolumnCount)+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		TempTranche.SetTrancheName("Senior Fees");
		TempTranche.GetCashFlow().ResetFlows();
		TempTrancheFlow.Clear();
		TempTrancheFlow.AddFlow(Structure.GetTotalSeniorFees());
		TempTranche.AddCashFlow(TempTrancheFlow);
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,(++ClolumnCount)+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2!=0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		TempTranche.SetTrancheName("Junior Fees");
		TempTranche.GetCashFlow().ResetFlows();
		TempTrancheFlow.Clear();
		TempTrancheFlow.AddFlow(Structure.GetTotalJuniorFees());
		TempTranche.AddCashFlow(TempTrancheFlow);
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,(++ClolumnCount)+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		for (int ResIter = 0; ResIter<Structure.GetNumReserves(); ResIter++) {
			TempTranche.SetTrancheName(QString("Reserve Fund %1").arg(ResIter+1));
			TempTranche.GetCashFlow().ResetFlows();
			TempTrancheFlow.Clear();
			TempTrancheFlow.AddFlow(Structure.GetReserveFund(ResIter)->GetReserveFundFlow());
			TempTranche.AddCashFlow(TempTrancheFlow);
			if(TempTranche.GetCashFlow().Count()>0)
				ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,(++ClolumnCount)+(6*Structure.GetTranchesCount())),(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		}
		if(Structure.GetExcessCashFlow().Count()>0){
			TempValList.clear(); for (int i = 0; i<Structure.GetExcessCashFlow().Count(); i++) TempValList.append(Structure.GetExcessCashFlow().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + Structure.GetExcessCashFlow().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
			ExcelOutput::PrintColumn("Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,(++ClolumnCount)+(6*Structure.GetTranchesCount())),"_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		}
		TempValList.clear(); for(int i=0;i<Structure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(Structure.GetAnnualizedExcess(i));
		ExcelOutput::PrintColumn("Annualized Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,(++ClolumnCount)+(6*Structure.GetTranchesCount())),"0.00%",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		TempValList.clear(); for(int i=0;i<Structure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(Structure.GetWACostOfCapital(i));
		ExcelOutput::PrintColumn("WA Cost of Funding",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,(++ClolumnCount)+(6*Structure.GetTranchesCount())),"0.00%",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		TempValList.clear(); for(int i=0;i<Structure.GetTranchesCount();i++) TempValList.append(Structure.GetTranche(i)->GetLossRate());
		ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(LossOutputAddress),"0.00%");
		TempValList.clear(); for(int i=0;i<Structure.GetTranchesCount();i++) TempValList.append(Structure.GetCreditEnhancement(i));
		ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(CreditEnanAddress),"0.00%");
		ExcelOutput::PrintMergedCell("Scenario To Maturity",ExcelCommons::CellOffset(TranchesOutputAddress,0,0),1,(++ClolumnCount)+(6*Structure.GetTranchesCount()),QColor(118,147,60));

		if(RunCall){
			ClolumnCount=0;
			ExcelOutput::PrintMergedCell("Scenario To Call",ExcelCommons::CellOffset(TranchesOutputAddress,3+Structure.GetTranche(0)->GetCashFlow().Count(),0),1,6+(6*Structure.GetTranchesCount()),QColor(118,147,60));
			for(int i=0;i<CallStructure.GetTranchesCount();i++)
				ExcelOutput::PrintTrancheFlow(*CallStructure.GetTranche(i),ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(i)->GetCashFlow().Count(),(i>0 ? 1:0) +(6*i)),i%2==0 ? QColor(235,241,222) : QColor(216,228,188),i==0);
			TempTranche.SetTrancheName("Senior Expenses");
			TempTranche.GetCashFlow().ResetFlows();
			TempTrancheFlow.Clear();
			TempTrancheFlow.AddFlow(CallStructure.GetTotalSeniorExpenses());
			TempTranche.AddCashFlow(TempTrancheFlow);
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),CallStructure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			TempTranche.SetTrancheName("Senior Fees");
			TempTranche.GetCashFlow().ResetFlows();
			TempTrancheFlow.Clear();
			TempTrancheFlow.AddFlow(CallStructure.GetTotalSeniorFees());
			TempTranche.AddCashFlow(TempTrancheFlow);
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),CallStructure.GetTranchesCount()%2!=0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			TempTranche.SetTrancheName("Junior Fees");
			TempTranche.GetCashFlow().ResetFlows();
			TempTrancheFlow.Clear();
			TempTrancheFlow.AddFlow(CallStructure.GetTotalJuniorFees());
			TempTranche.AddCashFlow(TempTrancheFlow);
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),CallStructure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			for (int ResIter = 0; ResIter<CallStructure.GetNumReserves(); ResIter++) {
				TempTranche.SetTrancheName(QString("Reserve Fund").arg(ResIter+1));
				TempTranche.GetCashFlow().ResetFlows();
				TrancheCashFlow TempTrancheFlow;
				TempTrancheFlow.AddFlow(CallStructure.GetReserveFund(ResIter)->GetReserveFundFlow());
				TempTranche.AddCashFlow(TempTrancheFlow);
				if(TempTranche.GetCashFlow().Count()>0)
					ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*Structure.GetTranchesCount())),(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			}
			if(CallStructure.GetExcessCashFlow().Count()>0){
				TempValList.clear(); for (int i = 0; i<CallStructure.GetExcessCashFlow().Count(); i++) TempValList.append(CallStructure.GetExcessCashFlow().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + CallStructure.GetExcessCashFlow().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
				ExcelOutput::PrintColumn("Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,5+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),"_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-",(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
			}
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(CallStructure.GetAnnualizedExcess(i));
			ExcelOutput::PrintColumn("Annualized Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,5+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),"0.00%",(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(CallStructure.GetWACostOfCapital(i));
			ExcelOutput::PrintColumn("WA Cost of Funding",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,5+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),"0.00%",(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranchesCount();i++) TempValList.append(CallStructure.GetTranche(i)->GetLossRate());
			ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(LossOnCallOutputAddress),"0.00%");
			ExcelOutput::PrintMergedCell("Scenario To Call",ExcelCommons::CellOffset(TranchesOutputAddress,3+Structure.GetTranche(0)->GetCashFlow().Count(),0),1,(++ClolumnCount)+(6*Structure.GetTranchesCount()),QColor(118,147,60));
		}
	}
	if(!PlotsSheet.isEmpty() && PlotIndexes[1]>0)
		ExcelOutput::PlotTranchesDynamic(Structure,PlotsSheet,PlotIndexes[1],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[2]>0)
		ExcelOutput::PlotOCTest(Structure,PlotsSheet,PlotIndexes[2],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[3]>0)
		ExcelOutput::PlotICTest(Structure,PlotsSheet,PlotIndexes[3],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[4]>0)
		ExcelOutput::PlotAnnualExcess(Structure,PlotsSheet,PlotIndexes[4],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[5]>0)
		ExcelOutput::PlotCostFunding(Structure,PlotsSheet,PlotIndexes[5],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[6]>0)
		ExcelOutput::PlotCPRLS(Structure,PlotsSheet,PlotIndexes[6]);
	if(!PlotsSheet.isEmpty() && PlotIndexes[7]>0)
		ExcelOutput::PlotEquityReturn(Structure,PlotsSheet,PlotIndexes[7]);
	if(!PlotsSheet.isEmpty() && PlotIndexes[8]>0)
		ExcelOutput::PlotCallToEquity(Structure,PlotsSheet,PlotIndexes[8],RunCall ? CallStructure.GetCalledPeriod():QDate());
	QApplication::quit();
}
void CentralUnit::StressFinished(){
	Stresser->SaveResults(FolderPath);
	QApplication::quit();
}
CentralUnit::~CentralUnit(){
	if(MtgsProgress) MtgsProgress->deleteLater();
}
void CentralUnit::CompileBaseRates(ConstantBaseRateTable& Values)const {
	for(int i=0;i<Structure.GetTranchesCount();i++){
		Structure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
	for(int i=0;i<CallStructure.GetTranchesCount();i++){
		CallStructure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
	LoansCalculator.CompileReferenceRateValue(Values);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = false;
}
void CentralUnit::CompileBaseRates(ForwardBaseRateTable& Values)const {
	for (int i = 0; i < Structure.GetTranchesCount(); i++) {
		Structure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
	for (int i = 0; i < CallStructure.GetTranchesCount(); i++) {
		CallStructure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
	LoansCalculator.CompileReferenceRateValue(Values);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = true;
}
#ifndef NO_DATABASE
void CentralUnit::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) const {
	for (int i = 0; i < Structure.GetTranchesCount(); i++) {
		Structure.GetTranche(i)->GetBaseRatesDatabase(Values, DownloadAll);
	}
	for (int i = 0; i < CallStructure.GetTranchesCount(); i++) {
		CallStructure.GetTranche(i)->GetBaseRatesDatabase(Values, DownloadAll);
	}
	LoansCalculator.GetBaseRatesDatabase(Values, DownloadAll);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = false;
}
void CentralUnit::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) const {
	for (int i = 0; i < Structure.GetTranchesCount(); i++) {
		Structure.GetTranche(i)->GetBaseRatesDatabase(Values, DownloadAll);
	}
	for (int i = 0; i < CallStructure.GetTranchesCount(); i++) {
		CallStructure.GetTranche(i)->GetBaseRatesDatabase(Values, DownloadAll);
	}
	LoansCalculator.GetBaseRatesDatabase(Values, DownloadAll);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = true;
}
#endif
