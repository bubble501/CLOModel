#include "WaterfallCalculator.h"
#include "Private/WaterfallCalculator_p.h"
#include "Private/InternalItems.h"
#include <QDir>
DEFINE_PUBLIC_QOBJECT_COMMONS(WaterfallCalculator)
WaterfallCalculatorPrivate::~WaterfallCalculatorPrivate() {}
WaterfallCalculatorPrivate::WaterfallCalculatorPrivate(WaterfallCalculator *q)
    :AbstrAsyncCalculatorPrivate(q)
    , m_cascadesFile(TEMP_FILES_DIR)
{
    ENSURE_DIR_EXIST(TEMP_FILES_DIR);
    m_cascadesFile.open();
    m_cascades.setDevice(&m_cascadesFile);
}
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
    d->m_cascades.setValue(ID, a);
}
std::tuple<bool, QString> WaterfallCalculator::StartCalculation(bool ignoreCheck/*=false*/)
{
    Q_D(WaterfallCalculator);
    RETURN_WHEN_RUNNING(true, std::make_tuple(false, "Calculator already running"))
        d->BeesReturned = 0;
    d->BeesSent.clear();
    if (!ignoreCheck) {
        const QString rdyClc = ReadyToCalculate();
        if (!rdyClc.isEmpty())
            return std::make_tuple(false, rdyClc);
    }
    setContinueCalculation (true);
	int NumberOfThreads = availableThreads();
    if (d->m_SequentialComputation || NumberOfThreads < 1) 
        NumberOfThreads = 1;
    LOGDEBUG(QString("Number of Waterfall Threads: %1").arg(NumberOfThreads));
	int NumofSent = 0;
	WaterfallCalcThread* CurrentThread;
    const auto cascadesKeys = d->m_cascades.keys();
    for (auto SingleWaterfall = cascadesKeys.constBegin(); SingleWaterfall != cascadesKeys.constEnd(); ++SingleWaterfall) {
		if (NumofSent >= NumberOfThreads) break;
        if (d->BeesSent.contains(*SingleWaterfall)) continue;
		CurrentThread = AddThread(*SingleWaterfall);
        CurrentThread->SetWaterfall(std::get<1>(d->m_cascades.value<Waterfall>(*SingleWaterfall)));
		CurrentThread->start();
		++NumofSent;
	}
    return std::make_tuple(true, QString());
}

std::tuple<bool, QString> WaterfallCalculator::StartCalculation()
{
    return StartCalculation(false);
}

void WaterfallCalculator::BeeReturned(int Ident, const Waterfall& a)
{
    Q_D(WaterfallCalculator);
    Q_ASSERT(d->m_cascades.contains(Ident));
	RETURN_WHEN_RUNNING(false, )
    TemplAsyncCalculator <WaterfallCalcThread, Waterfall >::BeeReturned(Ident, a);
	WaterfallCalcThread* CurrentThread;
    const auto cascadesKeys = d->m_cascades.keys();
    for (auto SingleWaterfall = cascadesKeys.begin(); SingleWaterfall != cascadesKeys.end(); ++SingleWaterfall) {
        if (d->BeesSent.contains(*SingleWaterfall)) 
            continue;
		CurrentThread = AddThread(*SingleWaterfall);
        CurrentThread->SetWaterfall(std::get<1>(d->m_cascades.value<Waterfall>(*SingleWaterfall)));
		CurrentThread->start();
		return;
	}
}
QString WaterfallCalculator::ReadyToCalculate() const
{
    Q_D(const WaterfallCalculator);
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n")
	QString Res;
    const auto cascadesKeys = d->m_cascades.keys();
    for (auto SingleWaterfall = cascadesKeys.begin(); SingleWaterfall != cascadesKeys.end(); ++SingleWaterfall){
        Res += std::get<1>(d->m_cascades.value<Waterfall>(*SingleWaterfall)).ReadyToCalculate();
    }
	return Res;
}

int WaterfallCalculator::NumBees() const
{
    Q_D(const WaterfallCalculator);
    return d->m_cascades.size();
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
    d->m_cascades.clear();
}

QDataStream& operator<<(QDataStream & stream, const WaterfallCalculator& flows)
{
    Q_ASSERT(!flows.ContinueCalculation());
    stream << static_cast<qint32>(flows.d_func()->m_cascades.size());
    const auto cascadesKeys = flows.d_func()->m_cascades.keys();
    for (auto SingleWaterfall = cascadesKeys.begin(); SingleWaterfall != cascadesKeys.end(); ++SingleWaterfall) {
        stream 
            << *SingleWaterfall 
            <<  std::get<1>(flows.d_func()->m_cascades.value<Waterfall>(*SingleWaterfall)).ReadyToCalculate();
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