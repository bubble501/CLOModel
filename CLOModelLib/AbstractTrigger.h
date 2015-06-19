#ifndef AbstractTrigger_h__
#define AbstractTrigger_h__
#include "BackwardCompatibilityInterface.h"
#include <QString>
class AbstractTriggerPrivate;
class CLOMODELLIB_EXPORT AbstractTrigger : public BackwardInterface
{
protected:
    Q_DECLARE_PRIVATE(AbstractTrigger)
public:
	enum TriggerType : quint8 {
		DateTrigger=0
		,VectorTrigger=1
		,PoolSizeTrigger=2
		,TrancheTrigger=3
		,DelinquencyTrigger=4
		,DuringStressTestTrigger=5
		, CumulativeLossTrigger=6
		, DeferredInterestTrigger=7
        , PDLTrigger=8
	};
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const = 0;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	QString TriggerTypeToString(TriggerType a) const;
	AbstractTrigger(const AbstractTrigger& a);
	AbstractTrigger(TriggerType TTP, const QString& lab = QString());
	TriggerType GetTriggerType() const;
	const QString& GetTriggerLabel() const;
	void SetTriggerLabel(const QString& lab);
	virtual QString ReadyToCalculate() const = 0;
	virtual QString ToString() const;
	friend QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows);
	friend QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows);
};
QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows);
QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows);
#endif // AbstractTrigger_h__
