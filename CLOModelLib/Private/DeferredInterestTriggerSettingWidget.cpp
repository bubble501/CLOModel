#include "Private/DeferredInterestTriggerSettingWidget.h"
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include "BloombergVector.h"
#include "IntegerVector.h"
#include "DeferredInterestTrigger.h"
#include <QGridLayout>
DeferredInterestTriggerSettingWidget::DeferredInterestTriggerSettingWidget(QWidget *parent)
    : AbstractTriggerSettingWidget(parent)
{
    m_seniorityLabel = new QLabel(this);
    m_seniorityLabel->setText(tr("Seniority group that represent the target of the trigger"));
    m_seniorityLevelLabel = new QLabel(this);
    m_seniorityLevelLabel->setText(tr("Level of seniority the seniority group refers to"));
    m_sizeLabel = new QLabel(this);
    m_sizeLabel->setText(tr("Target size of the Deferred Interest"));
    m_couponLabel = new QLabel(this);
    m_couponLabel->setText(tr("Target Coupon that is deferring interest"));
    m_multiplierLabel = new QLabel(this);
    m_multiplierLabel->setText(tr("Multiplier that will be applied to the target size"));
    m_senioritySideLabel = new QLabel(this);
    m_senioritySideLabel->setText(tr("Side of the seniority ladder the trigger applies too"));
    m_sizeSideLabel = new QLabel(this);
    m_sizeSideLabel->setText(tr("Side of the size where the trigger passes"));
    m_couponSideLabel = new QLabel(this);
    m_couponSideLabel->setText(tr("Side of the coupon that the trigger should consider"));

    m_seniorityEdit = new QLineEdit(this);
    m_seniorityEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
    m_seniorityEdit->setValidator(IntegerVector().GetValidator(this));
    m_seniorityLevelEdit = new QLineEdit(this);
    m_seniorityLevelEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
    m_seniorityLevelEdit->setValidator(IntegerVector().GetValidator(this));
    m_sizeEdit = new QLineEdit(this);
    m_sizeEdit->setValidator(BloombergVector().GetValidator(this));
    m_couponEdit = new QLineEdit(this);
    m_couponEdit->setValidator(IntegerVector().GetValidator(this));

    m_multiplierSpin = new QDoubleSpinBox(this);
    m_multiplierSpin->setSingleStep(0.5);
    m_multiplierSpin->setValue(0.0);

    m_senioritySideCombo = new QComboBox(this);
    m_senioritySideCombo->addItem(tr("Senior"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Senior));
    m_senioritySideCombo->addItem(tr("Junior"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Junior));
    m_senioritySideCombo->addItem(tr("Exactly"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Exactly));
    m_senioritySideCombo->addItem(tr("Senior or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::SeniorOrEqual));
    m_senioritySideCombo->addItem(tr("Junior or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::JuniorOrEqual));
    m_sizeSideCombo = new QComboBox(this);
    m_sizeSideCombo->addItem(tr("Bigger"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Bigger));
    m_sizeSideCombo->addItem(tr("Smaller"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Smaller));
    m_sizeSideCombo->addItem(tr("Exactly"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Exactly));
    m_sizeSideCombo->addItem(tr("Bigger or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::BiggerOrEqual));
    m_sizeSideCombo->addItem(tr("Smaller or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::SmallerOrEqual));
    m_couponSideCombo = new QComboBox(this);
    m_couponSideCombo->addItem(tr("Bigger"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Bigger));
    m_couponSideCombo->addItem(tr("Smaller"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Smaller));
    m_couponSideCombo->addItem(tr("Exactly"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Exactly));
    m_couponSideCombo->addItem(tr("Bigger or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::BiggerOrEqual));
    m_couponSideCombo->addItem(tr("Smaller or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::SmallerOrEqual));

    connect(m_seniorityEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_seniorityLevelEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_sizeEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_couponEdit, &QLineEdit::textChanged, this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_multiplierSpin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_couponSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_sizeSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AbstractTriggerSettingWidget::somethingChanged);
    connect(m_senioritySideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AbstractTriggerSettingWidget::somethingChanged);

    QGridLayout* mainLay = new QGridLayout(this);
    mainLay->addWidget(m_seniorityLabel, 0, 0);
    mainLay->addWidget(m_seniorityEdit, 0, 1);
    mainLay->addWidget(m_seniorityLevelLabel, 1, 0);
    mainLay->addWidget(m_seniorityLevelEdit, 1, 1);
    mainLay->addWidget(m_sizeLabel, 2, 0);
    mainLay->addWidget(m_sizeEdit, 2, 1);
    mainLay->addWidget(m_couponLabel, 3, 0);
    mainLay->addWidget(m_couponEdit, 3, 1);
    mainLay->addWidget(m_multiplierSpin, 4, 1);
    mainLay->addWidget(m_multiplierLabel, 4, 0);
    mainLay->addWidget(m_senioritySideCombo, 5, 1);
    mainLay->addWidget(m_senioritySideLabel, 5, 0);
    mainLay->addWidget(m_sizeSideCombo, 6, 1);
    mainLay->addWidget(m_sizeSideLabel, 6, 0);
    mainLay->addWidget(m_couponSideCombo, 7, 1);
    mainLay->addWidget(m_couponSideLabel, 7, 0);
    mainLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 8, 0, 1, 2);
}

void DeferredInterestTriggerSettingWidget::setParameters(const QString& pars)
{
    const QStringList SignlePars = pars.split(ParsSeparator, QString::KeepEmptyParts);
    if (SignlePars.count() < 8)
        return reset();
    bool checkDouble;
    SignlePars.at(4).toDouble(&checkDouble); //Multiplier
    if (
        IntegerVector(SignlePars.at(0)).IsEmpty() //Seniority
        || IntegerVector(SignlePars.at(1)).IsEmpty() //Seniority Level
        || BloombergVector(SignlePars.at(2)).IsEmpty() //Size
        || IntegerVector(SignlePars.at(3)).IsEmpty() // Coupon
        || !checkDouble
        || m_senioritySideCombo->findData(static_cast<quint8>(SignlePars.at(5).toInt())) < 0
        || m_sizeSideCombo->findData(static_cast<quint8>(SignlePars.at(6).toInt())) < 0
        || m_couponSideCombo->findData(static_cast<quint8>(SignlePars.at(7).toInt())) < 0
        )
        return reset();
    m_seniorityEdit->setText(SignlePars.at(0));
    m_seniorityLevelEdit->setText(SignlePars.at(1));
    m_sizeEdit->setText(SignlePars.at(2));
    m_senioritySideCombo->setCurrentIndex(m_senioritySideCombo->findData(static_cast<quint8>(SignlePars.at(3).toInt())));
    m_sizeSideCombo->setCurrentIndex(m_sizeSideCombo->findData(static_cast<quint8>(SignlePars.at(4).toInt())));
    m_multiplierSpin->setValue(SignlePars.at(5).toDouble());
    m_couponEdit->setText(SignlePars.at(6));
    m_couponSideCombo->setCurrentIndex(m_couponSideCombo->findData(static_cast<quint8>(SignlePars.at(7).toInt())));
}

QString DeferredInterestTriggerSettingWidget::parameters() const
{
    if (
        IntegerVector(m_seniorityEdit->text()).IsEmpty() //Seniority
        || IntegerVector(m_seniorityLevelEdit->text()).IsEmpty() //Seniority Level
        || BloombergVector(m_sizeEdit->text()).IsEmpty() //Size
        || IntegerVector(m_couponEdit->text()).IsEmpty() // Coupon
        )
        return QString();
    return
        m_seniorityEdit->text() + ParsSeparator
        + m_seniorityLevelEdit->text() + ParsSeparator
        + m_sizeEdit->text() + ParsSeparator
        + QString::number(static_cast<quint8>(m_senioritySideCombo->currentData().toInt())) + ParsSeparator
        + QString::number(static_cast<quint8>(m_sizeSideCombo->currentData().toInt())) + ParsSeparator
        + QString::number(m_multiplierSpin->value(), 'f') + ParsSeparator
        + m_couponEdit->text() + ParsSeparator
        + QString::number(static_cast<quint8>(m_couponSideCombo->currentData().toInt()))
        ;
}

void DeferredInterestTriggerSettingWidget::reset()
{
    m_couponSideCombo->setCurrentIndex(0);
    m_sizeSideCombo->setCurrentIndex(0);
    m_senioritySideCombo->setCurrentIndex(0);
    m_seniorityEdit->clear();
    m_seniorityLevelEdit->clear();
    m_sizeEdit->clear();
    m_couponEdit->clear();
    m_multiplierSpin->setValue(0.0);
}
