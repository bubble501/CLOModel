#include "CentralUnit.h"
#include <QMetaType>
#include <QApplication>
#include <QFile>
#include <WaterfallCalculator.h>
#include <CommonFunctions.h>
#include <QMessageBox>
#include <QDir>
#include "ExcelCommons.h"
#include "ExcelOutput.h"
#include <QStringList>
#include <QIcon>
#include <QLabel>
#include <QStandardItemModel>
#include <QListView>
#include <QGridLayout>
#include <QPushButton>
#include <Mortgage.h>
#include <Tranche.h>
#include <ReinvestmentTest.h>
#include <ReserveFund.h>
CentralUnit::CentralUnit(QObject* parent)
	:QObject(parent)
	,Stresser(nullptr)
	,MtgsProgress(nullptr)
	,RunCall(false)
	,UseFastStress(false)
	, m_UseForwardCurve(false)
	, m_SaveBaseCase(true)
	, m_BaseCaseToCall(true)
	, LastRateTable(nullptr)
{
	for(int i=0;i<NumberOfPlots;i++) PlotIndexes[i]=0;
    if (QMetaType::type("Waterfall")==QMetaType::UnknownType) {
        qRegisterMetaTypeStreamOperators<Waterfall>(typeid(Waterfall).name());
    }
    if (QMetaType::type("MtgCashFlow")==QMetaType::UnknownType) {
        qRegisterMetaTypeStreamOperators<MtgCashFlow>(typeid(MtgCashFlow).name());
    }
	connect(&LoansCalculator,SIGNAL(Calculated()),this,SLOT(CalculationStep2()));
	connect(this,SIGNAL(LoopStarted()),this,SLOT(CalculationStep1()),Qt::QueuedConnection);
	ParallWatFalls=new WaterfallCalculator(this);
	connect(ParallWatFalls,SIGNAL(Calculated()),this,SLOT(CheckCalculationDone()));
}
void CentralUnit::SetPoolCutOff(const QDate& a) { PoolCutOff = a; if (Stresser) Stresser->SetStartDate(PoolCutOff); }
void CentralUnit::SetFolderPath(const QString& a){FolderPath=a;}
void CentralUnit::AddLoan(
	const QDate& Maturity
	, double Size
	, const QString& Interest
	, const QString& Annuity
	, const QString& Freq
	, const QString& floatBase
	, const QString& Properties
	) {
	Mortgage TempMtg;
	TempMtg.SetMaturityDate(Maturity);
	TempMtg.SetSize(Size);
	TempMtg.SetInterest(Interest);
	TempMtg.SetFloatingRateBase((floatBase.isEmpty())? QString("ZERO"):floatBase);
	TempMtg.SetPaymentFreq(Freq);
	TempMtg.SetAnnuity(Annuity);
	QStringList PropList = Properties.split("#,#");
	foreach(const QString& SingleProp, PropList) {
		QStringList KeyVal = SingleProp.split("#=#");
		if (KeyVal.size() != 2) continue;
		TempMtg.SetProperty(KeyVal.first(), KeyVal.at(1));
	}
	LoansCalculator.AddLoan(TempMtg,LoansCalculator.NumBees());
	if (Stresser)Stresser->AddLoan(TempMtg);
}
#ifndef NO_BLOOMBERG
void CentralUnit::AddTranche(const QString& Name, const QString& ProRataGroup, double MinOC, double MinIC, double Price, double FxRate, const QString& BbgExt) {
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
	if (Stresser)Stresser->SetStructure(Structure);
}
#endif
void CentralUnit::AddTranche(
	const QString& Name
	, const QString& ISIN
	, const QString& ProRataGroup
	, double OrigAmnt
	, const QString& Crncy
	, double OutstandingAmt
	, const QList< QString>& Coupon
	, const  QList< QString>& RefRte
	, const QDate& LastPayDte
	, const QString& DefRefRte
	, const QString& PayFreq
	, const QDate& SettlementDate
	, double StartingDeferredInterest
	, double MinOC
	, double MinIC
	, double Price
	, double FxRate
	, const QString& BbgExt
    , const QList<QString>& DayCount
){
	Tranche TempTrnch;
	TempTrnch.SetTrancheName(Name);
	TempTrnch.SetISIN(ISIN);
	TempTrnch.SetProrataGroup(ProRataGroup);
	TempTrnch.SetOriginalAmount(OrigAmnt);
	TempTrnch.SetCurrency(Crncy);
	TempTrnch.SetOutstandingAmt(OutstandingAmt);
	for (int i = 0; i < Coupon.size(); i++)
		TempTrnch.SetCoupon(Coupon.at(i), i);
	for (int i = 0; i < RefRte.size(); i++)
		TempTrnch.SetReferenceRate(RefRte.at(i), i);
    for (int i = 0; i < DayCount.size(); i++)
        TempTrnch.SetDayCount(DayCount.at(i), i);
	TempTrnch.SetLastPaymentDate(LastPayDte);
	TempTrnch.SetDefaultRefRate(DefRefRte);
	TempTrnch.SetPaymentFrequency(PayFreq);
	TempTrnch.SetSettlementDate(SettlementDate);
	TempTrnch.SetMinIClevel(MinIC);
	TempTrnch.SetMinOClevel(MinOC);
	TempTrnch.SetPrice(Price);
	TempTrnch.SetExchangeRate(FxRate);
	TempTrnch.SetBloombergExtension(BbgExt);
	TempTrnch.SetStartingDeferredInterest(StartingDeferredInterest);
	Structure.AddTranche(TempTrnch);
	if (Stresser)Stresser->SetStructure(Structure);
}
void CentralUnit::AddWaterfallStep(
	WatFalPrior::WaterfallStepType Tpe
	, const QString& ArgSeniorityGroup
	, const QString& ArgSeniorityGroupLevel
	, const QString& ArgRedemptionGroup
	, const QString& ArgRedemptionGroupLevel
	, const QString& ArgRedemptionShare
	, const QString& ArgAdditionalCollateralShare
	, const QString& ArgSourceofFunding
	, const QString& ArgCouponIndex
	, const QString& ArgTestTargetOverride
	, const QString& ArgIRRtoEquityTarget
	, const QString& ArgReserveIndex
	, const QString& ArgTrigger
	, int ArgAccruePay
	){
	WatFalPrior TempStep;
	if (!ArgSeniorityGroup.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::SeniorityGroup, ArgSeniorityGroup); }
	if (!ArgSeniorityGroupLevel.isEmpty()) { 
        TempStep.SetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel, ArgSeniorityGroupLevel); 
    }
	if (!ArgRedemptionGroup.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::RedemptionGroup, ArgRedemptionGroup); }
	if (!ArgRedemptionGroupLevel.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel, ArgRedemptionGroupLevel); }
	if (!ArgRedemptionShare.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::RedemptionShare, ArgRedemptionShare); }
	if (!ArgAdditionalCollateralShare.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare, ArgAdditionalCollateralShare); }
	if (!ArgSourceofFunding.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::SourceOfFunding, ArgSourceofFunding); }
	if (!ArgCouponIndex.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::CouponIndex, ArgCouponIndex); }
	if (!ArgTestTargetOverride.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::TestTargetOverride, ArgTestTargetOverride); }
	if (!ArgIRRtoEquityTarget.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::IRRtoEquityTarget, ArgIRRtoEquityTarget); }
	if (!ArgReserveIndex.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::ReserveIndex, ArgReserveIndex); }
	if (!ArgTrigger.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::Trigger, ArgTrigger); }
	TempStep.SetParameter(WatFalPrior::wstParameters::PayAccrue, QString::number(ArgAccruePay));
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
	if(Stresser) Stresser->deleteLater();
	if (LastRateTable) { delete LastRateTable; LastRateTable = nullptr; }
	if (MtgsProgress) MtgsProgress->deleteLater();
}

