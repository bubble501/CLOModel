#include "StressTest.h"
#include "Mortgage.h"
#include "CommonFunctions.h"
#include "Waterfall.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <quazip.h>
#include <quazipfile.h>
#include "BloombergVector.h"
#include "MtgCalculator.h"
#include <QMessageBox>
#include "MtgCashFlow.h"
#include <QDataStream>
#include "WaterfallCalculator.h"
#include "ScenarioApplier.h"
#include <QBuffer>
#include "PhasedProgressWidget.h"
StressTest::StressTest(QObject* parent)
	: QObject(parent)
	, SequentialComputation(false)
	, ContinueCalculation(false)
	, ProgressForm(nullptr)
	, ShowProgress(true)
	, UseFastVersion(false)
	, m_ErrorsOccured(false)
{
	Structure.SetIsStressTest(true);
	BaseCalculator = new MtgCalculator(this);
	BaseCalculator->SetOverrideAssumptions(true);
	TranchesCalculator = new WaterfallCalculator(this);
	BaseApplier = new ScenarioApplier(this);
	ResetCurrentAssumption();
	connect(this, SIGNAL(CurrentScenarioCalculated()), this, SLOT(GoToNextScenario()),Qt::QueuedConnection);
	connect(BaseCalculator, SIGNAL(BeeError(int)), this, SLOT(ErrorInCalculation()), Qt::QueuedConnection);
	connect(this, SIGNAL(AllLoansCalculated()), TranchesCalculator, SLOT(StartCalculation()), Qt::QueuedConnection);
	//connect(this, SIGNAL(AllLoansCalculated()), this, SLOT(ReachedWaterfallCalc()));
	
	connect(TranchesCalculator, SIGNAL(Calculated()), this, SLOT(GatherResults()), Qt::QueuedConnection);
	connect(TranchesCalculator, SIGNAL(BeeError(int)), this, SLOT(ErrorInCalculation()));
	connect(TranchesCalculator, SIGNAL(BeeError(int)), this, SIGNAL(ErrorsOccured()));

	connect(BaseApplier, SIGNAL(Calculated()), this, SLOT(FastLoansCalculated()), Qt::QueuedConnection);
	connect(BaseApplier, SIGNAL(BeeError(int)), this, SIGNAL(ErrorsOccured()));
	connect(BaseApplier, SIGNAL(BeeError(int)), this, SLOT(ErrorInCalculation()));

	m_AssumptionsRef[AssCPR] = &m_CPRscenarios;
	m_AssumptionsRef[AssCDR] = &m_CDRscenarios;
	m_AssumptionsRef[AssLS] = &m_LSscenarios;
	m_AssumptionsRef[AssRecLag] = &m_RecLagScenarios;
	m_AssumptionsRef[AssDelinq] = &m_DelinqScenarios;
	m_AssumptionsRef[AssDelinqLag] = &m_DelinqLagScenarios;
}
void StressTest::ResetStressTest() {
	ResetLoans();
	ResetResults();
	if (ProgressForm) {
		ProgressForm->deleteLater();
	}
}
const MtgCalculator& StressTest::GetLoans() const {
	return *BaseCalculator;
}

