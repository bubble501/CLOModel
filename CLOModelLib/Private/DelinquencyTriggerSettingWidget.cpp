#include "Private/DelinquencyTriggerSettingWidget.h"
#include <QLabel>
#include <QLineEdit>
#include "BloombergVector.h"
#include <QRegularExpressionValidator>
#include <QGridLayout>
#include <QStringList>
DelinquencyTriggerSettingWidget::DelinquencyTriggerSettingWidget(QWidget *parent)
    : AbstractTriggerSettingWidget(parent)
{
    m_VectorLabel = new QLabel(this);
    m_VectorLabel->setText(tr("Percentage of the pool in arrears"));
    m_vectorEdit = new QLineEdit(this);
    m_vectorEdit->setToolTip(tr("Accepts vectors. The trigger will pass if the delinquency are less than this target value."));
    m_vectorEdit->setValidator(BloombergVector().GetValidator(this));

    connect(m_vectorEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);

    QGridLayout* mainLay = new QGridLayout(this);
    mainLay->addWidget(m_VectorLabel, 0, 0);
    mainLay->addWidget(m_vectorEdit, 0, 1);
    mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 1, 0, 1, 2);
}

void DelinquencyTriggerSettingWidget::setParameters(const QString& pars)
{
    const QStringList SignlePars = pars.split(ParsSeparator, QString::KeepEmptyParts);
    if (SignlePars.count() < 1)
        return reset();
    if(BloombergVector(SignlePars.at(0)).IsEmpty())
        return reset();
    m_vectorEdit->setText(SignlePars.at(0));
}

QString DelinquencyTriggerSettingWidget::parameters() const
{
    if (BloombergVector(m_vectorEdit->text()).IsEmpty())
        return QString();
    return m_vectorEdit->text();
}

void DelinquencyTriggerSettingWidget::reset()
{
    m_vectorEdit->clear();
}

