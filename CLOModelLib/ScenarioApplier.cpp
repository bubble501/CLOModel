#include "ScenarioApplier.h"
#include "Private/ScenarioApplier_p.h"
#include <QDir>
DEFINE_PUBLIC_QOBJECT_COMMONS(ScenarioApplier)
ScenarioApplierPrivate::ScenarioApplierPrivate(ScenarioApplier *q)
    :AbstrAsyncCalculatorPrivate(q)
{}
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
	RETURN_WHEN_RUNNING(true, )
    auto FoundAss = d->m_Scenarios.find(idx);
    if (FoundAss != d->m_Scenarios.end()) {
		delete FoundAss.value();
        d->m_Scenarios.erase(FoundAss);
	}
    d->m_Scenarios.insert(idx, new AssumptionSet(a));
}

const MtgCashFlow* ScenarioApplier::GetResult(const AssumptionSet& a) const
{
    Q_D(const ScenarioApplier);
    return GetResult(FindAssumption(a));
}

bool ScenarioApplier::StartCalculation()
{
    Q_D( ScenarioApplier);
	RETURN_WHEN_RUNNING(true, false)
    d->BeesReturned = 0;
    d->BeesSent.clear();
	ClearResults();
	if (!ReadyToCalculate().isEmpty()) return false;
    d->m_ContinueCalculation = true;
	int NumberOfThreads = QThread::idealThreadCount();
    if (d->m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	ApplyFlowThread* CurrentThread;
    for (auto SingleScen = d->m_Scenarios.constBegin(); SingleScen != d->m_Scenarios.constEnd(); ++SingleScen) {
		if (NumofSent >= NumberOfThreads) break;
        if (d->BeesSent.contains(SingleScen.key())) continue;
		CurrentThread = AddThread(SingleScen.key());
		CurrentThread->SetAssumption(*(SingleScen.value()));
        CurrentThread->SetBaseFlow(d->m_BaseFlows);
		CurrentThread->start();
		++NumofSent;
	}
	return true;
}
void ScenarioApplier::BeeReturned(int Ident, const MtgCashFlow& a)
{
    Q_D( ScenarioApplier);
	RETURN_WHEN_RUNNING(false, )
	TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::BeeReturned(Ident, a);
	ApplyFlowThread* CurrentThread;
    for (auto SingleScen = d->m_Scenarios.constBegin(); SingleScen != d->m_Scenarios.constEnd(); ++SingleScen) {
        if (d->BeesSent.contains(SingleScen.key())) continue;
		CurrentThread = AddThread(SingleScen.key());
		CurrentThread->SetAssumption(*(SingleScen.value()));
        CurrentThread->SetBaseFlow(d->m_BaseFlows);
		CurrentThread->start();
		return;
	}
}
void ScenarioApplier::Reset()
{
    Q_D( ScenarioApplier);
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
    for (auto i = d->m_Scenarios.begin(); i != d->m_Scenarios.end(); ++i) {
		delete i.value();
	}
    d->m_Scenarios.clear();
}

const AssumptionSet* ScenarioApplier::GetAssumption(qint32 idx) const
{
    Q_D(const ScenarioApplier);
    return d->m_Scenarios.value(idx, nullptr);
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
    for (auto i = d->m_Scenarios.constBegin(); i != d->m_Scenarios.constEnd(); i++) {
		if (!i.value()->IsValid())
			Result += "Invalid Scenario: " + i.value()->ToString() + '\n';
	}
	if (!Result.isEmpty()) return Result.left(Result.size() - 1);
	return Result;
}

QDataStream& operator<<(QDataStream & stream, const ScenarioApplier& flows) {
	if (flows.d_func()->m_ContinueCalculation) return stream;
    stream << flows.d_func()->m_BaseFlows;
    stream << static_cast<qint32>(flows.d_func()->m_Scenarios.size());
    for (auto i = flows.d_func()->m_Scenarios.constBegin(); i != flows.d_func()->m_Scenarios.constEnd(); ++i) {
		stream << i.key() << *(i.value());
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
	AssumptionSet* TempRes=nullptr;
	for (qint32 i = 0; i < TempSize; i++) {
		TempRes = new AssumptionSet();
		stream >> TempKey;
		TempRes->SetLoadProtocolVersion(loadProtocolVersion());
		stream >> (*TempRes);
        d->m_Scenarios.insert(TempKey, TempRes);
	}
	return TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::LoadOldVersion(stream);
}

qint32 ScenarioApplier::FindAssumption(const AssumptionSet& a)const
{
    Q_D(const ScenarioApplier);
    for (auto i = d->m_Scenarios.constBegin(); i != d->m_Scenarios.constEnd(); ++i) {
		if (a == *(i.value())) return i.key();
	}
	return -1;
}
QDataStream& operator>>(QDataStream & stream, ScenarioApplier& flows) {
	return flows.LoadOldVersion(stream);
}