void StressTest::AddLoan(const Mortgage& a){
	BaseCalculator->AddLoan(a,BaseCalculator->NumBees());
}
void StressTest::ResetLoans(){
	BaseCalculator->Reset();
}
void StressTest::RunStressTest() {
	if (ContinueCalculation) return;
	m_RainbowTable.clear();
	BaseCalculator->SetSequentialComputation(SequentialComputation);
	TranchesCalculator->SetSequentialComputation(SequentialComputation);
	BaseApplier->SetSequentialComputation(SequentialComputation);
	BaseApplier->ClearResults();
	BaseApplier->ClearScenarios();
	BaseCalculator->SetStartDate(StartDate);
	BaseCalculator->SetCDRass("0");
	BaseCalculator->SetCPRass("0");
	BaseCalculator->SetLSass("0");
	BaseCalculator->SetRecoveryLag("0");
	BaseCalculator->SetDelinquency("0");
	BaseCalculator->SetDelinquencyLag("0");
	QString HardCheks = BaseCalculator->ReadyToCalculate();
	if (!HardCheks.isEmpty()) {
		QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + HardCheks);
		return;
	}
	{
		MtgCashFlow FakeFlow;
		FakeFlow.AddFlow(QDate(2015, 5, 30), 1000.0, 0);
		Structure.AddMortgagesFlows(FakeFlow);
	}
	HardCheks = Structure.ReadyToCalculate();
	if (!HardCheks.isEmpty()) {
		QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + HardCheks);
		return;
	}
	if (m_CDRscenarios.isEmpty()) m_CDRscenarios.insert("0");
	if (m_CPRscenarios.isEmpty()) m_CPRscenarios.insert("0");
	if (m_LSscenarios.isEmpty()) m_LSscenarios.insert("0");
	if (m_RecLagScenarios.isEmpty()) m_RecLagScenarios.insert("0");
	if (m_DelinqScenarios.isEmpty()) m_DelinqScenarios.insert("0");
	if (m_DelinqLagScenarios.isEmpty()) m_DelinqLagScenarios.insert("0");
	quint32 CheckScen = RemoveInvalidScenarios();
	if (CheckScen > 0) {
		if (QMessageBox::warning(
			0
			, tr("Invalid Scenarios")
			, tr("%1 invalid scenarios detected\nWould you want to run the stress test anyway, ignoring those scenarios?").arg(CheckScen)
			, QMessageBox::Yes | QMessageBox::No
			, QMessageBox::Yes
		) != QMessageBox::Yes) return;
	}
	ResetCurrentAssumption();
	ContinueCalculation = true;
	m_ErrorsOccured=false;
#ifdef PrintExecutionTime
	ExecutionTime.start();
#endif
	if (ShowProgress) {
		ProgressForm = new PhasedProgressWidget();
		ProgressForm->AddPhase(tr("Calculating full assets cash flows"),0,CountScenarios());
		ProgressForm->AddPhase(tr("Applying scenarios to aggregated cash flows"), 0, 0);
		ProgressForm->AddPhase(tr("Calculating liabilities cash flows"), 0, 100);
		ProgressForm->SetTotalProgressLabel(tr("Stress Test"));
		ProgressForm->show();
		connect(ProgressForm, SIGNAL(Cancelled()), this, SLOT(StopCalculation()));
		connect(this, SIGNAL(AllLoansCalculated()), ProgressForm, SLOT(NextPhase()), Qt::QueuedConnection);
		connect(TranchesCalculator, SIGNAL(Progress(double)), ProgressForm, SLOT(SetPhaseProgress(double)));
		connect(BaseApplier, SIGNAL(Progress(double)), ProgressForm, SLOT(SetPhaseProgress(double)));
	}
	if (UseFastVersion) {
		connect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(BaseForFastCalculated()));
		if (!BaseCalculator->StartCalculation()){
			QMessageBox::critical(0, "Invalid Input", "A base rate in the loans is invalid");
			return;
		}
	}
	else
		RunCurrentScenario();
}
void StressTest::BaseForFastCalculated() {
	disconnect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(BaseForFastCalculated()));
	if (!ContinueCalculation) return StoppedCalculation();
	BaseApplier->SetBaseFlows(BaseCalculator->GetAggregatedResults());
	RunCurrentScenario();
}
void StressTest::RunCurrentScenario() {
	if (!ContinueCalculation)  return StoppedCalculation();
	AssumptionSet CurrentAss(
		*(m_CPRscenarios.constBegin() + CurrentAssumption[AssCPR])
		, *(m_CDRscenarios.constBegin() + CurrentAssumption[AssCDR])
		, *(m_LSscenarios.constBegin() + CurrentAssumption[AssLS])
		, *(m_RecLagScenarios.constBegin() + CurrentAssumption[AssRecLag])
		, *(m_DelinqScenarios.constBegin() + CurrentAssumption[AssDelinq])
		, *(m_DelinqLagScenarios.constBegin() + CurrentAssumption[AssDelinqLag])
	);
	if (
		UseFastVersion 
		&& !IntegerVector(CurrentAss.GetRecLagScenario()).IsEmpty(0, 0)
		&& !IntegerVector(CurrentAss.GetDelinqLagScenario()).IsEmpty(0, 0)
		&& !BloombergVector(CurrentAss.GetDelinqScenario()).IsEmpty(0.0, 0.0)
	) {
		if (ProgressForm) {
			ProgressForm->SetPhaseMax(1, 100);
			ProgressForm->SetPhaseMax(0, ProgressForm->GetPhaseMax(0) -1);
		}
		BaseApplier->AddAssumption(CurrentAss,BaseApplier->NumBees());
		emit CurrentScenarioCalculated();
	}
	else {
		connect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(SlowLoansCalculated()), Qt::UniqueConnection);
		BaseCalculator->SetCPRass(CurrentAss.GetCPRscenario());
		BaseCalculator->SetCDRass(CurrentAss.GetCDRscenario());
		BaseCalculator->SetLSass(CurrentAss.GetLSscenario());
		BaseCalculator->SetRecoveryLag(CurrentAss.GetRecLagScenario());
		BaseCalculator->SetDelinquency(CurrentAss.GetDelinqScenario());
		BaseCalculator->SetDelinquencyLag(CurrentAss.GetDelinqLagScenario());
		if (!BaseCalculator->StartCalculation()) {
			emit ErrorInCalculation();
		}
	}

}
void StressTest::SlowLoansCalculated() {
	if (!ContinueCalculation)  return StoppedCalculation();
	Structure.ResetMtgFlows();
	Structure.AddMortgagesFlows(BaseCalculator->GetAggregatedResults());
	AssumptionSet CurrentAss(
		*(m_CPRscenarios.constBegin() + CurrentAssumption[AssCPR])
		, *(m_CDRscenarios.constBegin() + CurrentAssumption[AssCDR])
		, *(m_LSscenarios.constBegin() + CurrentAssumption[AssLS])
		, *(m_RecLagScenarios.constBegin() + CurrentAssumption[AssRecLag])
		, *(m_DelinqScenarios.constBegin() + CurrentAssumption[AssDelinq])
		, *(m_DelinqLagScenarios.constBegin() + CurrentAssumption[AssDelinqLag])
		);
	if (ProgressForm)
		ProgressForm->SetPhaseProgress(ProgressForm->GetPhaseProgress() + 1);
	if (m_RainbowTable.contains(qHash(CurrentAss, 88))) {
		emit ErrorInCalculation();
		return;
	}
	m_RainbowTable.insert(qHash(CurrentAss, 88), CurrentAss);
	Structure.SetAssumptions(CurrentAss);
	TranchesCalculator->AddWaterfall(Structure, qHash(CurrentAss, 88));
	emit CurrentScenarioCalculated();
}

