#include "AbstractTriggerSettingWidget.h"
AbstractTriggerSettingWidget::AbstractTriggerSettingWidget(QWidget* parent)
    :QWidget(parent)
{}

void AbstractTriggerSettingWidget::setParameters(const QString& pars)
{}

QString AbstractTriggerSettingWidget::parameters() const
{
    return QString();
}

void AbstractTriggerSettingWidget::reset()
{}

