#ifndef TemplAsyncCalculator_h__
#define TemplAsyncCalculator_h__
#include <QObject>
#include <QHash>
#include <QSet>
#include <QPointer>
#include <AbstrAsyncCalculator.h>
#include <type_traits>
#include <QDataStream>
template <typename ThreadType, typename ResultType>
class CLOMODELLIB_EXPORT TemplAsyncCalculator : public AbstrAsyncCalculator
{
public:
	TemplAsyncCalculator(QObject* parent = nullptr);
	virtual ~TemplAsyncCalculator();
	virtual void SetSequentialComputation(bool a) { 
        d_func()->m_SequentialComputation = a;
	}
	virtual bool GetSequentialComputation()const { return d_func()->m_SequentialComputation; }
	virtual void Reset();
	virtual void ClearResults();
	virtual QList<qint32> GetResultsKeys() const { return m_Result.keys(); }
	virtual int NumBees() const = 0;
    virtual const ResultType* GetResult(qint32 key)const { return static_cast<ResultType>(m_Result.value(key, nullptr)); }
	virtual const QHash<qint32, ResultType*>& GetResults() const { return m_Result; }
protected:	
    TemplAsyncCalculator(AbstrAsyncCalculatorPrivate* d,QObject* parent = nullptr);
	virtual ThreadType* AddThread(qint32 Key);
	virtual void BeeReturned(int Ident, const ResultType& a);
	virtual void HandleErrorInCalculation(int a) { BeeReturned(a, ResultType()); }
    virtual void RemoveResult(qint32 Key);
	virtual QDataStream& SaveToStream(QDataStream& stream) const final;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
};

template <typename ThreadType, typename ResultType>
TemplAsyncCalculator<ThreadType, ResultType>::TemplAsyncCalculator(QObject* parent)
    :AbstrAsyncCalculator(parent)
{
    static_assert(std::is_base_of<QThread, TemplAsyncThread<ResultType> >::value, "ThreadType must inherit from TemplAsyncThread");
    static_assert(std::is_object<ThreadType>::value, "ThreadType can't be a reference or pointer");
    static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor");
    static_assert(std::is_base_of<BackwardInterface, ResultType >::value, "ResultType must inherit from BackwardInterface");
    RegisterAsMetaType<ResultType>();
    d->m_SequentialComputation = false;
    d->BeesReturned = 0;
}
template <typename ThreadType, typename ResultType>
TemplAsyncCalculator<ThreadType, ResultType>::TemplAsyncCalculator(AbstrAsyncCalculatorPrivate* d, QObject* parent)
    :AbstrAsyncCalculator(d,parent)
{
    static_assert(std::is_base_of<QThread, TemplAsyncThread<ResultType> >::value, "ThreadType must inherit from TemplAsyncThread");
    static_assert(std::is_object<ThreadType>::value, "ThreadType can't be a reference or pointer");
    static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor");
    static_assert(std::is_base_of<BackwardInterface, ResultType >::value, "ResultType must inherit from BackwardInterface");
    RegisterAsMetaType<ResultType>();
    d->m_SequentialComputation = false;
    d->BeesReturned = 0;
}

template <typename ThreadType, typename ResultType>
void TemplAsyncCalculator<ThreadType, ResultType>::RemoveResult(qint32 Key)
{
    auto i=m_Result.find(Key);
    if (i == m_Result.end())
        return;
    delete i.value();
    m_Result.erase(i);
}


template <typename ThreadType, typename ResultType>
QDataStream& TemplAsyncCalculator<ThreadType, ResultType>::LoadOldVersion(QDataStream& stream) {
	RETURN_WHEN_RUNNING(true,stream)
	qint32 TempSize,TempKey;
	ResultType* TempRes=nullptr;
	ClearResults();
	stream >> m_SequentialComputation >> TempSize;
	for (qint32 i = 0; i < TempSize; i++) {
		TempRes = new ResultType();
		stream >> TempKey;
		TempRes->SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> (*TempRes);
		m_Result.insert(TempKey, TempRes);
	}
	ResetProtocolVersion();
	return stream;
}

