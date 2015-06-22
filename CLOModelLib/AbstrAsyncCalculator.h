
#ifndef AbstrAsyncCalculator_h__
#define AbstrAsyncCalculator_h__
#include "BackwardCompatibilityInterface.h"
#include <QObject>
#ifndef RETURN_WHEN_RUNNING
#define RETURN_WHEN_RUNNING(rvr,retval) if(m_ContinueCalculation == rvr) return retval;
#endif
class AbstrAsyncCalculatorPrivate;
class CLOMODELLIB_EXPORT AbstrAsyncCalculator : public QObject, public BackwardInterface
{
	Q_OBJECT	
protected:
    inline AbstrAsyncCalculatorPrivate* d_func() { return reinterpret_cast<AbstrAsyncCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const AbstrAsyncCalculatorPrivate* d_func() const { return reinterpret_cast<const AbstrAsyncCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class AbstrAsyncCalculatorPrivate;
    AbstrAsyncCalculator(AbstrAsyncCalculatorPrivate *d, QObject* parent = nullptr);
public:
    AbstrAsyncCalculator(QObject* parent =nullptr);
    virtual QString ReadyToCalculate() const = 0;
signals :
	void Calculated();
	void BeeCalculated(int);
	void BeeError(int);
	void Progress(double);
	void ProgressPct(int);
	void Stopped();
private slots:
	void SendPctSignal(double a);
public slots:
	virtual void StopCalculation();
	virtual bool StartCalculation()=0;
};
#endif // AbstrAsyncCalculator_h__

