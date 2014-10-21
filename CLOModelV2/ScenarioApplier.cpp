#include "ScenarioApplier.h"
#include "ApplyFlowThread.h"
#include <QSettings>
#include <QDir>
ScenarioApplier::ScenarioApplier(QObject* parent) 
	: TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>(parent)
{}

void ScenarioApplier::AddAssumption(const AssumptionSet& a, qint32 idx) {
	auto FoundAss = m_Scenarios.find(idx);
	if (FoundAss != m_Scenarios.end()) {
		delete FoundAss.value();
		m_Scenarios.erase(FoundAss);
	}
	m_Scenarios.insert(idx, new AssumptionSet(a));
}

bool ScenarioApplier::StartCalculation() {
	if (m_ContinueCalculation) return false;
	BeesReturned = 0;
	BeesSent.clear();
	ClearResults();
	if (!ReadyToCalculate().isEmpty()) return false;
	m_ContinueCalculation = true;
	int NumberOfThreads = QThread::idealThreadCount();
	if (m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	ApplyFlowThread* CurrentThread;
	for (auto SingleScen = m_Scenarios.constBegin(); SingleScen != m_Scenarios.constEnd(); ++SingleScen) {
		if (NumofSent >= NumberOfThreads) break;
		if (BeesSent.contains(SingleScen.key())) continue;
		CurrentThread = AddThread(SingleScen.key());
		CurrentThread->SetAssumption(*(SingleScen.value()));
		CurrentThread->SetBaseFlow(m_BaseFlows);
		CurrentThread->start();
		++NumofSent;
	}
	return true;
}
void ScenarioApplier::BeeReturned(int Ident, const MtgCashFlow& a) {
	TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::BeeReturned(Ident, a);
	if (!m_ContinueCalculation) return;
	ApplyFlowThread* CurrentThread;
	for (auto SingleScen = m_Scenarios.constBegin(); SingleScen != m_Scenarios.constEnd(); ++SingleScen) {
		if (BeesSent.contains(SingleScen.key())) continue;
		CurrentThread = AddThread(SingleScen.key());
		CurrentThread->SetAssumption(*(SingleScen.value()));
		CurrentThread->SetBaseFlow(m_BaseFlows);
		CurrentThread->start();
		return;
	}
}
void ScenarioApplier::Reset() {
	ClearScenarios();
	TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::Reset();
}
void ScenarioApplier::ClearScenarios() {
	for (auto i = m_Scenarios.begin(); i != m_Scenarios.end(); ++i) {
		delete i.value();
	}
	m_Scenarios.clear();
}
QString ScenarioApplier::ReadyToCalculate()const {
	QString Result;
	if (m_BaseFlows.IsEmpty()) Result += "Invalid Base Flows\n";
	for (auto i = m_Scenarios.constBegin(); i != m_Scenarios.constEnd(); i++) {
		if (!i.value()->IsValid())
			Result += "Invalid Scenario: " + i.value()->ToString() + '\n';
	}
	if (!Result.isEmpty()) return Result.left(Result.size() - 1);
	return Result;
}

QDataStream& operator<<(QDataStream & stream, const ScenarioApplier& flows) {
	stream << flows.m_BaseFlows;
	stream << static_cast<qint32>(flows.m_Scenarios.size());
	for (auto i = flows.m_Scenarios.begin(); i != flows.m_Scenarios.end(); i++) {
		stream << i.key() << *(i.value());
	}
	return flows.SaveToStream(stream);
}
QDataStream& ScenarioApplier::LoadOldVersion(QDataStream& stream) {
	Reset();
	qint32 TempSize, TempKey;
	stream >> m_BaseFlows;
	stream >> TempSize;
	AssumptionSet* TempRes=nullptr;
	for (qint32 i = 0; i < TempSize; i++) {
		TempRes = new AssumptionSet();
		stream >> TempKey;
		TempRes->SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> (*TempRes);
		m_Scenarios.insert(TempKey, TempRes);
	}
	return TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::LoadOldVersion(stream);
}

qint32 ScenarioApplier::FindAssumption(const AssumptionSet& a)const {
	for (auto i = m_Scenarios.constBegin(); i != m_Scenarios.constEnd(); ++i) {
		if (a == *(i.value())) return i.key();
	}
	return -1;
}
QDataStream& operator>>(QDataStream & stream, ScenarioApplier& flows) {
	return flows.LoadOldVersion(stream);
}
