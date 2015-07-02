#include "DateTriggerSettingWidget.h"
#include <QLabel>
#include <QDateEdit>
#include <QComboBox>
#include "DateTrigger.h"
#include <QGridLayout>
#include <QStringList>
DateTriggerSettingWidget::DateTriggerSettingWidget(QWidget *parent)
    : AbstractTriggerSettingWidget(parent)
{
    m_dateLabel = new QLabel(this);
    m_dateLabel->setText(tr("Threshold date for the trigger"));
    m_sideLabel = new QLabel(this);
    m_sideLabel->setText(tr("Side of the date where the trigger passes"));
    m_dateEditor = new QDateEdit(this);
    m_dateEditor->setDate(QDate::currentDate());
    m_dateEditor->setCalendarPopup(true);
    m_sideCombo = new QComboBox(this);
    m_sideCombo->addItem(tr("Before Excluding"), static_cast<quint8>(DateTrigger::TriggerSide::BeforeExcluding));
    m_sideCombo->addItem(tr("After Excluding"), static_cast<quint8>(DateTrigger::TriggerSide::AfterExcluding));
    m_sideCombo->addItem(tr("Exactly"), static_cast<quint8>(DateTrigger::TriggerSide::Exactly));
    m_sideCombo->addItem(tr("Before Including"), static_cast<quint8>(DateTrigger::TriggerSide::BeforeIncluding));
    m_sideCombo->addItem(tr("After Including"), static_cast<quint8>(DateTrigger::TriggerSide::AfterIncluding));

    connect(m_dateEditor, &QDateEdit::dateChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_sideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AbstractTriggerSettingWidget::somethingChanged);

    QGridLayout* mainLay = new QGridLayout(this);
    mainLay->addWidget(m_dateLabel, 0, 0);
    mainLay->addWidget(m_dateEditor, 0, 1);
    mainLay->addWidget(m_sideLabel, 1, 0);
    mainLay->addWidget(m_sideCombo, 1, 1);
    mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 2, 0, 1, 2);
}

void DateTriggerSettingWidget::setParameters(const QString& pars)
{
    if (pars.isEmpty())
        return reset();
    const QStringList SignlePars = pars.split(ParsSeparator,QString::KeepEmptyParts);
    if(SignlePars.count()<2)
        return reset();
    if (
        QDate::fromString(SignlePars.at(0), Qt::ISODate).isNull() 
        || m_sideCombo->findData(static_cast<quint8>(SignlePars.at(1).toInt())) < 0
        )
        return reset();
    m_dateEditor->setDate(QDate::fromString(SignlePars.at(0), Qt::ISODate));
    m_sideCombo->setCurrentIndex(m_sideCombo->findData(static_cast<quint8>(SignlePars.at(1).toInt())));
}

QString DateTriggerSettingWidget::parameters() const
{
    if (m_dateEditor->date().isNull())
        return QString();
    return
        m_dateEditor->date().toString(Qt::ISODate) 
        + ParsSeparator
        + QString::number(static_cast<quint8>(m_sideCombo->currentData().toInt()))
        ;
}

void DateTriggerSettingWidget::reset()
{
    m_dateEditor->setDate(QDate::currentDate());
    m_sideCombo->setCurrentIndex(0);
}

