#ifndef DEFERREDINTERESTSETTINGWIDGET_H
#define DEFERREDINTERESTSETTINGWIDGET_H

#include "Private/AbstractTriggerSettingWidget.h"
class QLabel;
class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class DeferredInterestTriggerSettingWidget : public AbstractTriggerSettingWidget
{
    Q_OBJECT
public:
    DeferredInterestTriggerSettingWidget(QWidget *parent = nullptr);
    virtual void setParameters(const QString& pars) override;
    virtual QString parameters() const override;
public slots:
    virtual void reset() override;
private:
    QLabel* m_seniorityLabel;
    QLabel* m_seniorityLevelLabel;
    QLabel* m_sizeLabel;
    QLabel* m_couponLabel;
    QLabel* m_multiplierLabel;
    QLabel* m_senioritySideLabel;
    QLabel* m_sizeSideLabel;
    QLabel* m_couponSideLabel;
    QLineEdit* m_seniorityEdit;
    QLineEdit* m_seniorityLevelEdit;
    QLineEdit* m_sizeEdit;
    QLineEdit* m_couponEdit;
    QDoubleSpinBox* m_multiplierSpin;
    QComboBox* m_senioritySideCombo;
    QComboBox* m_sizeSideCombo;
    QComboBox* m_couponSideCombo;
};

#endif // DEFERREDINTERESTSETTINGWIDGET_H
