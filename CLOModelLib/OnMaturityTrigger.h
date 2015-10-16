#ifndef OnMaturityTrigger_h__
#define OnMaturityTrigger_h__

#include "AbstractTrigger.h"
class AbstractTriggerSettingWidget;
class QWidget;
class CLOMODELLIB_EXPORT OnMaturityTrigger : public AbstractTrigger
{
public:
    OnMaturityTrigger(const QString& lab = QString());
    OnMaturityTrigger(const OnMaturityTrigger& other);
    virtual ~OnMaturityTrigger();
    virtual bool Passing(bool IsMaturity) const;
    virtual bool Failing(bool IsMaturity) const;
    virtual QString ReadyToCalculate() const override;
    static AbstractTriggerSettingWidget* createSettingsWidget(QWidget* parent = nullptr);
protected:
    OnMaturityTrigger(AbstractTriggerPrivate* d);
    OnMaturityTrigger(AbstractTriggerPrivate* d, const OnMaturityTrigger& other);
    virtual QDataStream& WriteToStream(QDataStream& stream) const override;
};
#endif // OnMaturityTrigger_h__