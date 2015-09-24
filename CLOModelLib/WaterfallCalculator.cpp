#include "WaterfallCalculator.h"
#include "Private/WaterfallCalculator_p.h"
DEFINE_PUBLIC_QOBJECT_COMMONS(WaterfallCalculator)
WaterfallCalculatorPrivate::WaterfallCalculatorPrivate(WaterfallCalculator *q)
    :AbstrAsyncCalculatorPrivate(q)
{}
WaterfallCalculator::WaterfallCalculator(WaterfallCalculatorPrivate *d, QObject* parent)
    : TemplAsyncCalculator <WaterfallCalcThread, Waterfall>(d, parent)
{}


WaterfallCalculator::~WaterfallCalculator()
{
    Reset();
}

void WaterfallCalculator::AddWaterfall(const Waterfall& a, qint32 ID)
{
    Q_D(WaterfallCalculator);
    RETURN_WHEN_RUNNING(true, )
    auto cascIter= d->m_Cascades.find(ID);
    if (cascIter == d->m_Cascades.end())
        d->m_Cascades.insert(ID, std::make_shared<Waterfall>(a));
    else
        cascIter.value().reset(new Waterfall(a));
}
bool WaterfallCalculator::StartCalculation()
{
    Q_D(WaterfallCalculator);
	RETURN_WHEN_RUNNING(true, false)
        d->BeesReturned = 0;
    d->BeesSent.clear();
	if (!ReadyToCalculate().isEmpty()) return false;
    d->m_ContinueCalculation = true;
	int NumberOfThreads = availableThreads();
    if (d->m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	WaterfallCalcThread* CurrentThread;
    for (auto SingleWaterfall = d->m_Cascades.begin(); SingleWaterfall != d->m_Cascades.end(); ++SingleWaterfall) {
		if (NumofSent >= NumberOfThreads) break;
        if (d->BeesSent.contains(SingleWaterfall.key())) continue;
		CurrentThread = AddThread(SingleWaterfall.key());
		CurrentThread->SetWaterfall(*(SingleWaterfall.value()));
		CurrentThread->start();
		++NumofSent;
	}
	return true;
}
void WaterfallCalculator::BeeReturned(int Ident, const Waterfall& a)
{
    Q_D(WaterfallCalculator);
	RETURN_WHEN_RUNNING(false, )
	TemplAsyncCalculator <WaterfallCalcThread, Waterfall > ::BeeReturned(Ident, a);
    /*
    TODO avoid duplications, copy the result to the waterfall in cascades and then delete the result
    */
	WaterfallCalcThread* CurrentThread;
    for (auto SingleWaterfall = d->m_Cascades.begin(); SingleWaterfall != d->m_Cascades.end(); ++SingleWaterfall) {
        if (d->BeesSent.contains(SingleWaterfall.key())) continue;
		CurrentThread = AddThread(SingleWaterfall.key());
		CurrentThread->SetWaterfall(*(SingleWaterfall.value()));
		CurrentThread->start();
		return;
	}
}
QString WaterfallCalculator::ReadyToCalculate() const
{
    Q_D(const WaterfallCalculator);
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n")
	QString Res = "";
    for (auto i = d->m_Cascades.constBegin(); i != d->m_Cascades.constEnd(); ++i) {
		Res += i.value()->ReadyToCalculate();
	}
	return Res;
}

int WaterfallCalculator::NumBees() const
{
    Q_D(const WaterfallCalculator);
    return d->m_Cascades.size();
}

void WaterfallCalculator::Reset()
{
	RETURN_WHEN_RUNNING(true, )
	ClearWaterfalls();
	TemplAsyncCalculator<WaterfallCalcThread, Waterfall>::Reset();
}

void WaterfallCalculator::ClearWaterfalls()
{
    Q_D(WaterfallCalculator);
	RETURN_WHEN_RUNNING(true, )
    d->m_Cascades.clear();
}
QDataStream& operator<<(QDataStream & stream, const WaterfallCalculator& flows)
{
    if (flows.d_func()->m_ContinueCalculation) 
        return stream;
    stream << static_cast<qint32>(flows.d_func()->m_Cascades.size());
    for (auto i = flows.d_func()->m_Cascades.constBegin(); i != flows.d_func()->m_Cascades.constEnd(); i++) {
        stream << i.key() << *(i.value());
    }
    return flows.SaveToStream(stream);
}
QDataStream& WaterfallCalculator::LoadOldVersion(QDataStream& stream)
{
    RETURN_WHEN_RUNNING(true, stream)
    Reset();
    qint32 tempInt, TempKey;
    Waterfall tempWtf;
    stream >> tempInt;
    while (tempInt--){
        stream >> TempKey;
        tempWtf.SetLoadProtocolVersion(loadProtocolVersion());
        stream >> tempWtf;
        AddWaterfall(tempWtf, TempKey);
    }
    return TemplAsyncCalculator<WaterfallCalcThread, Waterfall>::LoadOldVersion(stream);
}
QDataStream& operator>>(QDataStream & stream, WaterfallCalculator& flows)
{
    return flows.LoadOldVersion(stream);
}