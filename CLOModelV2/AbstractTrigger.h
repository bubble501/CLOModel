#ifndef AbstractTrigger_h__
#define AbstractTrigger_h__
#include "BackwardCompatibilityInterface.h"
#include <QString>
#define NegateChar '!'
class AbstractTrigger : public BackwardInterface{
public:
	enum class TriggerType : quint8 {
		DateTrigger
	};
protected:
	TriggerType m_TriggerType;
	QString m_TriggerLabel;
	virtual QDataStream& WriteToStream(QDataStream& stream) const = 0;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	AbstractTrigger(TriggerType TTP, const QString& lab = QString()) :m_TriggerType(TTP), m_TriggerLabel(lab) {}
	TriggerType GetTriggerType() const { return m_TriggerType; }
	const QString& GetTriggerLabel() const { return m_TriggerLabel; }
	void SetTriggerLabel(const QString& lab) { m_TriggerLabel = lab; }
	friend QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows);
	friend QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows);
};
QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows);
QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows);
#endif // AbstractTrigger_h__
