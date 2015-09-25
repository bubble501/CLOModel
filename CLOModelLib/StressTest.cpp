#include "StressTest.h"
#include "Private/StressTest_p.h"
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
#include <QProgressDialog>
#include "Private/PhasedProgressWidget.h"
#include <QIcon>
DEFINE_PUBLIC_QOBJECT_COMMONS(StressTest)
StressTestPrivate::StressTestPrivate(StressTest *q)
    :BackwardInterfacePrivate(q)
{}
StressTest::StressTest(StressTestPrivate *d, QObject* parent)
    : BackwardInterface(d)
    , QObject(parent)
{
    d->SequentialComputation = false;
    d->ContinueCalculation = false;
    d->ProgressForm = nullptr;
    d->ShowProgress = true;
    d->UseFastVersion = false;
    d->m_ErrorsOccured = false;
    d->Structure.SetIsStressTest(true);
    d->BaseCalculator = new MtgCalculator(this);
    d->BaseCalculator->SetOverrideAssumptions(true);
    d->TranchesCalculator = new WaterfallCalculator(this);
    d->BaseApplier = new ScenarioApplier(this);
    ResetCurrentAssumption();
    connect(this, SIGNAL(CurrentScenarioCalculated()), this, SLOT(GoToNextScenario()), Qt::QueuedConnection);
    connect(d->BaseCalculator, SIGNAL(BeeError(int)), this, SLOT(ErrorInCalculation()), Qt::QueuedConnection);
    connect(this, SIGNAL(AllLoansCalculated()), d->TranchesCalculator, SLOT(StartCalculation()), Qt::QueuedConnection);
    //connect(this, &StressTest::AllLoansCalculated, [&]() { BaseApplier->ClearResults(); BaseApplier->ClearScenarios(); BaseCalculator->ClearLoans(); BaseCalculator->ClearResults(); });
    //connect(this, SIGNAL(AllLoansCalculated()), this, SLOT(ReachedWaterfallCalc()));

    connect(d->TranchesCalculator, SIGNAL(Calculated()), this, SLOT(GatherResults()), Qt::QueuedConnection);
    connect(d->TranchesCalculator, SIGNAL(BeeError(int)), this, SLOT(ErrorInCalculation()));
    connect(d->TranchesCalculator, SIGNAL(BeeError(int)), this, SIGNAL(ErrorsOccured()));

    connect(d->BaseApplier, SIGNAL(Calculated()), this, SLOT(FastLoansCalculated()), Qt::QueuedConnection);
    connect(d->BaseApplier, SIGNAL(BeeError(int)), this, SIGNAL(ErrorsOccured()));
    connect(d->BaseApplier, SIGNAL(BeeError(int)), this, SLOT(ErrorInCalculation()));

    d->m_AssumptionsRef[StressTestPrivate::AssCPR] = &d->m_CPRscenarios;
    d->m_AssumptionsRef[StressTestPrivate::AssCDR] = &d->m_CDRscenarios;
    d->m_AssumptionsRef[StressTestPrivate::AssLS] = &d->m_LSscenarios;
    d->m_AssumptionsRef[StressTestPrivate::AssRecLag] = &d->m_RecLagScenarios;
    d->m_AssumptionsRef[StressTestPrivate::AssDelinq] = &d->m_DelinqScenarios;
    d->m_AssumptionsRef[StressTestPrivate::AssDelinqLag] = &d->m_DelinqLagScenarios;
}

void StressTest::SetShowProgress(bool a/*=true*/)
{
    Q_D(StressTest);
    d->ShowProgress = a;
}

bool StressTest::GetShowProgress() const
{
    Q_D(const StressTest);
    return d->ShowProgress;
}

const QSet<QString>& StressTest::GetCDRscenarios() const
{
    Q_D(const StressTest);
    return d->m_CDRscenarios;
}

const QSet<QString>& StressTest::GetCPRscenarios() const
{
    Q_D(const StressTest);
    return d->m_CPRscenarios;
}

const QSet<QString>& StressTest::GetLSscenarios() const
{
    Q_D(const StressTest);
    return d->m_LSscenarios;
}

const QSet<QString>& StressTest::GetRecLagScenarios() const
{
    Q_D(const StressTest);
    return d->m_RecLagScenarios;
}

