#include "WaterfallCalculator.h"
WaterfallCalculator::WaterfallCalculator(QObject* parent)
	:TemplAsyncCalculator <WaterfallCalcThread, Waterfall>(parent)
{}
void WaterfallCalculator::AddWaterfall(const Waterfall& a, qint32 ID) {
	if (m_Cascades.contains(ID)) {
		delete m_Cascades[ID];
	}
	m_Cascades[ID] = new Waterfall(a);
}
bool WaterfallCalculator::StartCalculation() {
	if (m_ContinueCalculation) return false;
	BeesReturned = 0;
	BeesSent.clear();
	if (!ReadyToCalculate().isEmpty()) return false;
	m_ContinueCalculation = true;
	int NumberOfThreads = QThread::idealThreadCount();
	if (m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	WaterfallCalcThread* CurrentThread;
	for (auto SingleWaterfall = m_Cascades.begin(); SingleWaterfall != m_Cascades.end(); ++SingleWaterfall) {
		if (NumofSent >= NumberOfThreads) break;
		if (BeesSent.contains(SingleWaterfall.key())) continue;
		CurrentThread = AddThread(SingleWaterfall.key());
		CurrentThread->SetWaterfall(*(SingleWaterfall.value()));
		CurrentThread->start();
		++NumofSent;
	}
}
void WaterfallCalculator::BeeReturned(int Ident, const Waterfall& a) {
	TemplAsyncCalculator <WaterfallCalcThread, Waterfall > ::BeeReturned(Ident, a);
	if (!m_ContinueCalculation) return;
	WaterfallCalcThread* CurrentThread;
	for (auto SingleWaterfall = m_Cascades.begin(); SingleWaterfall != m_Cascades.end(); ++SingleWaterfall) {
		if (BeesSent.contains(SingleWaterfall.key())) continue;
		CurrentThread = AddThread(SingleWaterfall.key());
		CurrentThread->SetWaterfall(*(SingleWaterfall.value()));
		CurrentThread->start();
		return;
	}
}
QString WaterfallCalculator::ReadyToCalculate() const {
	QString Res = "";
	for (auto i = m_Cascades.constBegin(); i != m_Cascades.constEnd(); ++i) {
		Res += i.value()->ReadyToCalculate();
	}
	return Res;
}

void WaterfallCalculator::Reset() {
	ClearWaterfalls();
	TemplAsyncCalculator<WaterfallCalcThread, Waterfall>::Reset();
}

void WaterfallCalculator::ClearWaterfalls() {
	for (auto i = m_Cascades.begin(); i != m_Cascades.end(); ++i) {
		delete i.value();
	}
	m_Cascades.clear();
}
