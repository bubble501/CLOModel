#ifndef DelinquencyTriggerSettingWidget_h__
#define DelinquencyTriggerSettingWidget_h__

#include "Private/AbstractTriggerSettingWidget.h"
class QLabel;
class QLineEdit;
class DelinquencyTriggerSettingWidget : public AbstractTriggerSettingWidget
{
    Q_OBJECT

public:
    DelinquencyTriggerSettingWidget(QWidget *parent = nullptr);
    virtual void setParameters(const QString& pars) override;
    virtual QString parameters() const override;
    public slots:
    virtual void reset() override;
private:
    QLabel* m_VectorLabel;
    QLineEdit* m_vectorEdit;
};
#endif // DelinquencyTriggerSettingWidget_h__