
#ifndef AbstrAsyncCalculator_h__
#define AbstrAsyncCalculator_h__
#include "BackwardCompatibilityInterface.h"
#include <QObject>
#include <QHash>
#include <atomic>
#ifndef RETURN_WHEN_RUNNING
#define RETURN_WHEN_RUNNING(rvr,retval) if(m_ContinueCalculation == rvr) return retval;
#endif
class AbstrAsyncCalculator : public QObject, public BackwardInterface {
	Q_OBJECT
protected:
	std::atomic_bool m_ContinueCalculation;
signals :
	void Calculated();
	void BeeCalculated(int);
	void BeeError(int);
	void Progress(double);
	void ProgressPct(int);
	void Stopped();
public:
	AbstrAsyncCalculator(QObject* parent = 0) :QObject(parent) { 
		m_ContinueCalculation = false; 
		connect(this, &AbstrAsyncCalculator::Progress, this, &AbstrAsyncCalculator::SendPctSignal);
	}
	virtual QString ReadyToCalculate() const = 0;
private slots:
	void SendPctSignal(double a) {emit ProgressPct(static_cast<int>(a*100.0));}
public slots:
	virtual void StopCalculation() { m_ContinueCalculation = false; emit Stopped(); }
	virtual bool StartCalculation()=0;
};
#endif // AbstrAsyncCalculator_h__

