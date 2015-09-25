#include "TriggerHelperDialog.h"
#include "Private/TriggerHelperDialog_p.h"
#include <QComboBox>
#include <QStackedWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include "DateTrigger.h"
#include "PoolSizeTrigger.h"
#include "CumulativeLossTrigger.h"
#include "TrancheTrigger.h"
#include "PDLtrigger.h"
#include "VectorTrigger.h"
#include "DelinquencyTrigger.h"
#include "DuringStressTestTrigger.h"
#include "DeferredInterestTrigger.h"
#include "Private/DateTriggerSettingWidget.h"
#include "Private/VectorTriggerSettingWidget.h"
#include "Private/PDLTriggerSettingWidget.h"
#include "Private/PoolSizeTriggerSettingWidget.h"
#include "Private/TrancheTriggerSettingWidget.h"
#include "Private/DelinquencyTriggerSettingWidget.h"
#include "Private/CumulativeLossTriggerSettingWidget.h"
#include "Private/DeferredInterestTriggerSettingWidget.h"
TriggerHelperDialogPrivate::~TriggerHelperDialogPrivate() {}
TriggerHelperDialogPrivate::TriggerHelperDialogPrivate(TriggerHelperDialog *q)
    :q_ptr(q)
    , FirstCombodeleted(false)
    , Cleared(false)
{}
TriggerHelperDialog::TriggerHelperDialog( QWidget *parent)
    : TriggerHelperDialog(new TriggerHelperDialogPrivate(this), parent)
{}
TriggerHelperDialog::TriggerHelperDialog(TriggerHelperDialogPrivate* d, QWidget *parent)
	: QDialog(parent)
    , d_ptr(d)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("Edit trigger");
	QLabel *TriggerSelectLabel = new QLabel(this);
	TriggerSelectLabel->setText(tr("Select Trigger Type"));
	d->TriggerTypeCombo = new QComboBox(this);
    d->TriggerTypeCombo->addItem("Date", static_cast<quint8>(AbstractTrigger::TriggerType::DateTrigger));
    d->TriggerTypeCombo->addItem("Vector", static_cast<quint8>(AbstractTrigger::TriggerType::VectorTrigger));
    d->TriggerTypeCombo->addItem("Pool Size", static_cast<quint8>(AbstractTrigger::TriggerType::PoolSizeTrigger));
    d->TriggerTypeCombo->addItem("Tranche", static_cast<quint8>(AbstractTrigger::TriggerType::TrancheTrigger));
    d->TriggerTypeCombo->addItem("Delinquencies", static_cast<quint8>(AbstractTrigger::TriggerType::DelinquencyTrigger));
    d->TriggerTypeCombo->addItem("During Stress Test", static_cast<quint8>(AbstractTrigger::TriggerType::DuringStressTestTrigger));
    d->TriggerTypeCombo->addItem("Cumulative Loss Trigger", static_cast<quint8>(AbstractTrigger::TriggerType::CumulativeLossTrigger));
    d->TriggerTypeCombo->addItem("Deferred Interest Trigger", static_cast<quint8>(AbstractTrigger::TriggerType::DeferredInterestTrigger));
    d->TriggerTypeCombo->addItem("PDL Trigger", static_cast<quint8>(AbstractTrigger::TriggerType::PDLTrigger));
    d->TriggerTypeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	
	QLabel *TriggerLabelLabel = new QLabel(this);
	TriggerLabelLabel->setText(tr("Insert Trigger Label"));
    d->TriggerLabelEdit = new QLineEdit(this);
    d->TriggerLabelEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".+"), d->TriggerLabelEdit));
    d->TriggerLabelEdit->setText("Trigger");
    d->TriggerLabelEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    d->TriggerBuilderBase = new QStackedWidget(this);
    d->TriggerBuilderBase->addWidget(DateTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->addWidget(VectorTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->addWidget(PoolSizeTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->addWidget(TrancheTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->addWidget(DelinquencyTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->addWidget(DuringStressTestTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->addWidget(CumulativeLossTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->addWidget(DeferredInterestTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->addWidget(PDLTrigger::createSettingsWidget(this));
    d->TriggerBuilderBase->setMinimumSize(200, 200);
    d->TriggerBuilderBase->setStyleSheet("QLabel { qproperty-alignment: 'AlignRight | AlignVCenter'; }");


    d->DialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset, this);
    d->DialogButtons->button(QDialogButtonBox::Ok)->setEnabled(false);

    for (int i = d->TriggerBuilderBase->count() - 1; i >= 0; --i) {
        const auto CurrentWidg= qobject_cast<AbstractTriggerSettingWidget*>(d->TriggerBuilderBase->widget(i));
        connect(CurrentWidg, &AbstractTriggerSettingWidget::somethingChanged, this, &TriggerHelperDialog::CheckOkEnabled);
        connect(d->TriggerBuilderBase, &QStackedWidget::currentChanged, CurrentWidg, &AbstractTriggerSettingWidget::reset);
    }
    connect(d->TriggerTypeCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), d->TriggerBuilderBase, &QStackedWidget::setCurrentIndex);
    connect(d->DialogButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(d->DialogButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(d->DialogButtons->button(QDialogButtonBox::Reset), &QPushButton::clicked, [this]() ->void {done(QDialogButtonBox::Reset); });

	QGroupBox *TopGroup = new QGroupBox(this);
	TopGroup->setTitle("Trigger");
	QGroupBox *ParamGroup = new QGroupBox(this);
	ParamGroup->setTitle("Parameters");
	ParamGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	QHBoxLayout* ParamLay = new QHBoxLayout(ParamGroup);
    ParamLay->addWidget(d->TriggerBuilderBase);
	QGridLayout* TopLay = new QGridLayout(TopGroup);
    TopLay->addWidget(TriggerSelectLabel, 0, 0);
    TopLay->addWidget(d->TriggerTypeCombo, 0, 1);
    TopLay->addWidget(TriggerLabelLabel, 1, 0);
    TopLay->addWidget(d->TriggerLabelEdit, 1, 1);

	QVBoxLayout* mainlay = new QVBoxLayout(this);
	mainlay->addWidget(TopGroup);
	mainlay->addWidget(ParamGroup);
    mainlay->addWidget(d->DialogButtons);
}

TriggerHelperDialog::~TriggerHelperDialog()
{
    delete d_ptr;
}

void TriggerHelperDialog::CheckOkEnabled()
{
    Q_D(TriggerHelperDialog);
    d->DialogButtons->button(QDialogButtonBox::Ok)->setEnabled(!qobject_cast<AbstractTriggerSettingWidget*>(d->TriggerBuilderBase->currentWidget())->parameters().isEmpty());
}

QString TriggerHelperDialog::GetParameters() const
{
    Q_D(const TriggerHelperDialog);
    return
        QString::number(d->TriggerTypeCombo->currentData().toInt()) + AbstractTriggerSettingWidget::ParsSeparator +
        d->TriggerLabelEdit->text() + AbstractTriggerSettingWidget::ParsSeparator +
        qobject_cast<AbstractTriggerSettingWidget*>(d->TriggerBuilderBase->currentWidget())->parameters();
}

void TriggerHelperDialog::ClearParameters()
{
    Q_D(TriggerHelperDialog);
    for (int i = d->TriggerBuilderBase->count() - 1; i >= 0; --i)
        qobject_cast<AbstractTriggerSettingWidget*>(d->TriggerBuilderBase->widget(i))->reset();
}

void TriggerHelperDialog::SetCurrentPars(const QString& pars)
{
    Q_D(TriggerHelperDialog);
    if (pars.isEmpty()) 
        return ClearParameters();
    auto AllPars = pars.split(AbstractTriggerSettingWidget::ParsSeparator);
    d->TriggerTypeCombo->setCurrentIndex(qMax(0,d->TriggerTypeCombo->findData(AllPars.takeFirst().toInt())));
    if (!AllPars.isEmpty()) {
        if (AllPars.first().isEmpty()) 
            d->TriggerLabelEdit->setText(tr("Trigger"));
        else 
            d->TriggerLabelEdit->setText(AllPars.first());
        AllPars.removeFirst();
        if (!AllPars.isEmpty()) 
            qobject_cast<AbstractTriggerSettingWidget*>(d->TriggerBuilderBase->currentWidget())->setParameters(AllPars.join(AbstractTriggerSettingWidget::ParsSeparator));
    }
    else {
        d->TriggerLabelEdit->setText(tr("Trigger"));
    }
}
