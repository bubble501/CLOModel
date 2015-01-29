#include "WaterfallCalculator.h"
WaterfallCalculator::WaterfallCalculator(QObject* parent)
	:TemplAsyncCalculator <WaterfallCalcThread, Waterfall>(parent)
{}
void WaterfallCalculator::AddWaterfall(const Waterfall& a, qint32 ID) {
	RETURN_WHEN_RUNNING(true, )
	if (m_Cascades.contains(ID)) {
		delete m_Cascades[ID];
	}
	m_Cascades[ID] = new Waterfall(a);
}
bool WaterfallCalculator::StartCalculation() {
	RETURN_WHEN_RUNNING(true, false)
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
	return true;
}
void WaterfallCalculator::BeeReturned(int Ident, const Waterfall& a) {
	RETURN_WHEN_RUNNING(false, )
	TemplAsyncCalculator <WaterfallCalcThread, Waterfall > ::BeeReturned(Ident, a);
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
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n")
	QString Res = "";
	for (auto i = m_Cascades.constBegin(); i != m_Cascades.constEnd(); ++i) {
		Res += i.value()->ReadyToCalculate();
	}
	return Res;
}

void WaterfallCalculator::Reset() {
	RETURN_WHEN_RUNNING(true, )
	ClearWaterfalls();
	TemplAsyncCalculator<WaterfallCalcThread, Waterfall>::Reset();
}

void WaterfallCalculator::ClearWaterfalls() {
	RETURN_WHEN_RUNNING(true, )
	for (auto i = m_Cascades.begin(); i != m_Cascades.end(); ++i) {
		delete i.value();
	}
	m_Cascades.clear();
}
