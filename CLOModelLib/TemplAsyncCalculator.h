#ifndef TemplAsyncCalculator_h__
#define TemplAsyncCalculator_h__
#include <QObject>
#include <QHash>
#include <QSet>
#include <QPointer>
#include <AbstrAsyncCalculator.h>
#include <type_traits>
#include <QDataStream>
#include "MemoryMappedDevice.h"
template <typename ThreadType, typename ResultType>
class CLOMODELLIB_EXPORT TemplAsyncCalculator : public AbstrAsyncCalculator
{
public:
    TemplAsyncCalculator(QObject* parent = nullptr) 
        :AbstrAsyncCalculator(parent)
    {
        static_assert(std::is_base_of<QThread, TemplAsyncThread<ResultType> >::value, "ThreadType must inherit from TemplAsyncThread");
        static_assert(std::is_object<ThreadType>::value, "ThreadType can't be a reference or pointer");
        static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor");
        static_assert(std::is_base_of<BackwardInterface, ResultType >::value, "ResultType must inherit from BackwardInterface");
        if (QMetaType::type(typeid(ResultType).name()) == QMetaType::UnknownType) {
            qRegisterMetaType<ResultType>(typeid(ResultType).name());
            qRegisterMetaTypeStreamOperators<ResultType>(typeid(ResultType).name());
        }
    }
    virtual ~TemplAsyncCalculator()
    {
        Reset();
    }
    virtual void Reset()
    {
        setContinueCalculation(false);
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
    virtual void ClearResults()
    {
        getResultsDevice().clear();
    }
    virtual void RemoveResult(qint32 Key)
    {
        getResultsDevice().removeValue(Key);
    }
    virtual const ResultType GetResult(qint32 key)const { 
        return std::get<1>(getResultsDevice().value<ResultType>(key));
    }
protected:	
    TemplAsyncCalculator(AbstrAsyncCalculatorPrivate* d, QObject* parent = nullptr)
        :AbstrAsyncCalculator(d, parent)
    {
        static_assert(std::is_base_of<QThread, TemplAsyncThread<ResultType> >::value, "ThreadType must inherit from TemplAsyncThread");
        static_assert(std::is_object<ThreadType>::value, "ThreadType can't be a reference or pointer");
        static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor");
        static_assert(std::is_base_of<BackwardInterface, ResultType >::value, "ResultType must inherit from BackwardInterface");
        if (QMetaType::type(typeid(ResultType).name()) == QMetaType::UnknownType) {
            qRegisterMetaType<ResultType>(typeid(ResultType).name());
            qRegisterMetaTypeStreamOperators<ResultType>(typeid(ResultType).name());
        }
    }
    virtual ThreadType* AddThread(qint32 Key)
    {
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
        connect(a, &ThreadType::AnonimCalculated, a, &ThreadType::stop, Qt::QueuedConnection);

        connect(a, &ThreadType::ErrorCalculation, this, &TemplAsyncCalculator<ThreadType, ResultType>::BeeError);
        connect(a, &ThreadType::ErrorCalculation, this, &TemplAsyncCalculator<ThreadType, ResultType>::HandleErrorInCalculation);
        connect(a, &ThreadType::ErrorCalculation, a, &ThreadType::stop, Qt::QueuedConnection);
        tempThreadPool[Key] = a;
        Q_ASSERT(getThreadPool().contains(Key));
        return a;
    }
    void insertResult(qint32 Key, const ResultType& val)
    {
        getResultsDevice().setValue(Key, val);
    }
    virtual void BeeReturned(int Ident, const ResultType& a)
    {
        RETURN_WHEN_RUNNING(false, )
        insertResult(Ident,a);
        getThreadPool().remove(Ident);
        getBeesReturned()++;
        Q_ASSERT(getBeesReturned() <= NumBees());
        emit BeeCalculated(Ident);
        emit Progress(static_cast<double>(getBeesReturned()) / static_cast<double>(NumBees()));
        if (getBeesReturned() == NumBees() && ContinueCalculation()) {
            setContinueCalculation(false);
            emit Calculated();
        }
    }
	virtual void HandleErrorInCalculation(int a) { BeeReturned(a, ResultType()); }
   
    virtual QDataStream& SaveToStream(QDataStream& stream) const final
    {
        RETURN_WHEN_RUNNING(true, stream)
            auto tempRes = GetResultKeys();
        stream << GetSequentialComputation() << static_cast<qint32>(tempRes.size());
        for (auto i = tempRes.constBegin(); i != tempRes.constEnd(); ++i) {
            stream << *i
                << std::get<1>(getResultsDevice().value<ResultType>(*i));
        }
        return stream;
    }
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override
    {
        RETURN_WHEN_RUNNING(true, stream)
            qint32 TempSize, TempKey;
        ResultType TempRes;
        bool sequentComp;
        ClearResults();
        stream >> sequentComp >> TempSize;
        SetSequentialComputation(sequentComp);
        for (qint32 i = 0; i < TempSize; i++) {
            stream >> TempKey;
            TempRes.SetLoadProtocolVersion(loadProtocolVersion());
            stream >> TempRes;
            insertResult(TempKey, TempRes);
        }
        ResetProtocolVersion();
        return stream;
    }
};
#endif // TemplAsyncCalculator_h__

