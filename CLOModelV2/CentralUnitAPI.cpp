#include "CentralUnitAPI.h"
#include "ProgressWidget.h"
#include <QMessageBox>
#include <QApplication>
#include "WaterfallCalculator.h"
#include <QFile>
#include <QDir>
#include <QSettings>
#ifdef Q_WS_WIN
#include <Windows.h>
#endif
CentralUnitAPI::CentralUnitAPI(QObject* parent)
	:QObject(parent)
	,MtgsProgress(NULL)
	,RunCall(false)
	,ShowProgress(true)
	,SequentialComputation(false)
	, m_UseForwardCurve(false)
{
	if(!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");
	if(!QMetaType::isRegistered(qMetaTypeId<MtgCashFlow>()))
		qRegisterMetaType<MtgCashFlow>("MtgCashFlow");
	connect(&LoansCalculator,SIGNAL(Calculated()),this,SLOT(CalculationStep2()));
	connect(&LoansCalculator,SIGNAL(BeeCalculated(int)),this,SLOT(HandleProgress(int)));
	connect(this,SIGNAL(LoopStarted()),this,SLOT(CalculationStep1()),Qt::QueuedConnection);
	ParallWatFalls=new WaterfallCalculator(this);
	connect(ParallWatFalls,SIGNAL(Calculated()),this,SLOT(CheckCalculationDone()));
}
void CentralUnitAPI::SetSequentialComputation(bool a){
	LoansCalculator.SetSequentialComputation(a);
	ParallWatFalls->SetSequentialComputation(a);
	SequentialComputation=a;
}
const Mortgage* CentralUnitAPI::GetLoan(int index)const{
	if(index<0 || index>=LoansCount()) return NULL;
	return LoansCalculator.GetLoans().at(index);
}
Mortgage* CentralUnitAPI::GetLoan(int index){
	if(index<0 || index>=LoansCount()) return NULL;
	return LoansCalculator.GetLoans()[index];
}
void CentralUnitAPI::Calculate(){
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	emit LoopStarted();
	ComputationLoop.exec();
}
void CentralUnitAPI::CalculationStep1(){
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
		return;
	}
	if(MtgsProgress) MtgsProgress->deleteLater();
	if(ShowProgress){
		MtgsProgress=new ProgressWidget;
		MtgsProgress->SetValue(0);
		MtgsProgress->SetTitle("Calculating Loans");
		MtgsProgress->SetMax(LoansCalculator.Count());
		connect(&LoansCalculator,SIGNAL(BeeCalculated(int)),MtgsProgress,SLOT(SetValue(int)));
		MtgsProgress->show();
		QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	}
	LoansCalculator.StartCalculation();
}
void CentralUnitAPI::CalculationStep2(){
	if(ShowProgress){
		MtgsProgress->SetValue(0);
		MtgsProgress->SetTitle("Calculating Tranches");
		MtgsProgress->SetMax(1);
	}
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	Structure.ResetMtgFlows();
	Structure.AddMortgagesFlows(LoansCalculator.GetResult());
	Structure.SetUseCall(false);
	QString TmpStr=Structure.ReadyToCalculate();
	if(!TmpStr.isEmpty()){
		QMessageBox::critical(0,"Invalid Input","The following Inputs are missing or invalid:\n"+TmpStr);
		return;
	}
	if(!RunCall){
		CallStructure.ResetMtgFlows();
		CallStructure.ResetTranches();
		CallStructure.ResetSteps();
		Structure.CalculateTranchesCashFlows();
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
			return;
		}
		ParallWatFalls->AddWaterfall(CallStructure);
		ParallWatFalls->StartCalculation();
	}
}
void CentralUnitAPI::CheckCalculationDone()
{
	if(ShowProgress){
		MtgsProgress->SetValue(1);
		QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	}
	if(MtgsProgress) MtgsProgress->deleteLater();
	MtgsProgress=NULL;
	Tranche TempTranche;
	if(RunCall){
		Structure=*(ParallWatFalls->GetWaterfalls().at(0));
		CallStructure=*(ParallWatFalls->GetWaterfalls().at(1));
	}
	emit CalculationFinished();
	QApplication::quit();
}

