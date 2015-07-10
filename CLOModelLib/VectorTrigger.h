#ifndef VectorTrigger_h__
#define VectorTrigger_h__
#include "AbstractTrigger.h"
class BoolVector;
class VectorTriggerPrivate;
class VectorTriggerSettingWidget;
class QWidget;
class CLOMODELLIB_EXPORT VectorTrigger : public AbstractTrigger
{
    DECLARE_PUBLIC_COMMONS(VectorTrigger)
    DECLARE_PUBLIC_COMMONS_COPY(VectorTrigger)
public:
	VectorTrigger(const QString& lab);
	VectorTrigger(const QString& Vec, const QString& lab);
	VectorTrigger(const BoolVector& Vec, const QString& lab = QString());
	virtual bool Passing(const QDate& CurrentDate) const;
	virtual bool Failing(const QDate& CurrentDate) const;
	virtual bool Passing(int VectorIndex) const;
	virtual bool Failing(int VectorIndex) const;
	const BoolVector& GetTrigVector() const;
	void SetTrigVector(const BoolVector& val);
	void SetTrigVector(const QString& val);
	bool IsEmpty() const;
	bool HasAnchor() const;
	const QDate& GetAnchor() const;
	void SetAnchor(const QDate& a);
	virtual QString ReadyToCalculate() const override;
	virtual QString ToString() const override;
    static VectorTriggerSettingWidget* createSettingsWidget(QWidget* parent = nullptr);
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
};
#endif // VectorTrigger_h__