
#ifndef AbstrAsyncCalculator_h__
#define AbstrAsyncCalculator_h__
#include "BackwardCompatibilityInterface.h"
#include <QObject>
#include <QHash>
#include <atomic>
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
public:
	AbstrAsyncCalculator(QObject* parent = 0) :QObject(parent) { 
		m_ContinueCalculation = false; 
		connect(this, &AbstrAsyncCalculator::Progress, [&](double a) {
			emit ProgressPct(static_cast<int>(a*100.0));
		});
	}
	virtual QString ReadyToCalculate() const = 0;
public slots:
	virtual void StopCalculation() { m_ContinueCalculation = false; }
	virtual bool StartCalculation()=0;
};
#endif // AbstrAsyncCalculator_h__

