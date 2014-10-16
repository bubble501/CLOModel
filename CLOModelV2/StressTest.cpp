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
StressTest::StressTest(QObject* parent)
	: QObject(parent)
	, SequentialComputation(false)
	, ContinueCalculation(false)
	, ProgressForm(nullptr)
	, ShowProgress(true)
	, UseFastVersion(false)
	, m_ErrorsOccured(false)
	, m_CurrentProgressShift(0)
{
	BaseCalculator = new MtgCalculator(this);
	BaseCalculator->SetOverrideAssumptions(true);
	TranchesCalculator = new WaterfallCalculator(this);
	BaseApplier = new ScenarioApplier(this);
	ResetCurrentAssumption();
	connect(this, SIGNAL(CurrentScenarioCalculated()), this, SLOT(GoToNextScenario()),Qt::QueuedConnection);
	connect(BaseCalculator, SIGNAL(ErrorInCalculation()), this, SLOT(ErrorInCalculation()), Qt::QueuedConnection);
	connect(this, SIGNAL(AllLoansCalculated()), TranchesCalculator, SLOT(StartCalculation()), Qt::QueuedConnection);
	connect(this, SIGNAL(AllLoansCalculated()), this, SLOT(ReachedWaterfallCalc()));
	
	connect(TranchesCalculator, SIGNAL(Calculated()), this, SLOT(GatherResults()), Qt::QueuedConnection);
	connect(TranchesCalculator, SIGNAL(CurrentProgress(double)), this, SLOT(UpdateProgress(double)));
	connect(TranchesCalculator, SIGNAL(ErrorInCalculation(int)), this, SLOT(ErrorInCalculation()));
	connect(TranchesCalculator, SIGNAL(ErrorInCalculation(int)), this, SIGNAL(ErrorsOccured()));

	connect(BaseApplier, SIGNAL(Progress(double)), this, SLOT(UpdateProgress(double)));
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
	m_CurrentProgressShift = 0;
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
	BaseCalculator->AddLoan(a);
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
			, "Invalid Scenarios"
			, QString("%1 invalid scenarios detected\nWould you want to run the stress test anyway, ignoring those scenarios?").arg(CheckScen)
			, QMessageBox::Yes | QMessageBox::No
			, QMessageBox::Yes
		) != QMessageBox::Yes) return;
	}
	ResetCurrentAssumption();
	ContinueCalculation = true;
	m_ErrorsOccured=false;
	m_CurrentProgressShift = 0;
#ifdef PrintExecutionTime
	ExecutionTime.start();
