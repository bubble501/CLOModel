#include "Private/TrancheTriggerSettingWidget.h"
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include "BloombergVector.h"
#include "IntegerVector.h"
#include "TrancheTrigger.h"
#include <QGridLayout>
TrancheTriggerSettingWidget::TrancheTriggerSettingWidget(QWidget *parent)
    : AbstractTriggerSettingWidget(parent)
{
    m_seniorityLabel = new QLabel(this);
    m_seniorityLabel->setText(tr("Seniority group that represent the target of the trigger"));
    m_seniorityLevelLabel = new QLabel(this);
    m_seniorityLevelLabel->setText(tr("Level of seniority the seniority group refers to"));
    m_sizeLabel = new QLabel(this);
    m_sizeLabel->setText(tr("Target size of the pro-rata group"));
    m_multiplierLabel = new QLabel(this);
    m_multiplierLabel->setText(tr("Multiplier that will be applied to the target size"));
    m_senioritySideLabel = new QLabel(this);
    m_senioritySideLabel->setText(tr("Side of the group size where the trigger passes"));
    m_sizeSideLabel = new QLabel(this);
    m_sizeSideLabel->setText(tr("Side of the size where the trigger passes"));

    m_seniorityEdit = new QLineEdit(this);
    m_seniorityEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
    m_seniorityEdit->setValidator(IntegerVector().GetValidator(this));
    m_seniorityLevelEdit = new QLineEdit(this);
    m_seniorityLevelEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
    m_seniorityLevelEdit->setValidator(IntegerVector().GetValidator(this));
    m_sizeEdit = new QLineEdit(this);
    m_sizeEdit->setValidator(BloombergVector().GetValidator(this));

    m_multiplierSpin = new QDoubleSpinBox(this);
    m_multiplierSpin->setSingleStep(0.5);
    m_multiplierSpin->setValue(0.0);

    m_senioritySideCombo = new QComboBox(this);
    m_senioritySideCombo->addItem(tr("Senior"), static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Senior));
    m_senioritySideCombo->addItem(tr("Junior"), static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Junior));
    m_senioritySideCombo->addItem(tr("Exactly"), static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Exactly));
    m_senioritySideCombo->addItem(tr("Senior or Equal"), static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::SeniorOrEqual));
    m_senioritySideCombo->addItem(tr("Junior or Equal"), static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::JuniorOrEqual));
    m_sizeSideCombo = new QComboBox(this);
    m_sizeSideCombo->addItem(tr("Bigger"), static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Bigger));
    m_sizeSideCombo->addItem(tr("Smaller"), static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Smaller));
    m_sizeSideCombo->addItem(tr("Exactly"), static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Exactly));
    m_sizeSideCombo->addItem(tr("Bigger or Equal"), static_cast<quint8>(TrancheTrigger::TriggerSizeSide::BiggerOrEqual));
    m_sizeSideCombo->addItem(tr("Smaller or Equal"), static_cast<quint8>(TrancheTrigger::TriggerSizeSide::SmallerOrEqual));

    connect(m_seniorityEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_seniorityLevelEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_sizeEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_multiplierSpin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_sizeSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_senioritySideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AbstractTriggerSettingWidget::somethingChanged);

    QGridLayout* mainLay = new QGridLayout(this);
    mainLay->addWidget(m_seniorityLabel, 0, 0);
    mainLay->addWidget(m_seniorityEdit, 0, 1);
    mainLay->addWidget(m_seniorityLevelLabel, 1, 0);
    mainLay->addWidget(m_seniorityLevelEdit, 1, 1);
    mainLay->addWidget(m_sizeLabel, 2, 0);
    mainLay->addWidget(m_sizeEdit, 2, 1);
    mainLay->addWidget(m_multiplierSpin, 3, 1);
    mainLay->addWidget(m_multiplierLabel, 3, 0);
    mainLay->addWidget(m_senioritySideCombo, 4, 1);
    mainLay->addWidget(m_senioritySideLabel, 4, 0);
    mainLay->addWidget(m_sizeSideCombo, 5, 1);
    mainLay->addWidget(m_sizeSideLabel, 5, 0);
    mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 6, 0, 1, 2);
}


void TrancheTriggerSettingWidget::setParameters(const QString& pars)
{
    const QStringList SignlePars = pars.split(ParsSeparator, QString::KeepEmptyParts);
    if (SignlePars.count() < 6)
        return reset();
    bool checkDouble;
    SignlePars.at(3).toDouble(&checkDouble); //Multiplier
    if (
        IntegerVector(SignlePars.at(0)).IsEmpty() //Seniority
        || IntegerVector(SignlePars.at(1)).IsEmpty() //Seniority Level
        || BloombergVector(SignlePars.at(2)).IsEmpty() //Size
        || !checkDouble
        || m_senioritySideCombo->findData(static_cast<quint8>(SignlePars.at(4).toInt())) < 0
        || m_sizeSideCombo->findData(static_cast<quint8>(SignlePars.at(5).toInt())) < 0
        )
        return reset();
    m_seniorityEdit->setText(SignlePars.at(0));
    m_seniorityLevelEdit->setText(SignlePars.at(1));
    m_sizeEdit->setText(SignlePars.at(2));
    m_senioritySideCombo->setCurrentIndex(m_senioritySideCombo->findData(static_cast<quint8>(SignlePars.at(3).toInt())));
    m_sizeSideCombo->setCurrentIndex(m_sizeSideCombo->findData(static_cast<quint8>(SignlePars.at(4).toInt())));
    m_multiplierSpin->setValue(SignlePars.at(5).toDouble());
}

QString TrancheTriggerSettingWidget::parameters() const
{
    if (
        IntegerVector(m_seniorityEdit->text()).IsEmpty() //Seniority
        || IntegerVector(m_seniorityLevelEdit->text()).IsEmpty() //Seniority Level
        || BloombergVector(m_sizeEdit->text()).IsEmpty() //Size
        )
        return QString();
    return
        m_seniorityEdit->text() + ParsSeparator
        + m_seniorityLevelEdit->text() + ParsSeparator
        + m_sizeEdit->text() + ParsSeparator
        + QString::number(static_cast<quint8>(m_senioritySideCombo->currentData().toInt())) + ParsSeparator
        + QString::number(static_cast<quint8>(m_sizeSideCombo->currentData().toInt())) + ParsSeparator
        + QString::number(m_multiplierSpin->value(), 'f') 
        ;
}

void TrancheTriggerSettingWidget::reset()
{
    m_sizeSideCombo->setCurrentIndex(0);
    m_senioritySideCombo->setCurrentIndex(0);
    m_seniorityEdit->clear();
    m_seniorityLevelEdit->clear();
    m_sizeEdit->clear();
    m_multiplierSpin->setValue(0.0);
}