void CentralUnit::SetupStress(const QString& ConstPar,QList<QString> XSpann,QList<QString> YSpann, int Xvar,int Yvar){
	if(!Stresser){
		Stresser=new StressTest(this);
	}
	/*
	ChangingCDR = 0x1
	ChangingLS = 0x2
	ChangingCPR = 0x4
	*/
	switch (Xvar) {
	case 0x1:
		foreach(const QString& SingleScen, XSpann)
			{Stresser->AddCDRscenarios(SingleScen);}
		break;
	case 0x2:
		foreach(const QString& SingleScen, XSpann) {
			Stresser->AddLSscenarios(SingleScen);
		}
		break;
	case 0x4:
		foreach(const QString& SingleScen, XSpann) {
			Stresser->AddCPRscenarios(SingleScen);
		}
		break;
	default: 
		Stresser->deleteLater();
		PrintToTempFile("Error In Calculation", "Invalid Stress Dimension");
		return;
	}
	switch (Yvar) {
	case 0x1:
		foreach(const QString& SingleScen, YSpann) {
			Stresser->AddCDRscenarios(SingleScen);
		}
		break;
	case 0x2:
		foreach(const QString& SingleScen, YSpann) {
			Stresser->AddLSscenarios(SingleScen);
		}
		break;
	case 0x4:
		foreach(const QString& SingleScen, YSpann) {
			Stresser->AddCPRscenarios(SingleScen);
		}
		break;
	default:
		Stresser->deleteLater();
		PrintToTempFile("Error In Calculation", "Invalid Stress Dimension");
		return;
	}
	switch ((~(Xvar | Yvar)) & 0x7) {
	case 0x1: Stresser->AddCDRscenarios(ConstPar); break;
	case 0x2: Stresser->AddLSscenarios(ConstPar); break;
	case 0x4: Stresser->AddCPRscenarios(ConstPar); break;
	default:
		Stresser->deleteLater();
		PrintToTempFile("Error In Calculation", "Invalid Stress Dimension");
		return;
	}
	Stresser->SetStartDate(PoolCutOff);
	Structure.SetUseCall(StressToCall);
	Stresser->SetStructure(Structure);
	const auto& TempLoans = LoansCalculator.GetLoans();
	LOGDEBUG(QString("Loans in stress test: %1\nTranches in Stress: %2").arg(TempLoans.size()).arg(Stresser->GetStructure().GetTranchesCount()));
	for (auto i = TempLoans.constBegin(); i != TempLoans.constEnd(); ++i)
		Stresser->AddLoan(*(i.value()));
	if (LastRateTable) {
		if (m_UseForwardCurve) Stresser->CompileBaseRates(*(dynamic_cast<ForwardBaseRateTable*>(LastRateTable)));
		else Stresser->CompileBaseRates(*(dynamic_cast<ConstantBaseRateTable*>(LastRateTable)));
	}
	connect(this,SIGNAL(StressLoopStarted()),Stresser,SLOT(RunStressTest()),Qt::QueuedConnection);
	connect(Stresser,SIGNAL(AllFinished()),this,SLOT(StressFinished()));
}
void CentralUnit::Calculate(){
	char *argv[] = {"NoArgumnets"};
	int argc = 1;
	QApplication ComputationLoop(argc,argv);
	emit LoopStarted();
	ComputationLoop.exec();
}
void CentralUnit::CalculateStress(){
	char *argv[] = {"NoArgumnets"};
	int argc =  1;
	QApplication ComputationLoop(argc,argv);
	if(Stresser) Stresser->SetUseFastVersion(UseFastStress);
	emit StressLoopStarted();
	ComputationLoop.exec();
}
void CentralUnit::CalculationStep1(){
	LOGDEBUG("Reached CalculationStep1");
	LoansCalculator.DownloadScenarios();
	LoansCalculator.SetCPRass(Structure.GetReinvestmentTest().GetCPRAssumption().GetVector());
	LoansCalculator.SetCDRass(Structure.GetReinvestmentTest().GetCDRAssumption().GetVector());
	LoansCalculator.SetLSass(Structure.GetReinvestmentTest().GetLSAssumption().GetVector());
	LoansCalculator.SetRecoveryLag(Structure.GetReinvestmentTest().GetRecoveryLag().GetVector());
	LoansCalculator.SetDelinquency(Structure.GetReinvestmentTest().GetDelinquency().GetVector());
	LoansCalculator.SetDelinquencyLag(Structure.GetReinvestmentTest().GetDelinquencyLag().GetVector());
	LoansCalculator.SetStartDate(PoolCutOff);
#ifdef SAVE_EXCEL_INPUTS
    QString Filename = "C:\\Temp\\ModelInputs";
    QFile file(Filename);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out.setVersion(StreamVersionUsed);
        out << qint32(ModelVersionNumber) << LoansCalculator << Structure;
        file.close();
    }