void StressTest::GoToNextScenario() {
	if (!IncreaseCurrentAssumption()) {
#ifdef PrintExecutionTime
		QTime TempTime;
		TempTime.addMSecs(ExecutionTime.elapsed());
		QMessageBox::information(0, "Computation Time", QString("Stress Test Took: %1 Seconds").arg(ExecutionTime.elapsed() / 1000));
#endif
		disconnect(BaseCalculator);
		if (ProgressForm) ProgressForm->NextPhase();
		if (BaseApplier->NumBees() > 0) {
			BaseApplier->StartCalculation();
		}
		else {
			emit AllLoansCalculated();
		}
		return;
	}
	RunCurrentScenario();
}

bool StressTest::IncreaseCurrentAssumption(int level) {
	if (level >= NumStressDimentsions || level < 0) return false;
	if (CurrentAssumption[level] == m_AssumptionsRef[level]->size() - 1) {
		CurrentAssumption[level] = 0;
		return IncreaseCurrentAssumption(level - 1);
	}
	CurrentAssumption[level]++;
	return true;
}

qint32 StressTest::CountScenariosCalculated(int level) {
	if (level >= NumStressDimentsions || level < 0) return 0;
	qint32 Result = CurrentAssumption[level];
	for (int i = level + 1; i < NumStressDimentsions && Result>0; ++i) Result *= m_AssumptionsRef[i]->size();
	return Result + CountScenariosCalculated(level - 1);
}



