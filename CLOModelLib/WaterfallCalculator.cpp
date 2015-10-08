#include "WaterfallCalculator.h"
#include "Private/WaterfallCalculator_p.h"
#include <QTemporaryFile>
DEFINE_PUBLIC_QOBJECT_COMMONS(WaterfallCalculator)
WaterfallCalculatorPrivate::~WaterfallCalculatorPrivate() {}
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
void WaterfallCalculatorPrivate::clearTempDir()
{
    const auto fileList = QDir(m_dataDir.path()).entryInfoList();
    for (auto i = fileList.constBegin(); i != fileList.constEnd(); ++i)
        QFile::remove(i->absoluteFilePath());
}

void WaterfallCalculator::AddWaterfall(const Waterfall& a, qint32 ID)
{
    Q_D(WaterfallCalculator);
    auto cascIter= d->m_CascadesPath.find(ID);
    if (cascIter == d->m_CascadesPath.end()) {
        RETURN_WHEN_RUNNING(true, )
        d->m_CascadesPath.insert(ID, writeTempFile(a));
    }
    else {
        removeTempFile(cascIter.value());
        cascIter.value() = writeTempFile(a);
    }

}
bool WaterfallCalculator::StartCalculation()
{
    Q_D(WaterfallCalculator);
	RETURN_WHEN_RUNNING(true, false)
        d->BeesReturned = 0;
    d->BeesSent.clear();
	if (!ReadyToCalculate().isEmpty()) return false;
    setContinueCalculation (true);
	int NumberOfThreads = availableThreads();
    if (d->m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	WaterfallCalcThread* CurrentThread;
    for (auto SingleWaterfall = d->m_CascadesPath.begin(); SingleWaterfall != d->m_CascadesPath.end(); ++SingleWaterfall) {
		if (NumofSent >= NumberOfThreads) break;
        if (d->BeesSent.contains(SingleWaterfall.key())) continue;
		CurrentThread = AddThread(SingleWaterfall.key());
		CurrentThread->SetWaterfall(readTempFile<Waterfall>(SingleWaterfall.value()));
		CurrentThread->start();
		++NumofSent;
	}
	return true;
}
void WaterfallCalculator::BeeReturned(int Ident, const Waterfall& a)
{
    Q_D(WaterfallCalculator);
    Q_ASSERT(d->m_CascadesPath.contains(Ident));
	RETURN_WHEN_RUNNING(false, )
    TemplAsyncCalculator <WaterfallCalcThread, Waterfall >::BeeReturned(Ident, a);
	WaterfallCalcThread* CurrentThread;
    for (auto SingleWaterfall = d->m_CascadesPath.begin(); SingleWaterfall != d->m_CascadesPath.end(); ++SingleWaterfall) {
        if (d->BeesSent.contains(SingleWaterfall.key())) 
            continue;
		CurrentThread = AddThread(SingleWaterfall.key());
        CurrentThread->SetWaterfall(readTempFile<Waterfall>(SingleWaterfall.value()));
		CurrentThread->start();
		return;
	}
}
QString WaterfallCalculator::ReadyToCalculate() const
{
    Q_D(const WaterfallCalculator);
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n")
	QString Res = "";
    for (auto i = d->m_CascadesPath.constBegin(); i != d->m_CascadesPath.constEnd(); ++i) {
        Res += readTempFile<Waterfall>(i.value()).ReadyToCalculate();
	}
	return Res;
}

int WaterfallCalculator::NumBees() const
{
    Q_D(const WaterfallCalculator);
    return d->m_CascadesPath.size();
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
    d->m_CascadesPath.clear();
    d->clearTempDir();
}

QDataStream& operator<<(QDataStream & stream, const WaterfallCalculator& flows)
{
    Q_ASSERT(!flows.ContinueCalculation());
    stream << static_cast<qint32>(flows.d_func()->m_CascadesPath.size());
    for (auto i = flows.d_func()->m_CascadesPath.constBegin(); i != flows.d_func()->m_CascadesPath.constEnd(); i++) {
        stream << i.key() << flows.readTempFile<Waterfall>(i.value());
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