#endif // SAVE_EXCEL_INPUTS

    //Add Fake cash flows to structure to check inputs
    {
        MtgCashFlow FakeFlow;
        FakeFlow.AddFlow(QDate(2015, 5, 30), 100.0, MtgCashFlow::MtgFlowType::PrincipalFlow);
        Structure.AddMortgagesFlows(FakeFlow);
        Structure.SetUseCall(false);
    }

	QString TmpStr = LoansCalculator.ReadyToCalculate();
    if (Q_UNLIKELY(!TmpStr.isEmpty()))
        TmpStr += '\n';
    TmpStr += Structure.ReadyToCalculate();
    if (Q_UNLIKELY(!TmpStr.isEmpty())) {
        showErrorDialog(TmpStr);
        //QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + TmpStr);
        QApplication::quit();
        return;
    }
	if(MtgsProgress) MtgsProgress->deleteLater();
	MtgsProgress=new QProgressDialog();
	MtgsProgress->setWindowIcon(QIcon(":/Icons/Logo.png"));
	MtgsProgress->setWindowTitle(tr("Please Wait"));
	MtgsProgress->setRange(0, 100);
	MtgsProgress->setValue(0);
	MtgsProgress->setAutoClose(false);
	MtgsProgress->setLabelText(tr("Calculating Loans"));
	MtgsProgress->setCancelButtonText(tr("Cancel"));
	connect(&LoansCalculator, &MtgCalculator::ProgressPct, MtgsProgress, &QProgressDialog::setValue);
	connect(MtgsProgress, &QProgressDialog::canceled, &LoansCalculator, &MtgCalculator::StopCalculation);
	MtgsProgress->show();
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	if (!LoansCalculator.StartCalculation()) {
		QMessageBox::critical(0, "Invalid Input", "A base rate in the loans is invalid");
		QApplication::quit();
		return;
	}
}