QDataStream& operator<<(QDataStream & stream, const StressTest& flows){
	//Deprecated, use SaveResults(), it's much faster
	stream << flows.Structure;
	stream << flows.StartDate;
	stream << flows.SequentialComputation;
	stream << flows.ShowProgress;
	stream << flows.UseFastVersion;
	for (int i = 0; i < NumStressDimentsions; ++i) {
		stream << *(flows.m_AssumptionsRef[i]);
	}
	stream << static_cast<qint32>(flows.Results.size());
	for (auto i = flows.Results.constBegin(); i != flows.Results.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	return stream;
}

QDataStream& StressTest::LoadOldVersion(QDataStream& stream) {
	qint32 tempint;
	Structure.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> Structure;
	stream >> StartDate;
	stream >> SequentialComputation;
	stream >> ShowProgress;
	stream >> UseFastVersion;
	for (int i = 0; i < NumStressDimentsions; ++i) {
		stream >> *(m_AssumptionsRef[i]);
	}
	ResetResults();
	stream >> tempint;
	AssumptionSet TempAssSet;
	Waterfall tempWatf;
	for (qint32 i = 0; i < tempint; i++) {
		TempAssSet.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> TempAssSet;
		tempWatf.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> tempWatf;
		Results.insert(TempAssSet, QSharedPointer<Waterfall>(new Waterfall(tempWatf)));
	}
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator>>(QDataStream & stream, StressTest& flows) {
	//Deprecated, use LoadResultsFromFile(), it's much faster
	return flows.LoadOldVersion(stream);
}

void StressTest::SaveResults(const QString& DestPath)const{
	QString DestinationPath(DestPath.trimmed());
	if (*(DestinationPath.end() - 1) != '\\' && *(DestinationPath.end() - 1) != '/') DestinationPath.append('/');
	QString OldName = DestinationPath + QString("StressResult.fcsr");
	QString NewName;
	if (QFile::exists(OldName)) {
		NewName = DestinationPath + QString("StressResult%1.fcsr");
		for (quint64 i = 0; true; ++i) {
			if (!QFile::exists(NewName.arg(i))) {
				NewName.arg(i);
				break;
			}
		}
	}
	else {
		NewName = OldName;
		OldName = QString();
	}


	QDir curDir;
	QuaZip zip(NewName);
	QStringList UsedFileNames;
	if (!zip.open(QuaZip::mdCreate)) return;
	QuaZipFile TargetFile(&zip);
	QuaZipNewInfo ZipFileInfo("StressTestInputs");
	UsedFileNames << "StressTestInputs";
	if (!TargetFile.open(QIODevice::WriteOnly, ZipFileInfo))return;
	{
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_5_3);
		out << qint32(ModelVersionNumber) << StartDate << SequentialComputation << ShowProgress << UseFastVersion << (*BaseCalculator) << Structure;
		TargetFile.close();
	}
	ZipFileInfo.dateTime = QDateTime::currentDateTime();
	for (auto i = Results.constBegin(); i != Results.constEnd(); ++i) {
		ZipFileInfo.name = i.key().ToString() + ".csw";
		UsedFileNames << ZipFileInfo.name;
		if (TargetFile.open(QIODevice::WriteOnly, ZipFileInfo)) {
			QDataStream out(&TargetFile);
			out.setVersion(QDataStream::Qt_5_3);
			out << qint32(ModelVersionNumber) << *(i.value());
			TargetFile.close();
		}
	}

	if (!OldName.isEmpty()) {
		QuaZip OldZip(OldName);
		QuaZipFile OldFile(&OldZip);
		QuaZipFileInfo OldFileInfo;
		if (!OldZip.open(QuaZip::mdUnzip)) return;
		for (bool more = OldZip.goToFirstFile(); more; more = OldZip.goToNextFile()) {
			if (OldFile.open(QIODevice::ReadOnly)) {
				if (UsedFileNames.contains(OldFile.getActualFileName())) {
					OldFile.close();
					continue;
				}
				QDataStream Reader(&OldFile);
				Reader.setVersion(QDataStream::Qt_5_3);
				
				qint32 VesionCheck;
				Waterfall TempWF;
				if (!OldFile.getFileInfo(&OldFileInfo)) {
					OldFileInfo.dateTime = QDateTime::currentDateTime();
				}
				Reader >> VesionCheck;
				if (VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)) {
					OldFile.close();
					continue;
				}
				TempWF.SetLoadProtocolVersion(VesionCheck);
				Reader >> TempWF;

				ZipFileInfo.name = OldFile.getActualFileName();
				ZipFileInfo.dateTime = OldFileInfo.dateTime;
				UsedFileNames << ZipFileInfo.name;
				OldFile.close();
				if (TargetFile.open(QIODevice::WriteOnly, ZipFileInfo)) {
					QDataStream Writer(&TargetFile);
					Writer.setVersion(QDataStream::Qt_5_3);
					Writer << qint32(ModelVersionNumber) << TempWF;
					TargetFile.close();
				}
			}

		}
		OldZip.close();
		if(!curDir.remove(OldName))return;
		QFile::rename(NewName, OldName);
	}
	zip.close();
}