const QSet<QString>& StressTest::GetDelinqScenarios() const
{
    Q_D(const StressTest);
    return d->m_DelinqScenarios;
}

const QSet<QString>& StressTest::GetDelinqLagScenarios() const
{
    Q_D(const StressTest);
    return d->m_DelinqLagScenarios;
}

void StressTest::SetCDRscenarios(const QSet<QString>& val)
{
    Q_D( StressTest);
    d->m_CDRscenarios = val;
}

void StressTest::AddCDRscenarios(const QSet<QString>& val)
{
    Q_D( StressTest);
    d->m_CDRscenarios.unite(val);
}

void StressTest::AddCDRscenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_CDRscenarios.insert(val);
}

void StressTest::RemoveCDRscenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_CDRscenarios.remove(val);
}

void StressTest::ClearCDRscenarios()
{
    Q_D(StressTest);
    d->m_CDRscenarios.clear();
}

void StressTest::SetCPRscenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_CPRscenarios = val;
}

void StressTest::AddCPRscenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_CPRscenarios.unite(val);
}

void StressTest::AddCPRscenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_CPRscenarios.insert(val);
}

void StressTest::RemoveCPRscenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_CPRscenarios.remove(val);
}

void StressTest::ClearCPRscenarios()
{
    Q_D(StressTest);
    d->m_CPRscenarios.clear();
}

void StressTest::SetLSscenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_LSscenarios = val;
}

void StressTest::AddLSscenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_LSscenarios.unite(val);
}

void StressTest::AddLSscenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_LSscenarios.insert(val);
}

void StressTest::RemoveLSscenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_LSscenarios.remove(val);
}

void StressTest::ClearLSscenarios()
{
    Q_D(StressTest);
    d->m_LSscenarios.clear();
}

void StressTest::SetRecLagScenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_RecLagScenarios = val;
}

void StressTest::AddRecLagScenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_RecLagScenarios.unite(val);
}

void StressTest::AddRecLagScenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_RecLagScenarios.insert(val);
}

void StressTest::RemoveRecLagScenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_RecLagScenarios.remove(val);
}

void StressTest::ClearRecLagScenarios()
{
    Q_D(StressTest);
    d->m_RecLagScenarios.clear();
}

void StressTest::SetDelinqScenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_DelinqScenarios = val;
}

void StressTest::AddDelinqScenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_DelinqScenarios.unite(val);
}

void StressTest::AddDelinqScenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_DelinqScenarios.insert(val);
}

void StressTest::RemoveDelinqScenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_DelinqScenarios.remove(val);
}

void StressTest::ClearDelinqScenarios()
{
    Q_D(StressTest);
    d->m_DelinqScenarios.clear();
}

void StressTest::SetDelinqLagScenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_DelinqLagScenarios = val;
}

void StressTest::AddDelinqLagScenarios(const QSet<QString>& val)
{
    Q_D(StressTest);
    d->m_DelinqLagScenarios.unite(val);
}

void StressTest::AddDelinqLagScenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_DelinqLagScenarios.insert(val);
}

void StressTest::RemoveDelinqLagScenarios(const QString& val)
{
    Q_D(StressTest);
    d->m_DelinqLagScenarios.remove(val);
}

void StressTest::ClearDelinqLagScenarios()
{
    Q_D(StressTest);
    d->m_DelinqLagScenarios.clear();
}

void StressTest::ClearAssumptions()
{
    ClearCDRscenarios(); 
    ClearCPRscenarios(); 
    ClearLSscenarios(); 
    ClearRecLagScenarios(); 
    ClearDelinqScenarios(); 
    ClearDelinqLagScenarios();
}

const Waterfall& StressTest::GetStructure() const
{
    Q_D(const StressTest);
    return d->Structure;
}

const QDate& StressTest::GetStartDate() const
{
    Q_D(const StressTest);
    return d->StartDate;
}

const QHash<AssumptionSet, QSharedPointer<Waterfall> > & StressTest::GetResults() const
{
    Q_D(const StressTest);
    return d->Results;
}

bool StressTest::GetUseMultithread() const
{
    Q_D(const StressTest);
    return !d->SequentialComputation;
}

void StressTest::ResetStressTest()
{
    Q_D( StressTest);
	ResetLoans();
	ResetResults();
    if (d->ProgressForm) {
        d->ProgressForm->deleteLater();
	}
}
const MtgCalculator& StressTest::GetLoans() const
{
    Q_D(const StressTest);
    return *d->BaseCalculator;
}

