#ifndef AbstractTrigger_h__
#define AbstractTrigger_h__
#include "BackwardCompatibilityInterface.h"
#include <QString>
class AbstractTriggerPrivate;
class AbstractTrigger;
CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows);
CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows);
class CLOMODELLIB_EXPORT AbstractTrigger : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS_COPY(AbstractTrigger)
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
        , OnMaturityTrigger =9
	};
protected:
    Q_DECLARE_PRIVATE(AbstractTrigger);
    AbstractTrigger(AbstractTriggerPrivate* d, TriggerType TTP, const QString& lab = QString());
	virtual QDataStream& WriteToStream(QDataStream& stream) const = 0;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
    virtual ~AbstractTrigger();
	QString TriggerTypeToString(TriggerType a) const;
	AbstractTrigger(TriggerType TTP, const QString& lab = QString());
	TriggerType GetTriggerType() const;
	const QString& GetTriggerLabel() const;
	void SetTriggerLabel(const QString& lab);
	virtual QString ReadyToCalculate() const = 0;
	virtual QString ToString() const;
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows);
};
#endif // AbstractTrigger_h__