void CentralUnit::showErrorDialog(const QString& errList)
{
    auto errorList = errList.split('\n');
    QDialog errorDialog;
    errorDialog.setWindowTitle(tr("Invalid Input"));
    errorDialog.setWindowIcon(QIcon(":/Icons/Logo.png"));
    QGridLayout* mainLay = new QGridLayout(&errorDialog);
    QLabel* mainLabel = new QLabel(&errorDialog);
    mainLabel->setText(tr("The following Inputs are missing or invalid:"));
    mainLay->addWidget(mainLabel, 0, 0, 1, 3);
    QStandardItemModel* mainModel = new QStandardItemModel(&errorDialog);
    mainModel->setColumnCount(1);
    mainModel->setRowCount(errorList.size());
    for (int i = 0; i < errorList.size(); ++i)
        mainModel->setData(mainModel->index(i, 0), errorList.at(i));
    QListView* mainView = new QListView(&errorDialog);
    mainView->setModel(mainModel);
    mainLay->addWidget(mainView, 1, 0, 1, 3);
    QPushButton* okButton = new QPushButton(&errorDialog);
    okButton->setDefault(true);
    okButton->setText(tr("Ok"));
    connect(okButton, &QPushButton::clicked, &errorDialog, &QDialog::accept);
    mainLay->addWidget(okButton, 2, 1);
    errorDialog.exec();
}

void CentralUnit::CalculationStep2()
{
	LOGDEBUG("Reached CalculationStep2");
	MtgsProgress->setLabelText(tr("Calculating Tranches"));
	MtgsProgress->setAutoClose(true);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	Structure.ResetMtgFlows();
	if (Q_LIKELY(LoansCalculator.GetAggregatedResults())) 
        Structure.AddMortgagesFlows(*LoansCalculator.GetAggregatedResults());
	Structure.SetUseCall(false);
	QString TmpStr=Structure.ReadyToCalculate();
    if (Q_UNLIKELY(!TmpStr.isEmpty())) {
        showErrorDialog(TmpStr);
		//QMessageBox::critical(0,"Invalid Input","The following Inputs are missing or invalid:\n"+TmpStr);
		QApplication::quit();
		return;
	}
#ifdef SAVE_EXCEL_INPUTS
	QString Filename = "C:\\Temp\\.SavedInputs.clo";
	QFile file(Filename);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(StreamVersionUsed);
		out << qint32(ModelVersionNumber) << m_BaseCaseToCall << Structure << Waterfall();
		file.close();
	}