void StressTest::AddLoan(const Mortgage& a)
{
    Q_D( StressTest);
    d->BaseCalculator->AddLoan(a, d->BaseCalculator->NumBees());
}
void StressTest::ResetLoans()
{
    Q_D( StressTest);
    d->BaseCalculator->Reset();
}

void StressTest::SetStructure(const Waterfall& a)
{
    Q_D( StressTest);
    d->Structure = a; 
    d->Structure.SetIsStressTest(true);
}

void StressTest::SetStartDate(const QDate& a)
{
    Q_D( StressTest);
    d->StartDate = a;
}

void StressTest::UseMultithread(bool a/*=true*/)
{
    Q_D( StressTest);
    d->SequentialComputation = !a;
}

void StressTest::RunStressTest()
{
    Q_D( StressTest);
    if (d->ContinueCalculation) return;
    d->m_RainbowTable.clear();
    d->BaseCalculator->SetSequentialComputation(d->SequentialComputation);
    d->TranchesCalculator->SetSequentialComputation(d->SequentialComputation);
    d->BaseApplier->SetSequentialComputation(d->SequentialComputation);
    d->BaseApplier->ClearResults();
    d->BaseApplier->ClearScenarios();
    d->BaseCalculator->DownloadScenarios();
    d->BaseCalculator->SetStartDate(d->StartDate);
    d->BaseCalculator->SetCDRass("0");
    d->BaseCalculator->SetCPRass("0");
    d->BaseCalculator->SetLSass("0");
    d->BaseCalculator->SetRecoveryLag("0");
    d->BaseCalculator->SetDelinquency("0");
    d->BaseCalculator->SetDelinquencyLag("0");
    QString HardCheks = d->BaseCalculator->ReadyToCalculate();
	if (!HardCheks.isEmpty()) {
		QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + HardCheks);
		return;
	}
	{
		MtgCashFlow FakeFlow;
		FakeFlow.AddFlow(QDate(2015, 5, 30), 1000.0, 0);
        d->Structure.AddMortgagesFlows(FakeFlow);
	}
    HardCheks = d->Structure.ReadyToCalculate();
	if (!HardCheks.isEmpty()) {
		QMessageBox::critical(0, "Invalid Input", "The following Inputs are missing or invalid:\n" + HardCheks);
		return;
	}
    if (d->m_CDRscenarios.isEmpty()) d->m_CDRscenarios.insert("0");
    if (d->m_CPRscenarios.isEmpty()) d->m_CPRscenarios.insert("0");
    if (d->m_LSscenarios.isEmpty()) d->m_LSscenarios.insert("0");
    if (d->m_RecLagScenarios.isEmpty()) d->m_RecLagScenarios.insert("0");
    if (d->m_DelinqScenarios.isEmpty()) d->m_DelinqScenarios.insert("0");
    if (d->m_DelinqLagScenarios.isEmpty()) d->m_DelinqLagScenarios.insert("0");
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
    d->ContinueCalculation = true;
    d->m_ErrorsOccured = false;
#ifdef PrintExecutionTime
	ExecutionTime.start();
