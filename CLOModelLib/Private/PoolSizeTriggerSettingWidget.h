#ifndef PoolSizeTriggerSettingWidget_h__
#define PoolSizeTriggerSettingWidget_h__

#include "Private/AbstractTriggerSettingWidget.h"
class QLabel;
class QLineEdit;
class QComboBox;
class PoolSizeTriggerSettingWidget : public AbstractTriggerSettingWidget
{
    Q_OBJECT
public:
    PoolSizeTriggerSettingWidget(QWidget *parent = nullptr);
    virtual void setParameters(const QString& pars) override;
    virtual QString parameters() const override;
public slots:
    virtual void reset() override;
private:
    QLabel* m_VectorLabel;
    QLabel* m_SideLabel;
    QLineEdit* m_vectorEdit;
    QComboBox* m_sideCombo;
};
#endif // PoolSizeTriggerSettingWidget_h__