#endif
	if (ShowProgress) {
		ProgressForm = new ProgressWidget;
		connect(ProgressForm, SIGNAL(Cancelled()), this, SLOT(StopCalculation()));
		ProgressForm->SetValue(0);
		ProgressForm->SetTitle("Stress Test");
		ProgressForm->SetMax(3*10000);
		ProgressForm->show();
	}
	if (UseFastVersion) {
		connect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(BaseForFastCalculated()));
		if (!BaseCalculator->StartCalculation(false)){ //TODO should be true
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
	BaseApplier->SetBaseFlows(BaseCalculator->GetResult());
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
		BaseApplier->AddAssumption(CurrentAss);
	}
	else {
		connect(BaseCalculator, SIGNAL(Calculated()), this, SLOT(SlowLoansCalculated()), Qt::UniqueConnection);
		BaseCalculator->SetCPRass(CurrentAss.GetCPRscenario());
		BaseCalculator->SetCDRass(CurrentAss.GetCDRscenario());
		BaseCalculator->SetLSass(CurrentAss.GetLSscenario());
		BaseCalculator->SetRecoveryLag(CurrentAss.GetRecLagScenario());
		BaseCalculator->SetDelinquency(CurrentAss.GetDelinqScenario());
		BaseCalculator->SetDelinquencyLag(CurrentAss.GetDelinqLagScenario());
		if (!BaseCalculator->StartCalculation(false)) {
			emit ErrorInCalculation();
		}
	}

}
void StressTest::SlowLoansCalculated() {
	if (!ContinueCalculation)  return StoppedCalculation();
	Structure.ResetMtgFlows();
	Structure.AddMortgagesFlows(BaseCalculator->GetResult());
	AssumptionSet CurrentAss(
		*(m_CPRscenarios.constBegin() + CurrentAssumption[AssCPR])
		, *(m_CDRscenarios.constBegin() + CurrentAssumption[AssCDR])
		, *(m_LSscenarios.constBegin() + CurrentAssumption[AssLS])
		, *(m_RecLagScenarios.constBegin() + CurrentAssumption[AssRecLag])
		, *(m_DelinqScenarios.constBegin() + CurrentAssumption[AssDelinq])
		, *(m_DelinqLagScenarios.constBegin() + CurrentAssumption[AssDelinqLag])
		);
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
		m_CurrentProgressShift = 10000;
		UpdateProgress(0.0);
		if (BaseApplier->NumBees() > 0) {
			BaseApplier->StartCalculation();
		}
		else 
			emit AllLoansCalculated();
		return;
		double CurrentProgress = static_cast<double>(CountScenariosCalculated()) / static_cast<double>(CountScenarios());
		UpdateProgress(CurrentProgress*10000.0);
		emit ProgressStatus(CurrentProgress);
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
	QDir curDir;
	QString DestinationFull = DestinationPath + QString(".StressResult.fcsr");
	QuaZip zip(DestinationFull);
	QBuffer OldData;
	quint32 OldCounter = 0;
	if (QFile::exists(DestinationFull)) {
		QStringList FileNames;
		qint32 VesionCheck;
		FileNames.append("StressTestInputs");
		for (auto i = Results.constBegin(); i != Results.constEnd(); ++i) FileNames.append(i.key().ToString() + ".csw");
		if (!zip.open(QuaZip::mdUnzip)) return;
		OldData.open(QBuffer::ReadWrite);
		QuaZipFile TargetFile(&zip);
		QuaZipFileInfo ZipFileInfo;
		for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
			TargetFile.open(QIODevice::ReadOnly);
			if (FileNames.contains(TargetFile.getActualFileName())) {
				TargetFile.close();
				continue;
			}
			QDataStream Reader(&TargetFile);
			QDataStream Writer(&OldData);
			Reader.setVersion(QDataStream::Qt_5_3);
			Writer.setVersion(QDataStream::Qt_5_3);
			Waterfall TempWF;
			Reader >> VesionCheck;
			if (VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)) continue;
			TempWF.SetLoadProtocolVersion(VesionCheck);
			Reader >> TempWF;
			Writer << TargetFile.getActualFileName();
			if (TargetFile.getFileInfo(&ZipFileInfo)) Writer << ZipFileInfo.dateTime;
			else Writer << QDateTime::currentDateTime();
			Writer << TempWF;
			OldCounter++;
			TargetFile.close();
		}
		OldData.seek(0);
		OldData.deleteLater();
		zip.close();
		if (!curDir.remove(DestinationFull)) return;
	}
	{
		if (!zip.open(QuaZip::mdCreate)) return;
		QuaZipFile TargetFile(&zip);
		QuaZipNewInfo ZipFileInfo("StressTestInputs");
		if (!TargetFile.open(QIODevice::WriteOnly, ZipFileInfo))return;
		{
			QDataStream out(&TargetFile);
			out.setVersion(QDataStream::Qt_5_3);
			out << qint32(ModelVersionNumber) << StartDate << SequentialComputation << ShowProgress << UseFastVersion << (*BaseCalculator) << Structure;
			TargetFile.close();
		}
		QDataStream Reader(&OldData);
		for (; OldCounter > 0;--OldCounter) {
			Waterfall TempWF;
			Reader >> ZipFileInfo.name >> ZipFileInfo.dateTime >> TempWF;
			if (TargetFile.open(QIODevice::WriteOnly, ZipFileInfo)) {
				QDataStream out(&TargetFile);
				out.setVersion(QDataStream::Qt_5_3);
				out << qint32(ModelVersionNumber) << TempWF;
				TargetFile.close();
			}
		}
		OldData.close();
		ZipFileInfo.dateTime = QDateTime::currentDateTime();
		for (auto i = Results.constBegin(); i != Results.constEnd(); ++i) {
			ZipFileInfo.name = i.key().ToString() + ".csw";
			if (TargetFile.open(QIODevice::WriteOnly, ZipFileInfo)) {
				QDataStream out(&TargetFile);
				out.setVersion(QDataStream::Qt_5_3);
				out << qint32(ModelVersionNumber) << *(i.value());
				TargetFile.close();
			}
		}
		zip.close();
	}
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
			return false;
		}
		for (int i = 0; i < NumStressDimentsions; ++i) {
			m_AssumptionsRef[i]->insert(Spanns.at(i));
		}
		QDataStream out(&TargetFile);
		out.setVersion(QDataStream::Qt_5_3);
		Waterfall* TempWF = new Waterfall();
		out >> VesionCheck;
		if (VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)) continue;
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
	quint32 Result=1;
	Result *= m_CDRscenarios.size();
	Result *= m_CPRscenarios.size();
	Result *= m_LSscenarios.size();
	Result *= m_RecLagScenarios.size();
	Result *= m_DelinqScenarios.size();
	Result *= m_DelinqLagScenarios.size();
	return Result;
}

void StressTest::StopCalculation() {
	ContinueCalculation = false;
	TranchesCalculator->Stop();
	if (ProgressForm) {
		ProgressForm->deleteLater();
	}
}

void StressTest::GatherResults() {
	Results.clear();
	const QHash<qint32, Waterfall*>& CalcRes = TranchesCalculator->GetWaterfalls();
	for (auto i = CalcRes.constBegin(); i != CalcRes.constEnd(); ++i) {
		auto MatchingAssumption = m_RainbowTable.constFind(i.key());
		if (MatchingAssumption != m_RainbowTable.constEnd()) {
			Results[MatchingAssumption.value()] = QSharedPointer<Waterfall>(
				new Waterfall(*(i.value()))
			);
		}
	}
	TranchesCalculator->ResetWaterfalls();
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


void StressTest::UpdateProgress(double pr) {
	if (ProgressForm) 
		ProgressForm->SetValue(m_CurrentProgressShift + static_cast<int>(pr*100.0));
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
		Structure.SetAssumptions(*CurrentAss);
		TranchesCalculator->AddWaterfall(Structure, CurrentHash);
	}
	emit AllLoansCalculated();
}