#endif
    if (d->ShowProgress) {
        d->ProgressForm = new PhasedProgressWidget();
        d->ProgressForm->AddPhase(tr("Calculating full assets cash flows"), 0, CountScenarios());
        d->ProgressForm->AddPhase(tr("Applying scenarios to aggregated cash flows"), 0, 0);
        d->ProgressForm->AddPhase(tr("Calculating liabilities cash flows"), 0, 100);
        d->ProgressForm->SetTotalProgressLabel(tr("Stress Test"));
        d->ProgressForm->show();
        connect(d->ProgressForm, SIGNAL(Cancelled()), this, SLOT(StopCalculation()));
        connect(this, SIGNAL(AllLoansCalculated()), d->ProgressForm, SLOT(NextPhase()), Qt::QueuedConnection);
        connect(d->TranchesCalculator, SIGNAL(Progress(double)), d->ProgressForm, SLOT(SetPhaseProgress(double)));
        connect(d->BaseApplier, SIGNAL(Progress(double)), d->ProgressForm, SLOT(SetPhaseProgress(double)));
	}
    if (d->UseFastVersion) {
        connect(d->BaseCalculator, SIGNAL(Calculated()), this, SLOT(BaseForFastCalculated()));
        if (d->ShowProgress)
            connect(d->BaseCalculator, SIGNAL(Progress(double)), d->ProgressForm, SLOT(SetPhaseProgress(double)));
        if (!d->BaseCalculator->StartCalculation()) {
			QMessageBox::critical(0, "Invalid Input", "A base rate in the loans is invalid");
			return;
		}
	}
	else
		RunCurrentScenario();
}
void StressTest::BaseForFastCalculated() {
    Q_D( StressTest);
    disconnect(d->BaseCalculator, SIGNAL(Calculated()), this, SLOT(BaseForFastCalculated()));
    if (d->ShowProgress)
        disconnect(d->BaseCalculator, SIGNAL(Progress(double)), d->ProgressForm, SLOT(SetPhaseProgress(double)));
    if (!d->ContinueCalculation) return StoppedCalculation();
    d->BaseApplier->SetBaseFlows(*d->BaseCalculator->GetAggregatedResults());
	RunCurrentScenario();
}
void StressTest::RunCurrentScenario() {
    Q_D( StressTest);
    if (!d->ContinueCalculation)  return StoppedCalculation();
	AssumptionSet CurrentAss(
        *(d->m_CPRscenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssCPR])
        , *(d->m_CDRscenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssCDR])
        , *(d->m_LSscenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssLS])
        , *(d->m_RecLagScenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssRecLag])
        , *(d->m_DelinqScenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssDelinq])
        , *(d->m_DelinqLagScenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssDelinqLag])
	);
	if (
        d->UseFastVersion
		&& !IntegerVector(CurrentAss.GetRecLagScenario()).IsEmpty(0, 0)
		&& !IntegerVector(CurrentAss.GetDelinqLagScenario()).IsEmpty(0, 0)
		&& !BloombergVector(CurrentAss.GetDelinqScenario()).IsEmpty(0.0, 0.0)
	) {
        if (d->ProgressForm) {
            d->ProgressForm->SetPhaseMax(1, 100);
            d->ProgressForm->SetPhaseMax(0, d->ProgressForm->GetPhaseMax(0) - 1);
		}
        d->BaseApplier->AddAssumption(CurrentAss, d->BaseApplier->NumBees());
		emit CurrentScenarioCalculated();
	}
	else {
        connect(d->BaseCalculator, SIGNAL(Calculated()), this, SLOT(SlowLoansCalculated()), Qt::UniqueConnection);
        d->BaseCalculator->SetCPRass(CurrentAss.GetCPRscenario());
        d->BaseCalculator->SetCDRass(CurrentAss.GetCDRscenario());
        d->BaseCalculator->SetLSass(CurrentAss.GetLSscenario());
        d->BaseCalculator->SetRecoveryLag(CurrentAss.GetRecLagScenario());
        d->BaseCalculator->SetDelinquency(CurrentAss.GetDelinqScenario());
        d->BaseCalculator->SetDelinquencyLag(CurrentAss.GetDelinqLagScenario());
        if (!d->BaseCalculator->StartCalculation()) {
			emit ErrorInCalculation();
		}
	}

}
void StressTest::SlowLoansCalculated() {
    Q_D( StressTest);
    if (!d->ContinueCalculation)  return StoppedCalculation();
    d->Structure.ResetMtgFlows();
    d->Structure.AddMortgagesFlows(*d->BaseCalculator->GetAggregatedResults());
	AssumptionSet CurrentAss(
        *(d->m_CPRscenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssCPR])
        , *(d->m_CDRscenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssCDR])
        , *(d->m_LSscenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssLS])
        , *(d->m_RecLagScenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssRecLag])
        , *(d->m_DelinqScenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssDelinq])
        , *(d->m_DelinqLagScenarios.constBegin() + d->CurrentAssumption[StressTestPrivate::AssDelinqLag])
		);
    if (d->ProgressForm)
        d->ProgressForm->SetPhaseProgress(d->ProgressForm->GetPhaseProgress() + 1);
    if (d->m_RainbowTable.contains(qHash(CurrentAss, 88))) {
		emit ErrorInCalculation();
		return;
	}
    d->m_RainbowTable.insert(qHash(CurrentAss, 88), CurrentAss);
    d->Structure.SetAssumptions(CurrentAss);
    d->TranchesCalculator->AddWaterfall(d->Structure, qHash(CurrentAss, 88));
	emit CurrentScenarioCalculated();
}

