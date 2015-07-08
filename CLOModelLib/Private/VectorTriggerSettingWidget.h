#ifndef VECTORTRIGGERSETTINGWIDGET_H
#define VECTORTRIGGERSETTINGWIDGET_H

#include "Private/AbstractTriggerSettingWidget.h"
class QLabel;
class QLineEdit;
class VectorTriggerSettingWidget : public AbstractTriggerSettingWidget
{
    Q_OBJECT

public:
    VectorTriggerSettingWidget(QWidget *parent = nullptr);
    virtual void setParameters(const QString& pars) override;
    virtual QString parameters() const override;
public slots:
    virtual void reset() override;
private:
    QLabel* m_VectorLabel;
    QLineEdit* m_vectorEdit;
};

#endif // VECTORTRIGGERSETTINGWIDGET_H
