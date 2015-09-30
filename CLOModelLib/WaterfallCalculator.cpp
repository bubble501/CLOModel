#include "WaterfallCalculator.h"
#include "Private/WaterfallCalculator_p.h"
#include <QTemporaryFile>
DEFINE_PUBLIC_QOBJECT_COMMONS(WaterfallCalculator)
WaterfallCalculatorPrivate::~WaterfallCalculatorPrivate() {}
WaterfallCalculatorPrivate::WaterfallCalculatorPrivate(WaterfallCalculator *q)
    :AbstrAsyncCalculatorPrivate(q)
    , m_dataDir(QDir::tempPath() + "/CLOModel/")
{}
WaterfallCalculator::WaterfallCalculator(WaterfallCalculatorPrivate *d, QObject* parent)
    : TemplAsyncCalculator <WaterfallCalcThread, Waterfall>(d, parent)
{}


WaterfallCalculator::~WaterfallCalculator()
{
    Reset();
}
QString WaterfallCalculatorPrivate::writeTempFile(const Waterfall& val) const
{
    QTemporaryFile destFile(m_dataDir.path() + '/');
    destFile.setAutoRemove(false);
    if (destFile.open()){
        QDataStream out(&destFile);
        out.setVersion(StreamVersionUsed);
        out << val;
        destFile.close();
        return destFile.fileName();
    }
    PrintToTempFile("PermissionError", "Could not write temporary file to save Waterfall");
    return QString();
}
void WaterfallCalculatorPrivate::clearTempDir()
{
    const auto fileList = QDir(m_dataDir.path()).entryInfoList();
    for (auto i = fileList.constBegin(); i != fileList.constEnd(); ++i)
        QFile::remove(i->absoluteFilePath());
}
void WaterfallCalculatorPrivate::removeTempFile(const QString& path) const
{
    QFile::remove(path);
}
Waterfall WaterfallCalculatorPrivate::readTempFile(const QString& path) const
{
    Waterfall result;
    QFile sourceFile(path);
    if (sourceFile.open(QIODevice::ReadOnly)){
        QDataStream in(&sourceFile);
        in.setVersion(StreamVersionUsed);
        in >> result;
        sourceFile.close();
    }
    else
        PrintToTempFile("PermissionError", "Could not read temporary file to load Waterfall");
    return result;
}
void WaterfallCalculator::AddWaterfall(const Waterfall& a, qint32 ID)
{
    Q_D(WaterfallCalculator);
    auto cascIter= d->m_CascadesPath.find(ID);
    if (cascIter == d->m_CascadesPath.end()) {
        RETURN_WHEN_RUNNING(true, )
        d->m_CascadesPath.insert(ID, d->writeTempFile(a));
    }
    else {
        d->removeTempFile(cascIter.value());
        cascIter.value() = d->writeTempFile(a);
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
		CurrentThread->SetWaterfall(d->readTempFile(SingleWaterfall.value()));
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
    AddWaterfall(a, Ident);
    TemplAsyncCalculator <WaterfallCalcThread, Waterfall >::BeeReturned(Ident, a);
    auto& tempRes = getResultVoid();
    auto i = tempRes.find(Ident);
    if (i != tempRes.end())
        i.value().reset();
	WaterfallCalcThread* CurrentThread;
    for (auto SingleWaterfall = d->m_CascadesPath.begin(); SingleWaterfall != d->m_CascadesPath.end(); ++SingleWaterfall) {
        if (d->BeesSent.contains(SingleWaterfall.key())) 
            continue;
		CurrentThread = AddThread(SingleWaterfall.key());
        CurrentThread->SetWaterfall(d->readTempFile(SingleWaterfall.value()));
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
        Res += d->readTempFile(i.value()).ReadyToCalculate();
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

void WaterfallCalculator::RemoveResult(qint32 Key)
{
    Q_D(WaterfallCalculator);
    auto cascIter = d->m_CascadesPath.find(Key);
    if (cascIter != d->m_CascadesPath.end()){
        d->removeTempFile(cascIter.value());
        d->m_CascadesPath.erase(cascIter);
    }
}

const std::shared_ptr<Waterfall> WaterfallCalculator::GetResult(qint32 key) const
{
    Q_D(const WaterfallCalculator);
    if (d->m_CascadesPath.contains(key))
        return std::make_shared<Waterfall>(d->readTempFile(d->m_CascadesPath.value(key)));
    return std::shared_ptr<Waterfall>(nullptr);
}

void WaterfallCalculator::ClearResults()
{
    ClearWaterfalls();
}

QDataStream& operator<<(QDataStream & stream, const WaterfallCalculator& flows)
{
    Q_ASSERT(!flows.ContinueCalculation());
    stream << static_cast<qint32>(flows.d_func()->m_CascadesPath.size());
    for (auto i = flows.d_func()->m_CascadesPath.constBegin(); i != flows.d_func()->m_CascadesPath.constEnd(); i++) {
        stream << i.key() << flows.d_func()->readTempFile(i.value());
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