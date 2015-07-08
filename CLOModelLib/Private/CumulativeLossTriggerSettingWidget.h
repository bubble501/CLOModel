#ifndef CumulativeLossTriggerSettingWidget_h__
#define CumulativeLossTriggerSettingWidget_h__

#include "Private/AbstractTriggerSettingWidget.h"
class QLabel;
class QLineEdit;
class QComboBox;
class CumulativeLossTriggerSettingWidget : public AbstractTriggerSettingWidget
{
    Q_OBJECT
public:
    CumulativeLossTriggerSettingWidget(QWidget *parent = nullptr);
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
#endif // CumulativeLossTriggerSettingWidget_h__
