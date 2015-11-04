#include "ScenarioApplier.h"
#include "Private/ScenarioApplier_p.h"
#include <QDir>
#include "AssumptionSet.h"
#include "Private/InternalItems.h"
DEFINE_PUBLIC_QOBJECT_COMMONS(ScenarioApplier)
ScenarioApplierPrivate::~ScenarioApplierPrivate(){}
ScenarioApplierPrivate::ScenarioApplierPrivate(ScenarioApplier *q)
    :AbstrAsyncCalculatorPrivate(q)
    , m_ScenariosFile(TEMP_FILES_DIR)
{
    ENSURE_DIR_EXIST(TEMP_FILES_DIR);
    m_ScenariosFile.open();
    m_Scenarios.setDevice(&m_ScenariosFile);
}
ScenarioApplier::ScenarioApplier(ScenarioApplierPrivate *d, QObject* parent)
    : TemplAsyncCalculator <ApplyFlowThread, MtgCashFlow>(d, parent)
{}

ScenarioApplier::~ScenarioApplier()
{
    Reset();
}

const MtgCashFlow& ScenarioApplier::GetBaseFlows() const
{
    Q_D(const ScenarioApplier);
    return d->m_BaseFlows;
}

void ScenarioApplier::SetBaseFlows(const MtgCashFlow& val)
{
    Q_D( ScenarioApplier);
    RETURN_WHEN_RUNNING(true, )
    d->m_BaseFlows = val;
}

void ScenarioApplier::AddAssumption(const AssumptionSet& a, qint32 idx)
{
    Q_D( ScenarioApplier);
    d->m_Scenarios.setValue(idx, a);
}

const MtgCashFlow ScenarioApplier::GetResult(const AssumptionSet& a) const
{
    return GetResult(FindAssumption(a));
}

std::tuple<bool, QString> ScenarioApplier::StartCalculation()
{
    Q_D( ScenarioApplier);
    RETURN_WHEN_RUNNING(true, std::make_tuple(false, "Calculator already running"))
    d->BeesReturned = 0;
    d->BeesSent.clear();
	ClearResults();
    const QString rdyClc = ReadyToCalculate();
    if (!rdyClc.isEmpty())
        return std::make_tuple(false, rdyClc);
    setContinueCalculation ( true);
	int NumberOfThreads = availableThreads();
    if (d->m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	ApplyFlowThread* CurrentThread;
    const auto scenariosKeys = d->m_Scenarios.keys();
    for (auto SingleScen = scenariosKeys.constBegin(); SingleScen != scenariosKeys.constEnd(); ++SingleScen) {
		if (NumofSent >= NumberOfThreads) break;
        if (d->BeesSent.contains(*SingleScen)) continue;
		CurrentThread = AddThread(*SingleScen);
		CurrentThread->SetAssumption(std::get<1>(d->m_Scenarios.value<AssumptionSet>(*SingleScen)));
        CurrentThread->SetBaseFlow(d->m_BaseFlows);
		CurrentThread->start();
		++NumofSent;
	}
    return std::make_tuple(true, QString());
}
void ScenarioApplier::BeeReturned(int Ident, const MtgCashFlow& a)
{
    Q_D( ScenarioApplier);
	RETURN_WHEN_RUNNING(false, )
	TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::BeeReturned(Ident, a);
	ApplyFlowThread* CurrentThread;
    const auto scenariosKeys = d->m_Scenarios.keys();
    for (auto SingleScen = scenariosKeys.constBegin(); SingleScen != scenariosKeys.constEnd(); ++SingleScen) {
        if (d->BeesSent.contains(*SingleScen)) continue;
		CurrentThread = AddThread(*SingleScen);
        CurrentThread->SetAssumption(std::get<1>(d->m_Scenarios.value<AssumptionSet>(*SingleScen)));
        CurrentThread->SetBaseFlow(d->m_BaseFlows);
		CurrentThread->start();
		return;
	}
}
void ScenarioApplier::Reset()
{
	RETURN_WHEN_RUNNING(true, )
	ClearScenarios();
	TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::Reset();
}

int ScenarioApplier::NumBees() const
{
    Q_D(const ScenarioApplier);
    return d->m_Scenarios.size();
}

void ScenarioApplier::ClearScenarios()
{
    Q_D( ScenarioApplier);
	RETURN_WHEN_RUNNING(true, )
    d->m_Scenarios.clear();
}

AssumptionSet ScenarioApplier::GetAssumption(qint32 idx) const
{
    Q_D(const ScenarioApplier);
    return std::get<1>(d->m_Scenarios.value<AssumptionSet>(idx));
}

QList<qint32> ScenarioApplier::GetAssumptionKeys() const
{
    Q_D(const ScenarioApplier);
    return d->m_Scenarios.keys();
}

QString ScenarioApplier::ReadyToCalculate()const
{
    Q_D(const ScenarioApplier);
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n")
	QString Result;
    if (d->m_BaseFlows.IsEmpty()) Result += "Invalid Base Flows\n";
    const auto scenariosKeys = d->m_Scenarios.keys();
    for (auto i = scenariosKeys.constBegin(); i != scenariosKeys.constEnd(); i++) {
        const auto currAss = std::get<1>(d->m_Scenarios.value<AssumptionSet>(*i));
        if (!currAss.IsValid())
            Result += "Invalid Scenario: " + currAss.ToString() + '\n';
	}
	if (!Result.isEmpty()) return Result.left(Result.size() - 1);
	return Result;
}

QDataStream& operator<<(QDataStream & stream, const ScenarioApplier& flows) {
	Q_ASSERT (!flows.ContinueCalculation()) ;
    stream << flows.d_func()->m_BaseFlows;
    stream << static_cast<qint32>(flows.d_func()->m_Scenarios.size());
    const auto scenariosKeys = flows.d_func()->m_Scenarios.keys();
    for (auto i = scenariosKeys.constBegin(); i != scenariosKeys.constEnd(); ++i) {
        stream << *i << std::get<1>(flows.d_func()->m_Scenarios.value<AssumptionSet>(*i));
	}
	return flows.SaveToStream(stream);
}
QDataStream& ScenarioApplier::LoadOldVersion(QDataStream& stream)
{
    Q_D( ScenarioApplier);
	RETURN_WHEN_RUNNING(true, stream)
	Reset();
	qint32 TempSize, TempKey;
    stream >> d->m_BaseFlows;
	stream >> TempSize;
	AssumptionSet TempRes;
	for (qint32 i = 0; i < TempSize; i++) {
		stream >> TempKey;
		TempRes.SetLoadProtocolVersion(loadProtocolVersion());
		stream >> TempRes;
        d->m_Scenarios.setValue(TempKey, TempRes);
	}
	return TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::LoadOldVersion(stream);
}

qint32 ScenarioApplier::FindAssumption(const AssumptionSet& a)const
{
    Q_D(const ScenarioApplier);
    const auto scenariosKeys = d->m_Scenarios.keys();
    for (auto i = scenariosKeys.constBegin(); i != scenariosKeys.constEnd(); ++i) {
        if (a == std::get<1>(d->m_Scenarios.value<AssumptionSet>(*i)))
            return *i;
	}
	return -1;
}
QDataStream& operator>>(QDataStream & stream, ScenarioApplier& flows) {
	return flows.LoadOldVersion(stream);
}