void CentralUnitAPI::SaveBaseCase(const QString& FolderPath,const QString& BaseCaseFileName){
	QString AddedChar;
	if(FolderPath.endsWith('/') || FolderPath.endsWith('\\')) AddedChar="";
	else AddedChar="/";
	QDir TempDir=QDir::homePath();
	TempDir.mkpath(FolderPath+ AddedChar);
	QString AddedExt("");
	if(!BaseCaseFileName.trimmed().endsWith(".clp")) AddedExt=".clo";
	QString Filename=FolderPath+ AddedChar +BaseCaseFileName.trimmed()+AddedExt;
	QFile file(Filename);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_5_3);
		out << qint32(ModelVersionNumber) << Structure << CallStructure;
		file.close();
	}
}
bool CentralUnitAPI::LoadBaseCase(const QString& DestPath){
	if(!QFile::exists(DestPath)) return false;
	qint32 VersionCheck;
	QFile file(DestPath);
	if (!file.open(QIODevice::ReadOnly)) return false;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionCheck;
	if(VersionCheck<MinimumSupportedVersion) return false;
	Structure.SetLoadProtocolVersion(VersionCheck);
	out >> Structure;
	CallStructure.SetLoadProtocolVersion(VersionCheck);
	out >> CallStructure;
	file.close();
	return true;
}
void CentralUnitAPI::SaveLoanPool(const QString& FolderPath,const QString& LoanPoolFileName){
	QString AddedChar;
	if(FolderPath.endsWith('/') || FolderPath.endsWith('\\')) AddedChar="";
	else AddedChar="/";
	QDir TempDir=QDir::homePath();
	TempDir.mkpath(FolderPath+ AddedChar);
	QString AddedExt("");
	if(!LoanPoolFileName.trimmed().endsWith(".clp")) AddedExt=".clp";
	QString Filename=FolderPath+ AddedChar +LoanPoolFileName.trimmed()+AddedExt;
	QFile file(Filename);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_5_3);
		out << qint32(ModelVersionNumber) << LoansCalculator;
		file.close();
	}
}
bool CentralUnitAPI::LoadLoanPool(const QString& DestPath){
	if(!QFile::exists(DestPath)) return false;
	qint32 VersionCheck;
	QFile file(DestPath.trimmed() );
	if (!file.open(QIODevice::ReadOnly)) return false;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionCheck;
	if(VersionCheck<MinimumSupportedVersion) return false;
	out >> LoansCalculator;
	file.close();
	return true;
}
void CentralUnitAPI::SaveModel(const QString& ModelFileName) {
	QSettings ConfigIni(":/Configs/GlobalConfigs.ini", QSettings::IniFormat);
	ConfigIni.beginGroup("Folders");
	SaveModel(ConfigIni.value("UnifiedResultsFolder", "\\\\synserver2\\Company Share\\24AM\\Monitoring\\Model Results").toString(), ModelFileName);
}
void CentralUnitAPI::SaveModel(const QString& FolderPath, const QString& ModelFileName) {
	QDir TempDir(FolderPath);
	QString Filename = TempDir.absoluteFilePath(ModelFileName);
	QFile file(Filename);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_5_3);
		out << qint32(ModelVersionNumber) << LiborUpdateDate << m_UseForwardCurve << Structure << CallStructure << LoansCalculator;
		file.close();
	}
}
bool CentralUnitAPI::LoadModel(const QString& DestPath) {
	if (!QFile::exists(DestPath)) return false;
	qint32 VersionCheck;
	QFile file(DestPath);
	if (!file.open(QIODevice::ReadOnly)) return false;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionCheck;
	if (VersionCheck < MinimumSupportedVersion) return false;
	out >> LiborUpdateDate >> m_UseForwardCurve;
	Structure.SetLoadProtocolVersion(VersionCheck);
	out >> Structure;
	CallStructure.SetLoadProtocolVersion(VersionCheck);
	out >> CallStructure;
	out >> LoansCalculator;
	file.close();
	return true;
}
CentralUnitAPI::~CentralUnitAPI(){
	if(MtgsProgress) MtgsProgress->deleteLater();
}
void CentralUnitAPI::HandleProgress(int a){
	emit ProgressUpdate(
		1000*static_cast<double>(a)/static_cast<double>(LoansCalculator.Count())
	);
}
void CentralUnitAPI::CompileBaseRates(ForwardBaseRateTable& Values)const {
	for(int i=0;i<Structure.GetTranchesCount();i++){
		Structure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
	for(int i=0;i<CallStructure.GetTranchesCount();i++){
		CallStructure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
	LoansCalculator.CompileReferenceRateValue(Values);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = true;
}
void CentralUnitAPI::CompileBaseRates(ConstantBaseRateTable& Values)const {
	for (int i = 0; i < Structure.GetTranchesCount(); i++) {
		Structure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
	for (int i = 0; i < CallStructure.GetTranchesCount(); i++) {
		CallStructure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
	LoansCalculator.CompileReferenceRateValue(Values);
	LiborUpdateDate = Values.GetUpdateDate();
	m_UseForwardCurve = false;
}
#ifndef NO_DATABASE
void CentralUnitAPI::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll)const {
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
void CentralUnitAPI::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll)const {
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