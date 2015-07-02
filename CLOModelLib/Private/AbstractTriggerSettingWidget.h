
#ifndef AbstractTriggerSettingWidget_h__
#define AbstractTriggerSettingWidget_h__
#include <QWidget>
class TriggerHelperDialog;
class AbstractTriggerSettingWidget :public QWidget
{
    Q_OBJECT
public:
    AbstractTriggerSettingWidget(QWidget* parent=nullptr);
    virtual void setParameters(const QString& pars);
    virtual QString parameters() const;
public slots:
    virtual void reset();
signals:
    void somethingChanged();
protected:
    enum : char {ParsSeparator='#'};
    friend TriggerHelperDialog;
};
#endif // AbstractTriggerSettingWidget_h__