#endif // SAVE_EXCEL_INPUTS
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
		ParallWatFalls->ClearResults();
		ParallWatFalls->AddWaterfall(Structure,0);
		CallStructure.ResetMtgFlows();
		CallStructure=Structure;
		CallStructure.SetUseCall(true);
		TmpStr=CallStructure.ReadyToCalculate();
		if(!TmpStr.isEmpty()){
			QMessageBox::critical(0,"Invalid Input","The following Inputs are missing or invalid:\n"+TmpStr);
			QApplication::quit();
			return;
		}
		ParallWatFalls->AddWaterfall(CallStructure,1);
		connect(ParallWatFalls, &WaterfallCalculator::ProgressPct, MtgsProgress, &QProgressDialog::setValue);
		disconnect(MtgsProgress, &QProgressDialog::canceled,nullptr,nullptr);
		connect(MtgsProgress, &QProgressDialog::canceled, ParallWatFalls, &WaterfallCalculator::StopCalculation);
		ParallWatFalls->StartCalculation();
	}

}
void CentralUnit::CheckCalculationDone()
{
	LOGDEBUG("Reached Calculation Done");
	MtgsProgress->setValue(100);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	if(MtgsProgress) MtgsProgress->deleteLater();
	Tranche TempTranche;
	if(RunCall){
		Structure=*(ParallWatFalls->GetResult(0));
		CallStructure = *(ParallWatFalls->GetResult(1));
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
			out.setVersion(StreamVersionUsed);
			out << qint32(ModelVersionNumber) << LoansCalculator;
			file.close();
			#ifdef Q_WS_WIN
				SetFileAttributes((FolderPath+"\\.Loans.clp").toStdWString().c_str(),FILE_ATTRIBUTE_HIDDEN);
			#endif
		}
	}
	#endif
	QString Filename=FolderPath+"\\BaseCase.clo";
	QFile file(Filename);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(StreamVersionUsed);
		out << qint32(ModelVersionNumber) << m_BaseCaseToCall << Structure << CallStructure;
		file.close();
	}
	emit CalculationFinished();
	if (m_SaveBaseCase) {
        // Save it to database
        const Waterfall& applicableStructure = (m_BaseCaseToCall && RunCall) ? CallStructure : Structure;
        QString notLoadedString(tr("Could not update cashflows in the database for the following bonds:"));
        bool loadFailed = false;
        for (int tranIter = 0; tranIter < applicableStructure.GetTranchesCount(); ++tranIter) {
            if (!applicableStructure.GetTranche(tranIter)->saveCashflowsDatabase()) {
                loadFailed = true;
                notLoadedString += "\n" + applicableStructure.GetTranche(tranIter)->GetTrancheName();
            }
        }
        if (loadFailed)
            QMessageBox::warning(nullptr, tr("Database Upload Failed"), notLoadedString);
        // Save it to File
		QDir UnifiedDir(GetFromConfig("Folders", "UnifiedResultsFolder"));
		if (UnifiedDir.exists()) {
			QString AdjDealName = Structure.GetDealName();
			if (AdjDealName.isEmpty() && Structure.GetTranchesCount() > 0) {
				AdjDealName = Structure.GetTranche(0)->GetTrancheName();
			}
			QFile UnifiedFile(UnifiedDir.absoluteFilePath(AdjDealName + ".clom"));
			if (UnifiedFile.open(QIODevice::WriteOnly)) {
				QDataStream out(&UnifiedFile);
				out.setVersion(StreamVersionUsed);
				out 
					<< qint32(ModelVersionNumber) 
					<< LiborUpdateDate 
					<< m_UseForwardCurve 
					<< m_BaseCaseToCall 
					<< Structure 
					<< CallStructure 
					<< LoansCalculator
					<< m_OverrideConstants
					<< m_OverrideForwards
				;
				UnifiedFile.close();
			}
		}
	}
	if(!MtgOutputAddress.isEmpty()){
		#ifdef PrintAggregatedMtgFlows
			ExcelOutput::PrintMortgagesRepLines(Structure.GetAggregatedMtgFlows(),ExcelCommons::CellOffset(MtgOutputAddress));
		#else
			ExcelOutput::PrintMortgagesRepLines(Structure.GetCalculatedMtgPayments(), ExcelCommons::CellOffset(MtgOutputAddress));
		#endif
	}
	if(!PlotsSheet.isEmpty() && PlotIndexes[0]>0){
		ExcelOutput::PlotMortgagesFlows(Structure.GetCalculatedMtgPayments(),PlotsSheet,PlotIndexes[0]);
	}
	if(!TranchesOutputAddress.isEmpty()){
		int ClolumnCount=0;
		QList<double> TempValList;
		TrancheCashFlow TempTrancheFlow;
		for (int i = 0; i < Structure.GetTranchesCount(); i++)
		{
			
			ExcelOutput::PrintTrancheFlow(*Structure.GetTranche(i), ExcelCommons::CellOffset(TranchesOutputAddress, 1, (i>0 ? 1 : 0) + ClolumnCount), i % 2 == 0 ? QColor(235, 241, 222) : QColor(216, 228, 188), i == 0, true, true, true, false
				, Structure.GetTranche(i)->GetCashFlow().HasDeferred()
				, Structure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCFlow))
				, Structure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICFlow))
				, Structure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLCured)) || Structure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLOutstanding))
				);
			ClolumnCount += 3
                + (Structure.GetTranche(i)->GetCashFlow().HasDeferred() ? 1 : 0)
				+ (Structure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCFlow)) ? 1 : 0)
				+ (Structure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICFlow)) ? 1 : 0)
				+ (Structure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLCured)) || Structure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLOutstanding)) ? 2 : 0)
				;
		}
		TempTranche.SetTrancheName("Senior Expenses");
		TempTranche.GetCashFlow().ResetFlows();
		TempTrancheFlow.Clear();
		TempTrancheFlow.AddFlow(Structure.GetTotalSeniorExpenses());
		TempTranche.AddCashFlow(TempTrancheFlow);
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,++ClolumnCount),Structure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		TempTranche.SetTrancheName("Senior Fees");
		TempTranche.GetCashFlow().ResetFlows();
		TempTrancheFlow.Clear();
		TempTrancheFlow.AddFlow(Structure.GetTotalSeniorFees());
		TempTranche.AddCashFlow(TempTrancheFlow);
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,++ClolumnCount),Structure.GetTranchesCount()%2!=0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		TempTranche.SetTrancheName("Junior Fees");
		TempTranche.GetCashFlow().ResetFlows();
		TempTrancheFlow.Clear();
		TempTrancheFlow.AddFlow(Structure.GetTotalJuniorFees());
		TempTranche.AddCashFlow(TempTrancheFlow);
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,++ClolumnCount),Structure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		for (int ResIter = 0; ResIter<Structure.GetNumReserves(); ResIter++) {
            if (Structure.GetReserveFund(ResIter)->GetReserveFundFlow().IsEmpty())
                continue;
			TempTranche.SetTrancheName(QString("Reserve Fund %1").arg(ResIter+1));
			TempTranche.GetCashFlow().ResetFlows();
			TempTrancheFlow.Clear();
			TempTrancheFlow.SetFlow(Structure.GetReserveFund(ResIter)->GetReserveFundFlow());
			TempTranche.SetCashFlow(TempTrancheFlow);
			if(TempTranche.GetCashFlow().Count()>0)
				ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,++ClolumnCount),(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		}
		if (!Structure.GetGICflows().IsEmpty()) {
			GenericCashFlow TempAggregatedFlows = Structure.GetAggregatedGIC();
			const TrancheCashFlow& TempBaseFlows = Structure.GetTranche(0)->GetCashFlow();
			TempValList.clear(); for (int i = 0; i < TempBaseFlows.Count(); i++) TempValList.append(TempAggregatedFlows.GetFlow(TempBaseFlows.GetDate(i), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + TempAggregatedFlows.GetFlow(TempBaseFlows.GetDate(i), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
			ExcelOutput::PrintColumn("GIC Interest", TempValList, ExcelCommons::CellOffset(TranchesOutputAddress, 2, (++ClolumnCount)), "_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-", (Structure.GetTranchesCount() + ClolumnCount) % 2 == 0 ? QColor(235, 241, 222) : QColor(216, 228, 188));
		}
		if (!Structure.GetReinvested().IsEmpty()) {
			LOGDEBUG(Structure.GetReinvested().ToString());
			GenericCashFlow TempAggregatedFlows = Structure.GetAggregatedReinvestment();
			const TrancheCashFlow& TempBaseFlows = Structure.GetTranche(0)->GetCashFlow();
			TempValList.clear(); for (int i = 0; i < TempBaseFlows.Count(); i++) TempValList.append(TempAggregatedFlows.GetFlow(TempBaseFlows.GetDate(i), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + TempAggregatedFlows.GetFlow(TempBaseFlows.GetDate(i), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
			ExcelOutput::PrintColumn("Reinvestments", TempValList, ExcelCommons::CellOffset(TranchesOutputAddress, 2, (++ClolumnCount)), "_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-", (Structure.GetTranchesCount() + ClolumnCount) % 2 == 0 ? QColor(235, 241, 222) : QColor(216, 228, 188));
		}
		if (!Structure.GetExcessCashFlow().IsEmpty()) {
			TempValList.clear(); for (int i = 0; i<Structure.GetExcessCashFlow().Count(); i++) TempValList.append(Structure.GetExcessCashFlow().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + Structure.GetExcessCashFlow().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
			ExcelOutput::PrintColumn("Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,++ClolumnCount),"_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		}
		TempValList.clear(); for(int i=0;i<Structure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(Structure.GetAnnualizedExcess(i));
		ExcelOutput::PrintColumn("Annualized Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,++ClolumnCount),"0.00%",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		TempValList.clear(); for(int i=0;i<Structure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(Structure.GetWACostOfCapital(i));
		ExcelOutput::PrintColumn("WA Cost of Funding",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,++ClolumnCount),"0.00%",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		TempValList.clear(); for(int i=0;i<Structure.GetTranchesCount();i++) TempValList.append(Structure.GetTranche(i)->GetLossRate());
		ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(LossOutputAddress),"0.00%");
		TempValList.clear(); for(int i=0;i<Structure.GetTranchesCount();i++) TempValList.append(Structure.GetCreditEnhancement(i));
		ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(CreditEnanAddress),"0.00%");
		ExcelOutput::PrintMergedCell("Scenario To Maturity",ExcelCommons::CellOffset(TranchesOutputAddress,0,0),1,++ClolumnCount,QColor(118,147,60));

		if(RunCall){
			ClolumnCount=0;
			for (int i = 0; i < CallStructure.GetTranchesCount(); i++) {
				ExcelOutput::PrintTrancheFlow(*CallStructure.GetTranche(i), ExcelCommons::CellOffset(TranchesOutputAddress, 4 + Structure.GetTranche(i)->GetCashFlow().Count(), (i>0 ? 1 : 0) + ClolumnCount), i % 2 == 0 ? QColor(235, 241, 222) : QColor(216, 228, 188), i == 0, true, true, true, false
					, CallStructure.GetTranche(i)->GetCashFlow().HasDeferred()
					, CallStructure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCFlow))
					, CallStructure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICFlow))
					, CallStructure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLCured)) || CallStructure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLOutstanding))
					);
				ClolumnCount += 3
                    + (CallStructure.GetTranche(i)->GetCashFlow().HasDeferred() ? 1 : 0)
					+ (CallStructure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCFlow)) ? 1 : 0)
					+ (CallStructure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICFlow)) ? 1 : 0)
					+ (CallStructure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLCured)) || CallStructure.GetTranche(i)->GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLOutstanding)) ? 2 : 0)
					;
			}
			TempTranche.SetTrancheName("Senior Expenses");
			TempTranche.GetCashFlow().ResetFlows();
			TempTrancheFlow.Clear();
			TempTrancheFlow.AddFlow(CallStructure.GetTotalSeniorExpenses());
			TempTranche.AddCashFlow(TempTrancheFlow);
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),++ClolumnCount),CallStructure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			TempTranche.SetTrancheName("Senior Fees");
			TempTranche.GetCashFlow().ResetFlows();
			TempTrancheFlow.Clear();
			TempTrancheFlow.AddFlow(CallStructure.GetTotalSeniorFees());
			TempTranche.AddCashFlow(TempTrancheFlow);
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),++ClolumnCount),CallStructure.GetTranchesCount()%2!=0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			TempTranche.SetTrancheName("Junior Fees");
			TempTranche.GetCashFlow().ResetFlows();
			TempTrancheFlow.Clear();
			TempTrancheFlow.AddFlow(CallStructure.GetTotalJuniorFees());
			TempTranche.AddCashFlow(TempTrancheFlow);
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),++ClolumnCount),CallStructure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			for (int ResIter = 0; ResIter<CallStructure.GetNumReserves(); ResIter++) {
				TempTranche.SetTrancheName(QString("Reserve Fund").arg(ResIter+1));
				TempTranche.GetCashFlow().ResetFlows();
				TrancheCashFlow TempTrancheFlow;
				TempTrancheFlow.AddFlow(CallStructure.GetReserveFund(ResIter)->GetReserveFundFlow());
				TempTranche.AddCashFlow(TempTrancheFlow);
				if(TempTranche.GetCashFlow().Count()>0)
					ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)),(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			}
			if (!CallStructure.GetGICflows().IsEmpty()) {
				GenericCashFlow TempAggregatedFlows = CallStructure.GetAggregatedGIC();
				TempValList.clear(); for (int i = 0; i < CallStructure.GetTranche(0)->GetCashFlow().Count(); i++) TempValList.append(TempAggregatedFlows.GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + TempAggregatedFlows.GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
				ExcelOutput::PrintColumn("GIC Interest", TempValList, ExcelCommons::CellOffset(TranchesOutputAddress, 5 + Structure.GetTranche(0)->GetCashFlow().Count(), ++ClolumnCount), "_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-", (CallStructure.GetTranchesCount() + ClolumnCount) % 2 == 0 ? QColor(235, 241, 222) : QColor(216, 228, 188));
			}
			if (!CallStructure.GetReinvested().IsEmpty()) {
				GenericCashFlow TempAggregatedFlows = CallStructure.GetAggregatedReinvestment();
				TempValList.clear(); for (int i = 0; i <CallStructure.GetTranche(0)->GetCashFlow().Count(); i++) TempValList.append(TempAggregatedFlows.GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + TempAggregatedFlows.GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
				ExcelOutput::PrintColumn("Reinvestments", TempValList, ExcelCommons::CellOffset(TranchesOutputAddress, 5 + Structure.GetTranche(0)->GetCashFlow().Count(), ++ClolumnCount), "_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-", (CallStructure.GetTranchesCount() + ClolumnCount) % 2 == 0 ? QColor(235, 241, 222) : QColor(216, 228, 188));
			}
			if (!CallStructure.GetExcessCashFlow().IsEmpty()) {
				TempValList.clear(); for (int i = 0; i<CallStructure.GetExcessCashFlow().Count(); i++) TempValList.append(CallStructure.GetExcessCashFlow().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + CallStructure.GetExcessCashFlow().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
				ExcelOutput::PrintColumn("Excess Spread", TempValList, ExcelCommons::CellOffset(TranchesOutputAddress, 5 + Structure.GetTranche(0)->GetCashFlow().Count(), ++ClolumnCount), "_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-", (CallStructure.GetTranchesCount() + ClolumnCount) % 2 == 0 ? QColor(235, 241, 222) : QColor(216, 228, 188));
			}
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(CallStructure.GetAnnualizedExcess(i));
			ExcelOutput::PrintColumn("Annualized Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,5+Structure.GetTranche(0)->GetCashFlow().Count(),++ClolumnCount),"0.00%",(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(CallStructure.GetWACostOfCapital(i));
			ExcelOutput::PrintColumn("WA Cost of Funding",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,5+Structure.GetTranche(0)->GetCashFlow().Count(),++ClolumnCount),"0.00%",(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranchesCount();i++) TempValList.append(CallStructure.GetTranche(i)->GetLossRate());
			ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(LossOnCallOutputAddress),"0.00%");
			ExcelOutput::PrintMergedCell("Scenario To Call",ExcelCommons::CellOffset(TranchesOutputAddress,3+Structure.GetTranche(0)->GetCashFlow().Count(),0),1,++ClolumnCount,QColor(118,147,60));
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
    LOGDEBUG("Finished PlotCPRLS");
	if(!PlotsSheet.isEmpty() && PlotIndexes[7]>0)
		ExcelOutput::PlotEquityReturn(Structure,PlotsSheet,PlotIndexes[7]);
	if(!PlotsSheet.isEmpty() && PlotIndexes[8]>0)
		ExcelOutput::PlotCallToEquity(Structure,PlotsSheet,PlotIndexes[8],RunCall ? CallStructure.GetCalledPeriod():QDate());
	QApplication::quit();
}
void CentralUnit::StressFinished(){
	if (Stresser)
		Stresser->SaveResults(FolderPath);
	QApplication::quit();
}
CentralUnit::~CentralUnit(){
	if(MtgsProgress) MtgsProgress->deleteLater();
	if (LastRateTable) delete LastRateTable;
}
void CentralUnit::CompileBaseRates(ConstantBaseRateTable& Values) {
	m_OverrideConstants = Values;
	Structure.CompileReferenceRateValue(Values);
	CallStructure.CompileReferenceRateValue(Values);
	LoansCalculator.CompileReferenceRateValue(Values);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = false;
	if (LastRateTable) delete LastRateTable;
	LastRateTable = new ConstantBaseRateTable(Values);
}
void CentralUnit::CompileBaseRates(ForwardBaseRateTable& Values) {
	m_OverrideForwards = Values;
	Structure.CompileReferenceRateValue(Values);
	CallStructure.CompileReferenceRateValue(Values);
	LoansCalculator.CompileReferenceRateValue(Values);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = true;
	if (LastRateTable) delete LastRateTable;
	LastRateTable = new ForwardBaseRateTable(Values);
}
#ifndef NO_DATABASE
void CentralUnit::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll)  {
	m_OverrideConstants = Values;
	Structure.GetBaseRatesDatabase(Values, DownloadAll);
	CallStructure.GetBaseRatesDatabase(Values, DownloadAll);
	LoansCalculator.GetBaseRatesDatabase(Values, DownloadAll);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = false;
	if (LastRateTable) delete LastRateTable;
	LastRateTable = new ConstantBaseRateTable(Values);
}
void CentralUnit::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll)  {
	m_OverrideForwards = Values;
	Structure.GetBaseRatesDatabase(Values, DownloadAll);
	CallStructure.GetBaseRatesDatabase(Values, DownloadAll);
	LoansCalculator.GetBaseRatesDatabase(Values, DownloadAll);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = true;
	if (LastRateTable) delete LastRateTable;
	LastRateTable = new ForwardBaseRateTable(Values);
}
#endif