void StressTest::GoToNextScenario()
{
    Q_D( StressTest);
	if (!IncreaseCurrentAssumption()) {
#ifdef PrintExecutionTime
		QTime TempTime;
        TempTime.addMSecs(d->ExecutionTime.elapsed());
        QMessageBox::information(0, "Computation Time", QString("Stress Test Took: %1 Seconds").arg(d->ExecutionTime.elapsed() / 1000));
#endif
        disconnect(d->BaseCalculator);
        if (d->ProgressForm) d->ProgressForm->NextPhase();
        if (d->BaseApplier->NumBees() > 0) {
            d->BaseApplier->StartCalculation();
		}
		else {
			emit AllLoansCalculated();
		}
		return;
	}
	RunCurrentScenario();
}

void StressTest::ResetCurrentAssumption()
{
    Q_D( StressTest);
    for (int i = 0; i < NumStressDimentsions; ++i) 
        d->CurrentAssumption[i] = 0;
}

bool StressTest::IncreaseCurrentAssumption(int level)
{
    Q_D( StressTest);
	if (level >= NumStressDimentsions || level < 0) return false;
    if (d->CurrentAssumption[level] == d->m_AssumptionsRef[level]->size() - 1) {
        d->CurrentAssumption[level] = 0;
		return IncreaseCurrentAssumption(level - 1);
	}
    d->CurrentAssumption[level]++;
	return true;
}

qint32 StressTest::CountScenariosCalculated(int level)
{
    Q_D( StressTest);
	if (level >= NumStressDimentsions || level < 0) return 0;
    qint32 Result = d->CurrentAssumption[level];
    for (int i = level + 1; i < NumStressDimentsions && Result>0; ++i)
        Result *= d->m_AssumptionsRef[i]->size();
	return Result + CountScenariosCalculated(level - 1);
}



