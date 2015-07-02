#ifndef DATETRIGGERSETTINGWIDGET_H
#define DATETRIGGERSETTINGWIDGET_H

#include "Private/AbstractTriggerSettingWidget.h"
class QLabel;
class QComboBox;
class QDateEdit;
class DateTriggerSettingWidget : public AbstractTriggerSettingWidget
{
    Q_OBJECT
public:
    DateTriggerSettingWidget(QWidget *parent=nullptr);
    virtual void setParameters(const QString& pars) override;
    virtual QString parameters() const override;
public slots:
    virtual void reset() override;
private:
    QLabel* m_dateLabel;
    QLabel* m_sideLabel;
    QDateEdit* m_dateEditor;
    QComboBox* m_sideCombo;
};

#endif // DATETRIGGERSETTINGWIDGET_H
