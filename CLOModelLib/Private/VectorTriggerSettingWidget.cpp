#include "Private/VectorTriggerSettingWidget.h"
#include <QLabel>
#include <QLineEdit>
#include "BoolVector.h"
#include <QRegularExpressionValidator>
#include <QGridLayout>
#include <QStringList>
VectorTriggerSettingWidget::VectorTriggerSettingWidget(QWidget *parent)
    : AbstractTriggerSettingWidget(parent)
{
    m_VectorLabel = new QLabel(this);
    m_VectorLabel->setText(tr("Vector that determines if the trigger is passing"));
    m_vectorEdit = new QLineEdit(this);
    m_vectorEdit->setToolTip(tr("Accepted values are T or Y for passing and F or N for failing"));
    m_vectorEdit->setValidator(BoolVector().GetValidator(this));

    connect(m_vectorEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);

    QGridLayout* mainLay = new QGridLayout(this);
    mainLay->addWidget(m_VectorLabel, 0, 0);
    mainLay->addWidget(m_vectorEdit, 0, 1);
    mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 1, 0, 1, 2);
}

void VectorTriggerSettingWidget::setParameters(const QString& pars)
{
    const QStringList SignlePars = pars.split(ParsSeparator, QString::KeepEmptyParts);
    if (SignlePars.count() < 1)
        return reset();
    if(BoolVector(SignlePars.at(0)).IsEmpty())
        return reset();
    m_vectorEdit->setText(SignlePars.at(0));
}

QString VectorTriggerSettingWidget::parameters() const
{
    if (BoolVector(m_vectorEdit->text()).IsEmpty())
        return QString();
    return m_vectorEdit->text();
}

void VectorTriggerSettingWidget::reset()
{
    m_vectorEdit->clear();
}