QDataStream& operator<<(QDataStream & stream, const StressTest& flows)
{
	//Deprecated, use SaveResults(), it's much faster
	stream << flows.d_func()->Structure;
    stream << flows.d_func()->StartDate;
    stream << flows.d_func()->SequentialComputation;
    stream << flows.d_func()->ShowProgress;
    stream << flows.d_func()->UseFastVersion;
    std::for_each(std::begin(flows.d_func()->m_AssumptionsRef), std::end(flows.d_func()->m_AssumptionsRef), [&stream](QSet<QString>* i) {stream << *i; });
    stream << static_cast<qint32>(flows.d_func()->Results.size());
    for (auto i = flows.d_func()->Results.constBegin(); i != flows.d_func()->Results.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	return stream;
}

QDataStream& StressTest::LoadOldVersion(QDataStream& stream)
{
    Q_D( StressTest);
	qint32 tempint;
    d->Structure.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->Structure;
    stream >> d->StartDate;
    stream >> d->SequentialComputation;
    stream >> d->ShowProgress;
    stream >> d->UseFastVersion;
    std::for_each(std::begin(d->m_AssumptionsRef), std::end(d->m_AssumptionsRef), [&stream](QSet<QString>* i) {stream >> *i; });
	/*for (int i = 0; i < NumStressDimentsions; ++i) {
		stream >> *(m_AssumptionsRef[i]);
	}*/
	ResetResults();
	stream >> tempint;
	AssumptionSet TempAssSet;
	Waterfall tempWatf;
	for (qint32 i = 0; i < tempint; i++) {
		TempAssSet.SetLoadProtocolVersion(loadProtocolVersion()); stream >> TempAssSet;
        tempWatf.SetLoadProtocolVersion(loadProtocolVersion()); stream >> tempWatf;
        d->Results.insert(TempAssSet, QSharedPointer<Waterfall>(new Waterfall(tempWatf)));
	}
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator>>(QDataStream & stream, StressTest& flows) {
	//Deprecated, use LoadResultsFromFile(), it's much faster
	return flows.LoadOldVersion(stream);
}

void StressTest::SaveResults(const QString& DestPath)const
{
    Q_D(const StressTest);
	QString DestinationPath(DestPath.trimmed());
	if (*(DestinationPath.end() - 1) != '\\' && *(DestinationPath.end() - 1) != '/') DestinationPath.append('/');
	QString OldName = DestinationPath + QString("StressResult.fcsr");
	QString NewName;
	if (QFile::exists(OldName)) {
		NewName = DestinationPath + QString("StressResult%1.fcsr");
		for (quint64 i = 0; ; ++i) {
			if (!QFile::exists(NewName.arg(i))) {
				NewName=NewName.arg(i);
				break;
			}
		}
	}
	else {
		NewName = OldName;
		OldName = QString();
	}
	QScopedPointer<QProgressDialog, QScopedPointerDeleteLater> SaveProgress(nullptr);
    if (d->ShowProgress) {
		int Oldcount = 0;
		if (!OldName.isEmpty()) {
			QuaZip OldZip(OldName);
			if (OldZip.open(QuaZip::mdUnzip)) {
				Oldcount = qMax(0, OldZip.getEntriesCount() - 1);
				OldZip.close();
			}
		}
		SaveProgress.reset(new QProgressDialog(nullptr));
		SaveProgress->setCancelButton(nullptr);
		SaveProgress->setWindowTitle(tr("Please Wait..."));
		SaveProgress->setWindowIcon(QIcon(":/Icons/Logo.png"));
		SaveProgress->setLabelText("Saving Results");
        SaveProgress->setRange(0, 1 + d->Results.size() + Oldcount);
		SaveProgress->setValue(0);
		SaveProgress->show();
	}


	QDir curDir;
	QuaZip zip(NewName);
	QStringList UsedFileNames;
	if (!zip.open(QuaZip::mdCreate)) return;
	QuaZipFile TargetFile(&zip);
	QuaZipNewInfo ZipFileInfo("StressTestInputs");
	UsedFileNames << "StressTestInputs";
	if (TargetFile.open(QIODevice::WriteOnly, ZipFileInfo))
	{
		QDataStream out(&TargetFile);
		out.setVersion(StreamVersionUsed);
        out << qint32(ModelVersionNumber) << d->StartDate << d->SequentialComputation << d->ShowProgress << d->UseFastVersion << *(d->BaseCalculator) << d->Structure;
		TargetFile.close();
        if (d->ShowProgress) UpdateSaveProgress(SaveProgress.data(), SaveProgress->value() + 1);
	}
	ZipFileInfo.dateTime = QDateTime::currentDateTime();
    for (auto i = d->Results.constBegin(); i != d->Results.constEnd(); ++i) {
		ZipFileInfo.name = i.key().ToString() + ".csw";
		UsedFileNames << ZipFileInfo.name;
		if (TargetFile.open(QIODevice::WriteOnly, ZipFileInfo)) {
			QDataStream out(&TargetFile);
			out.setVersion(StreamVersionUsed);
			out << qint32(ModelVersionNumber) << *(i.value());
			TargetFile.close();
            if (d->ShowProgress)
                UpdateSaveProgress(SaveProgress.data(), SaveProgress->value() + 1);
		}
	}

	if (!OldName.isEmpty()) {
		QuaZip OldZip(OldName);
		QuaZipFile OldFile(&OldZip);
		QuaZipFileInfo64 OldFileInfo;
		if (!OldZip.open(QuaZip::mdUnzip)) return;
		for (bool more = OldZip.goToFirstFile(); more; more = OldZip.goToNextFile()) {
			if (OldFile.open(QIODevice::ReadOnly)) {
				if (UsedFileNames.contains(OldFile.getActualFileName())) {
					OldFile.close();
                    if (d->ShowProgress) UpdateSaveProgress(SaveProgress.data(), SaveProgress->value() + 1);
					continue;
				}
				if (!OldFile.getFileInfo(&OldFileInfo)) {
					OldFileInfo.dateTime = QDateTime::currentDateTime();
				}
				ZipFileInfo.name = OldFile.getActualFileName();
				ZipFileInfo.dateTime = OldFileInfo.dateTime;
				UsedFileNames << ZipFileInfo.name;
				if (TargetFile.open(QIODevice::WriteOnly, ZipFileInfo)) {
					TargetFile.write(OldFile.readAll());
					TargetFile.close();
                    if (d->ShowProgress) UpdateSaveProgress(SaveProgress.data(), SaveProgress->value() + 1);
				}
				OldFile.close();
			}

		}
		OldZip.close();
		curDir.remove(OldName);
	}
	zip.close();
	if (!OldName.isEmpty()) QFile::rename(NewName, OldName);
}

Waterfall StressTest::GetScenarioFromFile(const QString& DestPath, const QString& CPRscenario, const QString& CDRscenario, const QString& LSscenario, const QString& RecLagScenario, const QString& DelinqScenario, const QString& DelinqLagScenario)
{
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
	out.setVersion(StreamVersionUsed);
	out >> VesionCheck;
	if (VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)) return Result;
	Result.SetLoadProtocolVersion(VesionCheck);
	out >> Result;
	TargetFile.close();
	return Result;
}
bool StressTest::LoadResultsFromFile(const QString& DestPath)
{
    Q_D( StressTest);
    int argc = 0;
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
		out.setVersion(StreamVersionUsed);
		out >> VesionCheck;
		if(VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)){
			TargetFile.close();
			throw 1;
		}
        out >> d->StartDate >> d->SequentialComputation >> d->ShowProgress >> d->UseFastVersion;
        d->BaseCalculator->SetLoadProtocolVersion(VesionCheck); out >> *(d->BaseCalculator);
        d->Structure.SetLoadProtocolVersion(VesionCheck); out >> d->Structure;
		TargetFile.close();
	}
	catch(int){
		if(!QApplication::instance()){
            QApplication ComputationLoop(argc, 0);
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
		for (auto i = 0; i < NumStressDimentsions; ++i) {
            d->m_AssumptionsRef[i]->insert(Spanns.at(i));
		}
		QDataStream out(&TargetFile);
		out.setVersion(StreamVersionUsed);
		Waterfall* TempWF = new Waterfall();
		out >> VesionCheck;
		if (VesionCheck<qint32(MinimumSupportedVersion) || VesionCheck>qint32(ModelVersionNumber)) {
			TargetFile.close();
			continue;
		}
		TempWF->SetLoadProtocolVersion(VesionCheck);
		out >> (*TempWF);
        d->Results.insert(CurrentAss, QSharedPointer<Waterfall>(TempWF));
		TargetFile.close();
	}
	return true;
}

