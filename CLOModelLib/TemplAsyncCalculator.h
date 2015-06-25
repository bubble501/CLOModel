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
	virtual void Reset();
	virtual void ClearResults();
    virtual void RemoveResult(qint32 Key);
    virtual const ResultType* GetResult(qint32 key)const { return static_cast<const ResultType*>(getResultVoid(key)); }
protected:	
    TemplAsyncCalculator(AbstrAsyncCalculatorPrivate* d,QObject* parent = nullptr);
	virtual ThreadType* AddThread(qint32 Key);
	virtual void BeeReturned(int Ident, const ResultType& a);
	virtual void HandleErrorInCalculation(int a) { BeeReturned(a, ResultType()); }
   
	virtual QDataStream& SaveToStream(QDataStream& stream) const final;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
};

template <typename ThreadType, typename ResultType>
void TemplAsyncCalculator<ThreadType, ResultType>::RemoveResult(qint32 Key)
{
    auto& tempRes = getResultVoid();
    auto i = tempRes.find(Key);
    if (i == tempRes.end())
        return;
    delete i.value();
    tempRes.erase(i);
}

template <typename ThreadType, typename ResultType>
TemplAsyncCalculator<ThreadType, ResultType>::TemplAsyncCalculator(QObject* parent)
    :AbstrAsyncCalculator(parent)
{
    static_assert(std::is_base_of<QThread, TemplAsyncThread<ResultType> >::value, "ThreadType must inherit from TemplAsyncThread");
    static_assert(std::is_object<ThreadType>::value, "ThreadType can't be a reference or pointer");
    static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor");
    static_assert(std::is_base_of<BackwardInterface, ResultType >::value, "ResultType must inherit from BackwardInterface");
    if (!QMetaType::isRegistered(qMetaTypeId<ResultType>())) {
        int TypeID = qRegisterMetaType<ResultType>(typeid(ResultType).name());
        qRegisterMetaTypeStreamOperators<ResultType>(typeid(ResultType).name());
    }   
}
template <typename ThreadType, typename ResultType>
TemplAsyncCalculator<ThreadType, ResultType>::TemplAsyncCalculator(AbstrAsyncCalculatorPrivate* d, QObject* parent)
    :AbstrAsyncCalculator(d,parent)
{
    static_assert(std::is_base_of<QThread, TemplAsyncThread<ResultType> >::value, "ThreadType must inherit from TemplAsyncThread");
    static_assert(std::is_object<ThreadType>::value, "ThreadType can't be a reference or pointer");
    static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor");
    static_assert(std::is_base_of<BackwardInterface, ResultType >::value, "ResultType must inherit from BackwardInterface");
    if (!QMetaType::isRegistered(qMetaTypeId<ResultType>())) {
        int TypeID = qRegisterMetaType<ResultType>(typeid(ResultType).name());
        qRegisterMetaTypeStreamOperators<ResultType>(typeid(ResultType).name());
    }
}

template <typename ThreadType, typename ResultType>
QDataStream& TemplAsyncCalculator<ThreadType, ResultType>::LoadOldVersion(QDataStream& stream) {
	RETURN_WHEN_RUNNING(true,stream)
	qint32 TempSize,TempKey;
	ResultType* TempRes=nullptr;
    bool sequentComp;
	ClearResults();
    stream >> sequentComp >> TempSize;
    SetSequentialComputation(sequentComp);
	for (qint32 i = 0; i < TempSize; i++) {
		TempRes = new ResultType();
		stream >> TempKey;
		TempRes->SetLoadProtocolVersion(loadProtocolVersion());
		stream >> (*TempRes);
		insertResult(TempKey, TempRes);
	}
	ResetProtocolVersion();
	return stream;
}

template <typename ThreadType, typename ResultType>
QDataStream& TemplAsyncCalculator<ThreadType, ResultType>::SaveToStream(QDataStream& stream) const {
	RETURN_WHEN_RUNNING(true, stream)
    auto& tempRes = getResultVoid();
    stream << GetSequentialComputation() << static_cast<qint32>(tempRes.size());
    for (auto i = tempRes.constBegin(); i != tempRes.constEnd(); ++i) {
        stream << i.key() << *static_cast<ResultType*>(i.value());
	}
	return stream;
}