Waterfall StressTest::GetScenarioFromFile(const QString& DestPath, const QString& CPRscenario, const QString& CDRscenario, const QString& LSscenario, const QString& RecLagScenario, const QString& DelinqScenario, const QString& DelinqLagScenario) {
	AssumptionSet CurrentKey(CPRscenario, CDRscenario, LSscenario, RecLagScenario, DelinqScenario, DelinqLagScenario);
	Waterfall Result;
	qint32 VesionCheck;
	if(!QFile::exists(DestPath)) return Result;
	QuaZip zip(DestPath);
	if(!zip.open(QuaZip::mdUnzip)) return Result;
	QuaZipFile TargetFile(&zip);
	if (!zip.setCurrentFile(CurrentKey.ToString()+ ".csw")) return Result;
	TargetFile.open(QIODevice::ReadOnly);
	QDataStream out(&TargetFile);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VesionCheck;
	if (VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)) return Result;
	Result.SetLoadProtocolVersion(VesionCheck);
	out >> Result;
	TargetFile.close();
	return Result;
}
bool StressTest::LoadResultsFromFile(const QString& DestPath){
	ResetStressTest();
	qint32 VesionCheck;
	if(!QFile::exists(DestPath)) return false;
	QuaZip zip(DestPath);
	if(!zip.open(QuaZip::mdUnzip)) return false;
	QuaZipFile TargetFile(&zip);
	try{
		if(!zip.setCurrentFile("StressTestInputs")) throw 1;
		TargetFile.open(QIODevice::ReadOnly);
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_5_3);
		out >> VesionCheck;
		if(VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)){
			TargetFile.close();
			throw 1;
		}
		out >> StartDate >> SequentialComputation >> ShowProgress >> UseFastVersion;
		BaseCalculator->SetLoadProtocolVersion(VesionCheck); out >> (*BaseCalculator);
		Structure.SetLoadProtocolVersion(VesionCheck); out >> Structure;
		TargetFile.close();
	}
	catch(int ExcCode){
		if(!QApplication::instance()){
			char *argv[] = {"NoArgumnets"};
			int argc = sizeof(argv) / sizeof(char*) - 1;
			QApplication ComputationLoop(argc,argv);
			QMessageBox::critical(0,"Incompatible Version","The stress test data is not compatible with the current model version\nPlease run ALL the stress tests again");
			QApplication::quit();
		}
		else {
			QMessageBox::critical(0,"Incompatible Version","The stress test data is not compatible with the current model version\nPlease run ALL the stress tests again");
		}
		return false;
	}
	for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
		TargetFile.open(QIODevice::ReadOnly);
		if(TargetFile.getActualFileName()=="StressTestInputs"){
			TargetFile.close();
			continue;
		}
		AssumptionSet CurrentAss = TargetFile.getActualFileName().left(TargetFile.getActualFileName().lastIndexOf("."));
		QStringList Spanns = CurrentAss.ToString().split("#,#");
		if (Spanns.size() != NumStressDimentsions) {
			TargetFile.close();
			continue;
		}
		for (int i = 0; i < NumStressDimentsions; ++i) {
			m_AssumptionsRef[i]->insert(Spanns.at(i));
		}
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_5_3);
		Waterfall* TempWF = new Waterfall();
		out >> VesionCheck;
		if (VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)) {
			TargetFile.close();
			continue;
		}
		TempWF->SetLoadProtocolVersion(VesionCheck);
		out >> (*TempWF);
		Results.insert(CurrentAss, QSharedPointer<Waterfall>(TempWF));
		TargetFile.close();
	}
	return true;
}

