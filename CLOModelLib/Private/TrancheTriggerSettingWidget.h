#ifndef TRANCHETRIGGERSETTINGWIDGET_H
#define TRANCHETRIGGERSETTINGWIDGET_H

#include "Private/AbstractTriggerSettingWidget.h"
class QLabel;
class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class TrancheTriggerSettingWidget : public AbstractTriggerSettingWidget
{
    Q_OBJECT
public:
    TrancheTriggerSettingWidget(QWidget *parent = nullptr);
    virtual void setParameters(const QString& pars) override;
    virtual QString parameters() const override;
    public slots:
    virtual void reset() override;
private:
    QLabel* m_seniorityLabel;
    QLabel* m_seniorityLevelLabel;
    QLabel* m_sizeLabel;
    QLabel* m_multiplierLabel;
    QLabel* m_senioritySideLabel;
    QLabel* m_sizeSideLabel;
    QLineEdit* m_seniorityEdit;
    QLineEdit* m_seniorityLevelEdit;
    QLineEdit* m_sizeEdit;
    QDoubleSpinBox* m_multiplierSpin;
    QComboBox* m_senioritySideCombo;
    QComboBox* m_sizeSideCombo;
};

#endif // TRANCHETRIGGERSETTINGWIDGET_H