template <typename ThreadType, typename ResultType>
QDataStream& TemplAsyncCalculator<ThreadType, ResultType>::SaveToStream(QDataStream& stream) const {
	RETURN_WHEN_RUNNING(true, stream)
	stream << m_SequentialComputation << static_cast<qint32>(m_Result.size());
	for (auto i = m_Result.constBegin(); i != m_Result.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	return stream;
}


template <typename ThreadType, typename ResultType>
void TemplAsyncCalculator<ThreadType, ResultType>::ClearResults() {
	RETURN_WHEN_RUNNING(true, )
	for (auto j = m_Result.begin(); j != m_Result.end(); j++) {
		delete j.value();
	}
	m_Result.clear();
}

template <typename ThreadType, typename ResultType>
TemplAsyncCalculator<ThreadType, ResultType>::~TemplAsyncCalculator() {
	Reset();
}
template <typename ThreadType, typename ResultType>
void TemplAsyncCalculator<ThreadType, ResultType>::Reset() {
	m_ContinueCalculation = false;
	ThreadType* CurrentRunning;
	for (auto j = m_ThreadPool.begin(); j != m_ThreadPool.end(); j++) {
		CurrentRunning = j.value();
		if (CurrentRunning) {
			if (CurrentRunning->isRunning()) {
				CurrentRunning->exit();
				CurrentRunning->wait();
			}
		}
	}
	m_ThreadPool.clear();
	ClearResults();
	BeesSent.clear();
	BeesReturned = 0;
}
template <typename ThreadType, typename ResultType>
ThreadType* TemplAsyncCalculator<ThreadType, ResultType>::AddThread(qint32 Key) {
	ThreadType* a = new ThreadType(Key, this);
	auto FoundThread = m_ThreadPool.find(Key);
	if (FoundThread != m_ThreadPool.end()) {
		if (FoundThread.value()) {
			if (FoundThread.value()->isRunning()) {
				FoundThread.value()->exit();
				FoundThread.value()->wait();
			}
			FoundThread.value()->deleteLater();
		}
	}
	BeesSent.insert(Key);
	connect(a, &ThreadType::Calculated, this, &TemplAsyncCalculator<ThreadType, ResultType>::BeeReturned);
	connect(a, SIGNAL(AnonimCalculated(int)), a, SLOT(stop()), Qt::QueuedConnection);

	connect(a, SIGNAL(ErrorCalculation(int)), this, SIGNAL(BeeError(int)));
	connect(a, &ThreadType::ErrorCalculation, this, &TemplAsyncCalculator<ThreadType, ResultType>::HandleErrorInCalculation);
	connect(a, SIGNAL(ErrorCalculation(int)), a, SLOT(stop()), Qt::QueuedConnection);
	m_ThreadPool[Key] = a;
	return a;
}
template <typename ThreadType, typename ResultType>
void TemplAsyncCalculator<ThreadType, ResultType>::BeeReturned(int Ident, const ResultType& a) {
	RETURN_WHEN_RUNNING(false, )
	auto FindRe = m_Result.find(Ident);
	if (FindRe != m_Result.end()) {
		delete FindRe.value();
		m_Result.erase(FindRe);
	}
	m_Result.insert(Ident, new ResultType(a));
	m_ThreadPool.remove(Ident);
	//emit BeeCalculated(++BeesReturned);
    ++BeesReturned;
    Q_ASSERT(BeesReturned <= NumBees());
    emit BeeCalculated(Ident);
	emit Progress(static_cast<double>(BeesReturned) / static_cast<double>(NumBees()));
    if (BeesReturned == NumBees() && m_ContinueCalculation) {
		m_ContinueCalculation = false;
		emit Calculated();
	}
}
#endif // TemplAsyncCalculator_h__