void StressTest::SetUseFastVersion(bool a/*=true*/)
{
    Q_D( StressTest);
    d->UseFastVersion = a;
}

bool StressTest::GetUseFastVersion() const
{
    Q_D(const StressTest);
    return d->UseFastVersion;
}

quint32 StressTest::RemoveInvalidScenarios()
{
    Q_D( StressTest);
	quint32 InvalidScen = 0;
    for (auto i = d->m_CDRscenarios.begin(); i != d->m_CDRscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)){
            i = d->m_CDRscenarios.erase(i); 
            ++InvalidScen;
            if (i == d->m_CDRscenarios.end())
                break;
		}
	}
    for (auto i = d->m_CPRscenarios.begin(); i != d->m_CPRscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
            i = d->m_CPRscenarios.erase(i);
            ++InvalidScen;
            if (i == d->m_CPRscenarios.end())
                break;
		}
	}
    for (auto i = d->m_LSscenarios.begin(); i != d->m_LSscenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
            i = d->m_LSscenarios.erase(i);
            ++InvalidScen;
            if (i == d->m_LSscenarios.end()) 
                break;
		}
	}
    for (auto i = d->m_DelinqScenarios.begin(); i != d->m_DelinqScenarios.end(); ++i) {
		while (BloombergVector(*i).IsEmpty(0.0, 1.0)) {
            i = d->m_DelinqScenarios.erase(i);
            ++InvalidScen;
            if (i == d->m_DelinqScenarios.end())
                break;
		}
	}
    for (auto i = d->m_RecLagScenarios.begin(); i != d->m_RecLagScenarios.end(); ++i) {
		while (IntegerVector(*i).IsEmpty(0)) {
            i = d->m_RecLagScenarios.erase(i); 
            ++InvalidScen;
            if (i == d->m_RecLagScenarios.end()) 
                break;
		}
	}
    for (auto i = d->m_DelinqLagScenarios.begin(); i != d->m_DelinqLagScenarios.end(); ++i) {
		while (IntegerVector(*i).IsEmpty(0)) {
            i = d->m_DelinqLagScenarios.erase(i); 
            ++InvalidScen;
            if (i == d->m_DelinqLagScenarios.end())
                break;
		}
	}
	return InvalidScen;
}