template <typename ThreadType, typename ResultType>
void TemplAsyncCalculator<ThreadType, ResultType>::ClearResults() {
    RETURN_WHEN_RUNNING(true, )
    auto& tempRes = getResultVoid();
    for (auto j = tempRes.begin(); j != tempRes.end();) {
		delete j.value();
        j=tempRes.erase(j);
	}
    Q_ASSERT(getResultVoid().isEmpty());
}

template <typename ThreadType, typename ResultType>
TemplAsyncCalculator<ThreadType, ResultType>::~TemplAsyncCalculator() {
	Reset();
}
template <typename ThreadType, typename ResultType>
void TemplAsyncCalculator<ThreadType, ResultType>::Reset() {
	ContinueCalculation(false);
	ThreadType* CurrentRunning;
    auto& tempThreadPool = getThreadPool();
    for (auto j = tempThreadPool.begin(); j != tempThreadPool.end();) {
        CurrentRunning = static_cast<ThreadType*>(j.value().data());
		if (CurrentRunning) {
			if (CurrentRunning->isRunning()) {
				CurrentRunning->exit();
				CurrentRunning->wait();
			}
		}
        j = tempThreadPool.erase(j);
	}
    Q_ASSERT(getThreadPool().isEmpty());
	ClearResults();
	getBeeSent().clear();
	getBeesReturned() = 0;
}
template <typename ThreadType, typename ResultType>
ThreadType* TemplAsyncCalculator<ThreadType, ResultType>::AddThread(qint32 Key) {
	ThreadType* a = new ThreadType(Key, this);
    auto& tempThreadPool = getThreadPool();
    auto FoundThread = tempThreadPool.find(Key);
    if (FoundThread != tempThreadPool.end()) {
		if (FoundThread.value()) {
            ThreadType* oldThread = static_cast<ThreadType*>(FoundThread.value().data());
            if (oldThread->isRunning()) {
                oldThread->exit();
                oldThread->wait();
			}
            oldThread->deleteLater();
		}
	}
    getBeeSent().insert(Key);
	connect(a, &ThreadType::Calculated, this, &TemplAsyncCalculator<ThreadType, ResultType>::BeeReturned);
	connect(a, SIGNAL(AnonimCalculated(int)), a, SLOT(stop()), Qt::QueuedConnection);

	connect(a, SIGNAL(ErrorCalculation(int)), this, SIGNAL(BeeError(int)));
	connect(a, &ThreadType::ErrorCalculation, this, &TemplAsyncCalculator<ThreadType, ResultType>::HandleErrorInCalculation);
	connect(a, SIGNAL(ErrorCalculation(int)), a, SLOT(stop()), Qt::QueuedConnection);
    tempThreadPool[Key] = a;
    Q_ASSERT(getThreadPool().contains(Key));
	return a;
}
template <typename ThreadType, typename ResultType>
void TemplAsyncCalculator<ThreadType, ResultType>::BeeReturned(int Ident, const ResultType& a) {
    RETURN_WHEN_RUNNING(false, )
    auto& tempRes = getResultVoid();
    auto FindRe = tempRes.find(Ident);
    if (FindRe != tempRes.end()) {
		delete FindRe.value();
        tempRes.erase(FindRe);
        Q_ASSERT(!getResultVoid().contains(Ident));
	}
	insertResult(Ident, new ResultType(a));
	getThreadPool().remove(Ident);
	//emit BeeCalculated(++BeesReturned);
    getBeesReturned()++;
    Q_ASSERT(getBeesReturned() <= NumBees());
    emit BeeCalculated(Ident);
    emit Progress(static_cast<double>(getBeesReturned()) / static_cast<double>(NumBees()));
    if (getBeesReturned() == NumBees() && ContinueCalculation()) {
        ContinueCalculation(false);
		emit Calculated();
	}
}
#endif // TemplAsyncCalculator_h__

