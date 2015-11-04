#ifndef AbstrAsyncCalculator_h__
#define AbstrAsyncCalculator_h__
#include "BackwardCompatibilityInterface.h"
#include <QObject>
#include <memory>
#include <QString>
#include <QFile>
#include <tuple>
#ifndef RETURN_WHEN_RUNNING
#define RETURN_WHEN_RUNNING(rvr,retval) if( ContinueCalculation() == rvr) return retval;
#endif
class MemoryMappedDevice;
class AbstrAsyncCalculatorPrivate;
namespace ConcurrentFunctions { 
    QString checkReadyToCalculateLoan(const QString& path); 
    QString checkReadyToCalculateWaterfall(const QString& path);
}
class CLOMODELLIB_EXPORT AbstrAsyncCalculator : public QObject, public BackwardInterface
{
	Q_OBJECT	
protected:
    friend QString ConcurrentFunctions::checkReadyToCalculateLoan(const QString& path);
    friend QString ConcurrentFunctions::checkReadyToCalculateWaterfall(const QString& path);
    inline AbstrAsyncCalculatorPrivate* d_func() { return reinterpret_cast<AbstrAsyncCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const AbstrAsyncCalculatorPrivate* d_func() const { return reinterpret_cast<const AbstrAsyncCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class AbstrAsyncCalculatorPrivate;
    AbstrAsyncCalculator(AbstrAsyncCalculatorPrivate *d, QObject* parent = nullptr);
    virtual QHash<qint32, QPointer<QObject> >& getThreadPool();
    virtual const QHash<qint32, QPointer<QObject> >& getThreadPool() const;
    virtual const QSet<qint32>& getBeeSent() const;
    virtual QSet<qint32>& getBeeSent();
    virtual qint32& getBeesReturned();
    virtual const qint32& getBeesReturned() const;
    virtual bool ContinueCalculation() const;
    virtual void setContinueCalculation(bool val);
    virtual int availableThreads() const;
    virtual MemoryMappedDevice& getDevice();
    virtual const MemoryMappedDevice& getDevice() const;
public:
    virtual quint8 operativity() const;
    virtual void setOperativity(quint8 val);
    AbstrAsyncCalculator(QObject* parent =nullptr);
    virtual ~AbstrAsyncCalculator();
    virtual QString ReadyToCalculate() const = 0;
    virtual void SetSequentialComputation(bool a);
    virtual bool GetSequentialComputation()const;
    virtual QList<qint32> GetResultKeys() const;
    virtual int NumBees() const = 0;
    const QMultiHash<int, QString>& errors() const;
    void clearErrors();
    QString allErrors() const;
signals :
	void Calculated();
	void BeeCalculated(int);
	void BeeError(int,QString);
	void Progress(double);
	void ProgressPct(int);
	void Stopped();
private slots:
	void SendPctSignal(double a);
    void addError(int id, QString err);
public slots:
	virtual void StopCalculation();
	virtual std::tuple<bool,QString> StartCalculation()=0;
};
#endif // AbstrAsyncCalculator_h__

