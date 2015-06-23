#ifndef DelinquencyTrigger_h__
#define DelinquencyTrigger_h__
#include "AbstractTrigger.h"
class DelinquencyTriggerPrivate;
class CLOMODELLIB_EXPORT DelinquencyTrigger : public AbstractTrigger
{
    DECLARE_PUBLIC_COMMONS(DelinquencyTrigger)
    DECLARE_PUBLIC_COMMONS_COPY(DelinquencyTrigger)
public:
	DelinquencyTrigger(const BloombergVector& Target, const QString& lab = QString());
	DelinquencyTrigger(const QString& Target, const QString& lab);
	DelinquencyTrigger(const QString& lab);
	virtual bool Passing(double CurrentDelinq,const QDate& CurrentPeriod) const;
	virtual bool Passing(double CurrentDelinq, int CurrentPeriod) const;
	virtual bool Failing(double CurrentDelinq, const QDate& CurrentPeriod) const;
	virtual bool Failing(double CurrentDelinq, int CurrentPeriod) const;
	virtual QString ReadyToCalculate() const override;
	const BloombergVector& GetTarget() const;
	void SetTarget(const BloombergVector& val);
	void SetTarget(const QString& val);
	void SetAnchorDate(const QDate& a);
	void RemoveAnchorDate();
	bool HasAnchor()const;
	virtual QString ToString() const override;
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	
};
#endif // DelinquencyTrigger_h__