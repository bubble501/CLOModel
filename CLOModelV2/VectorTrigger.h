#ifndef VectorTrigger_h__
#define VectorTrigger_h__
#include "AbstractTrigger.h"
#include "BoolVector.h"
class VectorTrigger : public AbstractTrigger {
public:
	VectorTrigger(const QString& lab = QString());
	VectorTrigger(const QString& Vec, const QString& lab);
	VectorTrigger(const BoolVector& Vec, const QString& lab = QString());
	VectorTrigger(const VectorTrigger& a) : m_TrigVector(a.m_TrigVector), AbstractTrigger(a) {}
	virtual bool Passing(const QDate& CurrentDate) const;
	virtual bool Failing(const QDate& CurrentDate) const { return !Passing(CurrentDate); }
	virtual bool Passing(int VectorIndex) const;
	virtual bool Failing(int VectorIndex) const { return !Passing(VectorIndex); }
	const BoolVector& GetTrigVector() const { return m_TrigVector; }
	void SetTrigVector(const BoolVector& val) { m_TrigVector = val; }
	void SetTrigVector(const QString& val) { m_TrigVector = val; }
	bool IsEmpty() const { return m_TrigVector.IsEmpty(); }
	bool HasAnchor() const { return !m_TrigVector.GetAnchorDate().isNull(); }
	const QDate& GetAnchor() const { return m_TrigVector.GetAnchorDate(); }
	void SetAnchor(const QDate& a) { m_TrigVector.SetAnchorDate(a); }
	virtual QString ReadyToCalculate() const override;
	virtual QString ToString() const override {	return AbstractTrigger::ToString() + "\nVector: " + m_TrigVector.GetVector(); }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	BoolVector m_TrigVector;
};
#endif // VectorTrigger_h__