
#ifndef AbstrAsyncCalculator_h__
#define AbstrAsyncCalculator_h__
#include "BackwardCompatibilityInterface.h"
#include <QObject>
#include <QHash>
#include <atomic>
class AbstrAsyncCalculator : public QObject, public BackwardInterface {
	Q_OBJECT
public:
	AbstrAsyncCalculator(QObject* parent = 0) :QObject(parent) { m_ContinueCalculation = false; }
	virtual QString ReadyToCalculate() const = 0;
protected:
	std::atomic_bool m_ContinueCalculation;
public slots:
	virtual void StopCalculation() { m_ContinueCalculation = false; }
	virtual bool StartCalculation()=0;
signals:
	void Calculated();
	void BeeCalculated(int);
	void BeeError(int);
	void Progress(double);
};
#endif // AbstrAsyncCalculator_h__

