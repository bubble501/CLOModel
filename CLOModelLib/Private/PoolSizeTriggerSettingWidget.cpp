#include "Private/PoolSizeTriggerSettingWidget.h"
#include <QLabel>
#include <QLineEdit>
#include "BloombergVector.h"
#include <QRegExpValidator>
#include <QGridLayout>
#include <QStringList>
#include <QComboBox>
#include "PoolSizeTrigger.h"
PoolSizeTriggerSettingWidget::PoolSizeTriggerSettingWidget(QWidget *parent)
    : AbstractTriggerSettingWidget(parent)
{
    m_VectorLabel = new QLabel(this);
    m_VectorLabel->setText(tr("Size of the pool that the trigger targets"));
    m_SideLabel = new QLabel(this);
    m_SideLabel->setText(tr("Side of the pool size where the trigger passes"));
    m_vectorEdit = new QLineEdit(this);
    m_vectorEdit->setToolTip(tr("Accepts Vectors"));
    m_vectorEdit->setValidator(BloombergVector().GetValidator(this));
    m_sideCombo = new QComboBox(this);
    m_sideCombo->addItem(tr("Bigger"), static_cast<quint8>(PoolSizeTrigger::TriggerSide::Bigger));
    m_sideCombo->addItem(tr("Smaller"), static_cast<quint8>(PoolSizeTrigger::TriggerSide::Smaller));
    m_sideCombo->addItem(tr("Exactly"), static_cast<quint8>(PoolSizeTrigger::TriggerSide::Exactly));
    m_sideCombo->addItem(tr("Bigger or Equal"), static_cast<quint8>(PoolSizeTrigger::TriggerSide::BiggerOrEqual));
    m_sideCombo->addItem(tr("Smaller or Equal"), static_cast<quint8>(PoolSizeTrigger::TriggerSide::SmallerOrEqual));

    connect(m_vectorEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_sideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AbstractTriggerSettingWidget::somethingChanged);

    QGridLayout* mainLay = new QGridLayout(this);
    mainLay->addWidget(m_VectorLabel, 0, 0);
    mainLay->addWidget(m_vectorEdit, 0, 1);
    mainLay->addWidget(m_SideLabel, 1, 0);
    mainLay->addWidget(m_sideCombo, 1, 1);
    mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 2, 0, 1, 2);
}

void PoolSizeTriggerSettingWidget::setParameters(const QString& pars)
{
    const QStringList SignlePars = pars.split(ParsSeparator, QString::KeepEmptyParts);
    if (SignlePars.count() < 2)
        return reset();
    if (
        BloombergVector(SignlePars.at(0)).IsEmpty()
        || m_sideCombo->findData(static_cast<quint8>(SignlePars.at(1).toInt())) < 0
        )
        return reset();
    m_vectorEdit->setText(SignlePars.at(0));
    m_sideCombo->setCurrentIndex(m_sideCombo->findData(static_cast<quint8>(SignlePars.at(1).toInt())));
}

QString PoolSizeTriggerSettingWidget::parameters() const
{
    if (BloombergVector(m_vectorEdit->text()).IsEmpty())
        return QString();
    return
        m_vectorEdit->text()
        + ParsSeparator
        + QString::number(static_cast<quint8>(m_sideCombo->currentData().toInt()))
        ;
}

void PoolSizeTriggerSettingWidget::reset()
{
    m_vectorEdit->clear();
    m_sideCombo->setCurrentIndex(0);
}