quint32 StressTest::RemoveInvalidScenarios() {
	quint32 InvalidScen = 0;
	for (auto i = m_CDRscenarios.begin(); i != m_CDRscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)){
			i = m_CDRscenarios.erase(i); ++InvalidScen;
			if (i == m_CDRscenarios.end()) break;
		}
	}
	for (auto i = m_CPRscenarios.begin(); i != m_CPRscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
			i = m_CPRscenarios.erase(i); ++InvalidScen;
			if (i == m_CPRscenarios.end()) break;
		}
	}
	for (auto i = m_LSscenarios.begin(); i != m_LSscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
			i = m_LSscenarios.erase(i); ++InvalidScen;
			if (i == m_LSscenarios.end()) break;
		}
	}
	for (auto i = m_DelinqScenarios.begin(); i != m_DelinqScenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
			i = m_DelinqScenarios.erase(i); ++InvalidScen;
			if (i == m_DelinqScenarios.end()) break;
		}
	}
	for (auto i = m_RecLagScenarios.begin(); i != m_RecLagScenarios.end(); ++i) {
		while (IntegerVector(*i).IsEmpty(0)) {
			i = m_RecLagScenarios.erase(i); ++InvalidScen;
			if (i == m_RecLagScenarios.end()) break;
		}
	}
	for (auto i = m_DelinqLagScenarios.begin(); i != m_DelinqLagScenarios.end(); ++i) {
		while (IntegerVector(*i).IsEmpty(0)) {
			i = m_DelinqLagScenarios.erase(i); ++InvalidScen;
			if (i == m_DelinqLagScenarios.end()) break;
		}
	}
	return InvalidScen;
}

quint32 StressTest::CountScenarios() const {
	return
		m_CDRscenarios.size()
		* m_CPRscenarios.size()
		* m_LSscenarios.size()
		* m_RecLagScenarios.size()
		* m_DelinqScenarios.size()
		* m_DelinqLagScenarios.size();
}

void StressTest::StopCalculation() {
	ContinueCalculation = false;
	TranchesCalculator->StopCalculation();
	if (ProgressForm) {
		ProgressForm->deleteLater();
	}
}

void StressTest::GatherResults() {
	Results.clear();
	TranchesCalculator->ClearWaterfalls();
	const QHash<qint32, Waterfall*>& CalcRes = TranchesCalculator->GetResults();
	for (auto i = CalcRes.constBegin(); i != CalcRes.constEnd(); ++i) {
		auto MatchingAssumption = m_RainbowTable.constFind(i.key());
		if (MatchingAssumption != m_RainbowTable.constEnd()) {
			Results[MatchingAssumption.value()] = QSharedPointer<Waterfall>(
				new Waterfall(*(i.value()))
			);
		}
	}
	TranchesCalculator->ClearResults();
	if (ProgressForm) {
		ProgressForm->deleteLater();
	}
	if (m_ErrorsOccured)
		emit FinishedWithErrors();
	else
		emit AllFinished();
}

void StressTest::ResetScenarios() {
	for (int i = 0; i < NumStressDimentsions; i++) m_AssumptionsRef[i]->clear();
}



void StressTest::FastLoansCalculated() {
	uint CurrentHash;
	QList<qint32> AllKeys = BaseApplier->GetAssumptionKeys();
	const AssumptionSet* CurrentAss;
	foreach(qint32 SingleKey, AllKeys) {
		CurrentAss = BaseApplier->GetAssumption(SingleKey);
		if (!CurrentAss)continue;
		uint CurrentHash = qHash(*CurrentAss, 88);
		if (m_RainbowTable.contains(CurrentHash)) {
			emit ErrorInCalculation();
			return;
		}
		m_RainbowTable.insert(CurrentHash, *CurrentAss);
		Structure.ResetMtgFlows();
		Structure.AddMortgagesFlows(*(BaseApplier->GetResult(SingleKey)));
		Structure.SetAssumptions(*CurrentAss);
		TranchesCalculator->AddWaterfall(Structure, CurrentHash);
	}
	emit AllLoansCalculated();
}

#ifndef NO_DATABASE
void StressTest::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) {
	Structure.GetBaseRatesDatabase(Values, DownloadAll);
	BaseCalculator->GetBaseRatesDatabase(Values, DownloadAll);
}
void StressTest::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) {
	Structure.GetBaseRatesDatabase(Values, DownloadAll);
	BaseCalculator->GetBaseRatesDatabase(Values, DownloadAll);
}
#endif
void StressTest::CompileBaseRates(ConstantBaseRateTable& Values) {
	Structure.CompileReferenceRateValue(Values);
	BaseCalculator->CompileReferenceRateValue(Values);
}
void StressTest::CompileBaseRates(ForwardBaseRateTable& Values) {
	Structure.CompileReferenceRateValue(Values);
	BaseCalculator->CompileReferenceRateValue(Values);
}




