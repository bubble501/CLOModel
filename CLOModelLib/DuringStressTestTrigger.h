#ifndef DuringStressTestTrigger_h__
#define DuringStressTestTrigger_h__
#include "AbstractTrigger.h"
class DuringStressTestTrigger : public AbstractTrigger { 
public:
    DuringStressTestTrigger(const QString& lab=QString());
    DuringStressTestTrigger(const DuringStressTestTrigger& other);
	virtual bool Passing(bool IsStressTest) const;
	virtual bool Failing(bool IsStressTest) const;
	virtual QString ReadyToCalculate() const override;
protected:
    DuringStressTestTrigger(AbstractTriggerPrivate* d);
    DuringStressTestTrigger(AbstractTriggerPrivate* d, const DuringStressTestTrigger& other);
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
};
#endif // DuringStressTestTrigger_h__