quint32 StressTest::CountScenarios() const
{
    Q_D(const StressTest);
	return
        d->m_CDRscenarios.size()
        * d->m_CPRscenarios.size()
        * d->m_LSscenarios.size()
        * d->m_RecLagScenarios.size()
        * d->m_DelinqScenarios.size()
        * d->m_DelinqLagScenarios.size();
}

void StressTest::ResetResults()
{
    Q_D( StressTest);
    d->Results.clear();
}

void StressTest::StopCalculation()
{
    Q_D( StressTest);
    d->ContinueCalculation = false;
    d->TranchesCalculator->StopCalculation();
    if (d->ProgressForm) {
        d->ProgressForm->deleteLater();
	}
}

void StressTest::GatherResults()
{
    Q_D( StressTest);
    d->Results.clear();
    d->TranchesCalculator->ClearWaterfalls();
    const auto CalcRes = d->TranchesCalculator->GetResultsKeys();
	for (auto i = CalcRes.constBegin(); i != CalcRes.constEnd(); ++i) {
        auto MatchingAssumption = d->m_RainbowTable.constFind(*i);
        if (MatchingAssumption != d->m_RainbowTable.constEnd()) {
            d->Results[MatchingAssumption.value()] = QSharedPointer<Waterfall>(
                new Waterfall(*(d->TranchesCalculator->GetResult(*i)))
			);
		}
	}
    d->TranchesCalculator->ClearResults();
    if (d->ProgressForm) {
        d->ProgressForm->hide();
        d->ProgressForm->deleteLater();
	}
    if (d->m_ErrorsOccured)
		emit FinishedWithErrors();
	else
		emit AllFinished();
}

void StressTest::ErrorInCalculation()
{
    Q_D( StressTest);
    d->m_ErrorsOccured = true;
}

void StressTest::ResetScenarios()
{
    Q_D(StressTest);
	std::for_each(
		std::begin(d->m_AssumptionsRef)
		, std::end(d->m_AssumptionsRef)
		, [](QSet<QString>* a) { a->clear(); }
	);
}



void StressTest::FastLoansCalculated() {
    Q_D( StressTest);
    QList<qint32> AllKeys = d->BaseApplier->GetAssumptionKeys();
	std::shared_ptr<AssumptionSet> CurrentAss;
	foreach(qint32 SingleKey, AllKeys) {
        CurrentAss = d->BaseApplier->GetAssumption(SingleKey);
		if (!CurrentAss)continue;
		uint CurrentHash = qHash(*CurrentAss, 88);
        if (d->m_RainbowTable.contains(CurrentHash)) {
			emit ErrorInCalculation();
			return;
		}
        d->m_RainbowTable.insert(CurrentHash, *CurrentAss);
        d->Structure.ResetMtgFlows();
        d->Structure.AddMortgagesFlows(*(d->BaseApplier->GetResult(SingleKey)));
        d->Structure.SetAssumptions(*CurrentAss);
        d->TranchesCalculator->AddWaterfall(d->Structure, CurrentHash);
	}
	emit AllLoansCalculated();
}
void StressTest::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) {
    Q_D( StressTest);
    d->Structure.GetBaseRatesDatabase(Values, DownloadAll);
    d->BaseCalculator->GetBaseRatesDatabase(Values, DownloadAll);
}
void StressTest::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) {
    Q_D( StressTest);
    d->Structure.GetBaseRatesDatabase(Values, DownloadAll);
    d->BaseCalculator->GetBaseRatesDatabase(Values, DownloadAll);
}
void StressTest::CompileBaseRates(ConstantBaseRateTable& Values) {
    Q_D( StressTest);
    d->Structure.CompileReferenceRateValue(Values);
    d->BaseCalculator->CompileReferenceRateValue(Values);
}
void StressTest::CompileBaseRates(ForwardBaseRateTable& Values) {
    Q_D( StressTest);
    d->Structure.CompileReferenceRateValue(Values);
    d->BaseCalculator->CompileReferenceRateValue(Values);
}

void StressTest::UpdateSaveProgress(QProgressDialog* prog, int val) const {
	prog->setValue(val);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}




