#ifndef DuringStressTestTrigger_h__
#define DuringStressTestTrigger_h__
#include "AbstractTrigger.h"
class DuringStressTestTrigger : public AbstractTrigger {
public:
	DuringStressTestTrigger(const QString& lab = QString()) :AbstractTrigger(TriggerType::DuringStressTestTrigger, lab) {}
	DuringStressTestTrigger(const DuringStressTestTrigger& a) : AbstractTrigger(a) {}
	virtual bool Passing(bool IsStressTest) const { return IsStressTest; }
	virtual bool Failing(bool IsStressTest) const { return !Passing(IsStressTest); }
	virtual QString ReadyToCalculate() const override { return QString(); }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override { return stream; }
};
#endif // DuringStressTestTrigger_h__