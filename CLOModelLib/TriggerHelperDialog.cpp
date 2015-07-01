#include "TriggerHelperDialog.h"
#include "Private/TriggerHelperDialog_p.h"
#include <QComboBox>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include "IntegerVector.h"
#include "BoolVector.h"
#include "AbstractTrigger.h"
#include "BloombergVector.h"
#include "DateTrigger.h"
#include "PoolSizeTrigger.h"
#include "CumulativeLossTrigger.h"
#include "TrancheTrigger.h"
#include "PDLtrigger.h"
#include "DeferredInterestTrigger.h"

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
    d->TriggerTypeCombo->addItem("", -1);
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
    d->TriggerLabelEdit->setValidator(new QRegExpValidator(QRegExp(".+"), d->TriggerLabelEdit));
    d->TriggerLabelEdit->setText("Trigger");
    d->TriggerLabelEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    d->TriggerBuilderBase = new QStackedWidget(this);
    connect(d->TriggerTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ClearParameters()));
    connect(d->TriggerTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int)));
    d->TriggerBuilderBase->setMinimumSize(200, 200);
    d->TriggerBuilderBase->setStyleSheet("QLabel { qproperty-alignment: 'AlignRight | AlignVCenter'; }");
    d->TriggerBuilderBase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->TriggerBuilderBase->addWidget(new QWidget(this)); //No Trigger Selected
    d->TriggerBuilderBase->addWidget(d->CreateDateTriggerWidget());
    d->TriggerBuilderBase->addWidget(d->CreateVectorTriggerWidget());
    d->TriggerBuilderBase->addWidget(d->CreatePoolSizeTriggerWidget());
    d->TriggerBuilderBase->addWidget(d->CreateTrancheTriggerWidget());
    d->TriggerBuilderBase->addWidget(d->CreateDelinquencyTriggerWidget());
    d->TriggerBuilderBase->addWidget(new QWidget(d->TriggerBuilderBase)); //DuringStressTestTrigger
    d->TriggerBuilderBase->addWidget(d->CreateCumulativeLossTriggerWidget());
    d->TriggerBuilderBase->addWidget(d->CreateDeferredInterestTriggerWidget());
    d->TriggerBuilderBase->addWidget(d->CreatePDLWidget());

    d->AcceptButton = new QPushButton(this);
    d->AcceptButton->setText(tr("OK"));
    d->AcceptButton->setDefault(true);
    d->AcceptButton->setEnabled(false);
    connect(d->AcceptButton, &QPushButton::clicked, this, &QDialog::accept);
	QPushButton*  CancelButton = new QPushButton(this);
	CancelButton->setText(tr("Cancel"));
	CancelButton->setDefault(false);
	connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);
	QPushButton* ClearAcceptButton = new QPushButton(this);
	ClearAcceptButton->setText(tr("Clear"));
	ClearAcceptButton->setDefault(false);
    connect(ClearAcceptButton, &QPushButton::clicked, this, [&]() { d_func()->Cleared = true; accept(); });

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

	QHBoxLayout* ButtonsLay = new QHBoxLayout;
	ButtonsLay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    ButtonsLay->addWidget(d->AcceptButton);
	ButtonsLay->addWidget(ClearAcceptButton);
	ButtonsLay->addWidget(CancelButton);

	QVBoxLayout* mainlay = new QVBoxLayout(this);
	mainlay->addWidget(TopGroup);
	mainlay->addWidget(ParamGroup);
	mainlay->addLayout(ButtonsLay);
}

TriggerHelperDialog::~TriggerHelperDialog()
{
    delete d_ptr;
}

void TriggerHelperDialog::CheckOkEnabled(int index)
{
    Q_D(TriggerHelperDialog);
    if (d->FirstCombodeleted) return;
    d->AcceptButton->setEnabled(index != 0);
	if (index != 0) {
        disconnect(d->TriggerTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int)));
        QWidget* EmptyWidg = d->TriggerBuilderBase->widget(0);
        d->TriggerBuilderBase->removeWidget(EmptyWidg);
		EmptyWidg->deleteLater();
        d->FirstCombodeleted = true;
        d->TriggerTypeCombo->removeItem(0);
        d->TriggerBuilderBase->setCurrentIndex(d->TriggerTypeCombo->currentIndex());
        connect(d->TriggerTypeCombo, SIGNAL(currentIndexChanged(int)), d->TriggerBuilderBase, SLOT(setCurrentIndex(int)), Qt::QueuedConnection);
	}
}

void TriggerHelperDialog::SetParameter(int parIdx, const QString& parVal)
{
    Q_D(TriggerHelperDialog);
    if (parIdx<0 || (parVal.isEmpty() && parIdx >= d->ParamColumns.size())) 
        return;
    while (d->ParamColumns.size() <= parIdx)
        d->ParamColumns.append(QString());
    d->ParamColumns[parIdx] = parVal;
    while (d->ParamColumns.last().isEmpty()) {
        d->ParamColumns.removeLast();
        if (d->ParamColumns.isEmpty()) 
            return;
	}
}

QString TriggerHelperDialog::GetParameters() const
{
    Q_D(const TriggerHelperDialog);
    if (d->Cleared) return QString("#");
	return 
        QString::number(d->TriggerTypeCombo->currentData().toInt()) + '#' +
        d->TriggerLabelEdit->text() + '#' +
        d->ParamColumns.join('#')
	;
}

QWidget* TriggerHelperDialogPrivate::CreateDateTriggerWidget() {
    Q_Q(TriggerHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* DateLabel = new QLabel(Result);
	DateLabel->setText(q->tr("Threshold date for the trigger"));
	DateTrigger_dateEdit = new QDateEdit(Result);
	DateTrigger_dateEdit->setDate(QDate::currentDate());
	DateTrigger_dateEdit->setCalendarPopup(true);
    q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		DateTrigger_dateEdit->setDate(QDate::currentDate());
		if (TriggerBuilderBase->currentWidget()->children().contains(DateTrigger_dateEdit))
			q_func()->SetParameter(0, QDate::currentDate().toString("yyyy-MM-dd"));
	});
    q->connect(DateTrigger_dateEdit, &QDateEdit::dateChanged, [&](const QDate& NewDate) {
		if (TriggerBuilderBase->currentWidget()->children().contains(DateTrigger_dateEdit))
		{
            if (NewDate.isNull()) 
                q_func()->SetParameter(0, QString());
            else
                q_func()->SetParameter(0, NewDate.toString("yyyy-MM-dd"));
		}
	});
    q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) {
			if (parVal.isEmpty()) DateTrigger_dateEdit->setDate(QDate::currentDate());
			else DateTrigger_dateEdit->setDate(QDate::fromString(parVal, "yyyy-MM-dd"));
		}
	});
	ResLay->addWidget(DateLabel, CountRow, 0);
	ResLay->addWidget(DateTrigger_dateEdit, CountRow, 1);
	CountRow++;

	QLabel* SideLabel = new QLabel(Result);
    SideLabel->setText(q->tr("Side of the date where the trigger passes"));
	DateTrigger_SideCombo = new QComboBox(Result);
	DateTrigger_SideCombo->addItem("Before Excluding", static_cast<quint8>(DateTrigger::TriggerSide::BeforeExcluding));
	DateTrigger_SideCombo->addItem("After Excluding", static_cast<quint8>(DateTrigger::TriggerSide::AfterExcluding));
	DateTrigger_SideCombo->addItem("Exactly", static_cast<quint8>(DateTrigger::TriggerSide::Exactly));
	DateTrigger_SideCombo->addItem("Before Including", static_cast<quint8>(DateTrigger::TriggerSide::BeforeIncluding));
	DateTrigger_SideCombo->addItem("After Including", static_cast<quint8>(DateTrigger::TriggerSide::AfterIncluding));
    q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		DateTrigger_SideCombo->setCurrentIndex(0);
		if (TriggerBuilderBase->currentWidget()->children().contains(DateTrigger_SideCombo))
            q_func()->SetParameter(1, QString::number(DateTrigger_SideCombo->itemData(0).toInt()));
	});
    q->connect(DateTrigger_SideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
		if (TriggerBuilderBase->currentWidget()->children().contains(DateTrigger_SideCombo))
            q_func()->SetParameter(1, QString::number(DateTrigger_SideCombo->itemData(NewIndex).toInt()));
	});
    q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 1)
            DateTrigger_SideCombo->setCurrentIndex(DateTrigger_SideCombo->findData(parVal.toInt()));
	});
	ResLay->addWidget(SideLabel, CountRow, 0);
	ResLay->addWidget(DateTrigger_SideCombo, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* TriggerHelperDialogPrivate::CreateVectorTriggerWidget() {
    Q_Q(TriggerHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* VecLabel = new QLabel(Result);
    VecLabel->setText(q->tr("Vector that determines if the trigger is passing"));
	VectorTrigger_VecEditor = new QLineEdit(Result);
    VectorTrigger_VecEditor->setToolTip(q->tr("Accepted values are T or Y for passing and F or N for failing"));
	VectorTrigger_VecEditor->setValidator(BoolVector().GetValidator(VectorTrigger_VecEditor));
    q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		VectorTrigger_VecEditor->clear();
		if (TriggerBuilderBase->currentWidget()->children().contains(VectorTrigger_VecEditor))
			q_func()->SetParameter(0, QString());
	});
    q->connect(VectorTrigger_VecEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
		if (TriggerBuilderBase->currentWidget()->children().contains(VectorTrigger_VecEditor))
            q_func()->SetParameter(0, NewVec);
	});
    q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) VectorTrigger_VecEditor->setText(parVal);
	});
	ResLay->addWidget(VecLabel, CountRow, 0);
	ResLay->addWidget(VectorTrigger_VecEditor, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* TriggerHelperDialogPrivate::CreatePoolSizeTriggerWidget()
{
    Q_Q(TriggerHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* VecLabel = new QLabel(Result);
    VecLabel->setText(q->tr("Size of the pool that the trigger targets"));
	PoolTrigger_VecEditor = new QLineEdit(Result);
    PoolTrigger_VecEditor->setToolTip(q->tr("Accepts Vectors"));
	PoolTrigger_VecEditor->setValidator(BloombergVector().GetValidator(PoolTrigger_VecEditor));
    q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		PoolTrigger_VecEditor->clear();
		if (TriggerBuilderBase->currentWidget()->children().contains(PoolTrigger_VecEditor))
			q_func()->SetParameter(0, QString());
	});
    q->connect(PoolTrigger_VecEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
		if (TriggerBuilderBase->currentWidget()->children().contains(PoolTrigger_VecEditor))
            q_func()->SetParameter(0, NewVec);
	});
    q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) PoolTrigger_VecEditor->setText(parVal);
	});
	ResLay->addWidget(VecLabel, CountRow, 0);
	ResLay->addWidget(PoolTrigger_VecEditor, CountRow, 1);
	CountRow++;

	QLabel* SideLabel = new QLabel(Result);
    SideLabel->setText(q->tr("Side of the pool size where the trigger passes"));
	PoolTrigger_SideCombo = new QComboBox(Result);
	PoolTrigger_SideCombo->addItem("Bigger", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Bigger));
	PoolTrigger_SideCombo->addItem("Smaller", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Smaller));
	PoolTrigger_SideCombo->addItem("Exactly", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Exactly));
	PoolTrigger_SideCombo->addItem("Bigger or Equal", static_cast<quint8>(PoolSizeTrigger::TriggerSide::BiggerOrEqual));
	PoolTrigger_SideCombo->addItem("Smaller or Equal", static_cast<quint8>(PoolSizeTrigger::TriggerSide::SmallerOrEqual));
    q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		PoolTrigger_SideCombo->setCurrentIndex(0);
		if (TriggerBuilderBase->currentWidget()->children().contains(PoolTrigger_SideCombo))
            q_func()->SetParameter(1, QString::number(PoolTrigger_SideCombo->itemData(0).toInt()));
	});
    q->connect(PoolTrigger_SideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
		if (TriggerBuilderBase->currentWidget()->children().contains(PoolTrigger_SideCombo))
            q_func()->SetParameter(1, QString::number(PoolTrigger_SideCombo->itemData(NewIndex).toInt()));
	});
    q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 1) PoolTrigger_SideCombo->setCurrentIndex(PoolTrigger_SideCombo->findData(parVal.toInt()));
	});
	ResLay->addWidget(SideLabel, CountRow, 0);
	ResLay->addWidget(PoolTrigger_SideCombo, CountRow, 1);
	CountRow++;
	

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* TriggerHelperDialogPrivate::CreateTrancheTriggerWidget()
{
    Q_Q(TriggerHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	{
		QLabel* VecLabel = new QLabel(Result);
		VecLabel->setText(q->tr("Seniority group that represent the target of the trigger"));
		TrancheTrigger_SenEditor = new QLineEdit(Result);
        TrancheTrigger_SenEditor->setToolTip(q->tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
		TrancheTrigger_SenEditor->setValidator(IntegerVector().GetValidator(TrancheTrigger_SenEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SenEditor->clear();
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenEditor))
                q_func()->SetParameter(0, QString());
		});
        q->connect(TrancheTrigger_SenEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenEditor))
                q_func()->SetParameter(0, NewVec);
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 0)
                TrancheTrigger_SenEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SenEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Level of seniority the seniority group refers to"));
		TrancheTrigger_SenLvlEditor = new QLineEdit(Result);
        TrancheTrigger_SenLvlEditor->setToolTip(q->tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
		TrancheTrigger_SenLvlEditor->setValidator(IntegerVector().GetValidator(TrancheTrigger_SenLvlEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SenLvlEditor->setText("1");
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenLvlEditor))
                q_func()->SetParameter(1, "1");
		});
        q->connect(TrancheTrigger_SenLvlEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenLvlEditor))
                q_func()->SetParameter(1, NewVec);
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 1)
                TrancheTrigger_SenLvlEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SenLvlEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Target size of the pro-rata group"));
		TrancheTrigger_SizeEditor = new QLineEdit(Result);
		TrancheTrigger_SizeEditor->setValidator(BloombergVector().GetValidator(TrancheTrigger_SizeEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SizeEditor->setText("0");
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeEditor))
                q_func()->SetParameter(2, "0");
		});
        q->connect(TrancheTrigger_SizeEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeEditor))
                q_func()->SetParameter(2, NewVec);
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 2)
                TrancheTrigger_SizeEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SizeEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Multiplier that will be applied to the target size"));
		TrancheTrigger_SizeMultEditor = new QDoubleSpinBox(Result);
		TrancheTrigger_SizeMultEditor->setSingleStep(0.5);
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SizeMultEditor->setValue(1.0);
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeMultEditor))
                q_func()->SetParameter(5, "1.0");
		});
        q->connect(TrancheTrigger_SizeMultEditor, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double NewVal) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeMultEditor))
                q_func()->SetParameter(5, QString::number(NewVal, 'f'));
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 5) 
                TrancheTrigger_SizeMultEditor->setValue(parVal.toDouble());
		});

		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SizeMultEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* SideLabel = new QLabel(Result);
        SideLabel->setText(q->tr("Side of the seniority ladder where the trigger passes"));
		TrancheTrigger_SeniorSideCombo = new QComboBox(Result);
		TrancheTrigger_SeniorSideCombo->addItem("Senior", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Senior));
		TrancheTrigger_SeniorSideCombo->addItem("Junior", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Junior));
		TrancheTrigger_SeniorSideCombo->addItem("Exactly", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Exactly));
		TrancheTrigger_SeniorSideCombo->addItem("Senior or Equal", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::SeniorOrEqual));
		TrancheTrigger_SeniorSideCombo->addItem("Junior or Equal", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::JuniorOrEqual));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SeniorSideCombo->setCurrentIndex(0);
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SeniorSideCombo))
                q_func()->SetParameter(3, QString::number(TrancheTrigger_SeniorSideCombo->itemData(0).toInt()));
		});
        q->connect(TrancheTrigger_SeniorSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SeniorSideCombo))
                q_func()->SetParameter(3, QString::number(TrancheTrigger_SeniorSideCombo->itemData(NewIndex).toInt()));
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 3) 
                TrancheTrigger_SeniorSideCombo->setCurrentIndex(TrancheTrigger_SeniorSideCombo->findData(parVal.toInt()));
		});
		ResLay->addWidget(SideLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SeniorSideCombo, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* SideLabel = new QLabel(Result);
        SideLabel->setText(q->tr("Side of the group size where the trigger passes"));
		TrancheTrigger_SizeSideCombo = new QComboBox(Result);
		TrancheTrigger_SizeSideCombo->addItem("Bigger", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Bigger));
		TrancheTrigger_SizeSideCombo->addItem("Smaller", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Smaller));
		TrancheTrigger_SizeSideCombo->addItem("Exactly", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Exactly));
		TrancheTrigger_SizeSideCombo->addItem("Bigger or Equal", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::BiggerOrEqual));
		TrancheTrigger_SizeSideCombo->addItem("Smaller or Equal", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::SmallerOrEqual));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SizeSideCombo->setCurrentIndex(0);
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeSideCombo))
                q_func()->SetParameter(4, QString::number(TrancheTrigger_SizeSideCombo->itemData(0).toInt()));
		});
        q->connect(TrancheTrigger_SizeSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeSideCombo))
                q_func()->SetParameter(4, QString::number(TrancheTrigger_SizeSideCombo->itemData(NewIndex).toInt()));
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 4) 
                TrancheTrigger_SizeSideCombo->setCurrentIndex(TrancheTrigger_SizeSideCombo->findData(parVal.toInt()));
		});
		ResLay->addWidget(SideLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SizeSideCombo, CountRow, 1);
		CountRow++;
	}

	

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}
QWidget* TriggerHelperDialogPrivate::CreatePDLWidget()
{
    Q_Q(TriggerHelperDialog);
    QWidget* Result = new QWidget(q);
    QGridLayout *ResLay = new QGridLayout(Result);
    int CountRow = 0;

    {
        QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Seniority group that represent the target of the trigger"));
        TrancheTrigger_SenEditor = new QLineEdit(Result);
        TrancheTrigger_SenEditor->setToolTip(q->tr("This is the pro-rata level of the tranches whose PDL is considered.\nAccepts vectors."));
        TrancheTrigger_SenEditor->setValidator(IntegerVector().GetValidator(TrancheTrigger_SenEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int)
        {
            TrancheTrigger_SenEditor->clear();
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenEditor))
                q_func()->SetParameter(0, QString());
        });
        q->connect(TrancheTrigger_SenEditor, &QLineEdit::textChanged, [&](const QString& NewVec)
        {
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenEditor))
                q_func()->SetParameter(0, NewVec);
        });
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal)
        {
            if (parIdx == 0)
                TrancheTrigger_SenEditor->setText(parVal);
        });
        ResLay->addWidget(VecLabel, CountRow, 0);
        ResLay->addWidget(TrancheTrigger_SenEditor, CountRow, 1);
        CountRow++;
    }
    {
        QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Level of seniority the seniority group refers to"));
        TrancheTrigger_SenLvlEditor = new QLineEdit(Result);
        TrancheTrigger_SenLvlEditor->setToolTip(q->tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
        TrancheTrigger_SenLvlEditor->setValidator(IntegerVector().GetValidator(TrancheTrigger_SenLvlEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int)
        {
            TrancheTrigger_SenLvlEditor->setText("1");
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenLvlEditor))
                q_func()->SetParameter(1, "1");
        });
        q->connect(TrancheTrigger_SenLvlEditor, &QLineEdit::textChanged, [&](const QString& NewVec)
        {
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenLvlEditor))
                q_func()->SetParameter(1, NewVec);
        });
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal)
        {
            if (parIdx == 1) TrancheTrigger_SenLvlEditor->setText(parVal);
        });
        ResLay->addWidget(VecLabel, CountRow, 0);
        ResLay->addWidget(TrancheTrigger_SenLvlEditor, CountRow, 1);
        CountRow++;
    }
    {
        QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Target size of the PDL"));
        TrancheTrigger_SizeEditor = new QLineEdit(Result);
        TrancheTrigger_SizeEditor->setValidator(BloombergVector().GetValidator(TrancheTrigger_SizeEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int)
        {
            TrancheTrigger_SizeEditor->setText("0");
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeEditor))
                q_func()->SetParameter(2, "0");
        });
        q->connect(TrancheTrigger_SizeEditor, &QLineEdit::textChanged, [&](const QString& NewVec)
        {
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeEditor))
                q_func()->SetParameter(2, NewVec);
        });
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal)
        {
            if (parIdx == 2) TrancheTrigger_SizeEditor->setText(parVal);
        });
        ResLay->addWidget(VecLabel, CountRow, 0);
        ResLay->addWidget(TrancheTrigger_SizeEditor, CountRow, 1);
        CountRow++;
    }
    {
        QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Multiplier that will be applied to the target size"));
        TrancheTrigger_SizeMultEditor = new QDoubleSpinBox(Result);
        TrancheTrigger_SizeMultEditor->setSingleStep(0.5);
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int)
        {
            TrancheTrigger_SizeMultEditor->setValue(1.0);
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeMultEditor))
                q_func()->SetParameter(5, "1.0");
        });
        q->connect(TrancheTrigger_SizeMultEditor, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double NewVal)
        {
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeMultEditor))
                q_func()->SetParameter(5, QString::number(NewVal, 'f'));
        });
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal)
        {
            if (parIdx == 5) TrancheTrigger_SizeMultEditor->setValue(parVal.toDouble());
        });

        ResLay->addWidget(VecLabel, CountRow, 0);
        ResLay->addWidget(TrancheTrigger_SizeMultEditor, CountRow, 1);
        CountRow++;
    }
    {
        QLabel* SideLabel = new QLabel(Result);
        SideLabel->setText(q->tr("Side of the seniority ladder where the PDL is computed"));
        TrancheTrigger_SeniorSideCombo = new QComboBox(Result);
        TrancheTrigger_SeniorSideCombo->addItem("Senior", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Senior));
        TrancheTrigger_SeniorSideCombo->addItem("Junior", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Junior));
        TrancheTrigger_SeniorSideCombo->addItem("Exactly", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Exactly));
        TrancheTrigger_SeniorSideCombo->addItem("Senior or Equal", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::SeniorOrEqual));
        TrancheTrigger_SeniorSideCombo->addItem("Junior or Equal", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::JuniorOrEqual));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int)
        {
            TrancheTrigger_SeniorSideCombo->setCurrentIndex(0);
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SeniorSideCombo))
                q_func()->SetParameter(3, QString::number(TrancheTrigger_SeniorSideCombo->itemData(0).toInt()));
        });
        q->connect(TrancheTrigger_SeniorSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex)
        {
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SeniorSideCombo))
                q_func()->SetParameter(3, QString::number(TrancheTrigger_SeniorSideCombo->itemData(NewIndex).toInt()));
        });
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal)
        {
            if (parIdx == 3) TrancheTrigger_SeniorSideCombo->setCurrentIndex(TrancheTrigger_SeniorSideCombo->findData(parVal.toInt()));
        });
        ResLay->addWidget(SideLabel, CountRow, 0);
        ResLay->addWidget(TrancheTrigger_SeniorSideCombo, CountRow, 1);
        CountRow++;
    }
    {
        QLabel* SideLabel = new QLabel(Result);
        SideLabel->setText(q->tr("Side of the group size where the trigger passes"));
        TrancheTrigger_SizeSideCombo = new QComboBox(Result);
        TrancheTrigger_SizeSideCombo->addItem("Bigger", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Bigger));
        TrancheTrigger_SizeSideCombo->addItem("Smaller", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Smaller));
        TrancheTrigger_SizeSideCombo->addItem("Exactly", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Exactly));
        TrancheTrigger_SizeSideCombo->addItem("Bigger or Equal", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::BiggerOrEqual));
        TrancheTrigger_SizeSideCombo->addItem("Smaller or Equal", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::SmallerOrEqual));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int)
        {
            TrancheTrigger_SizeSideCombo->setCurrentIndex(0);
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeSideCombo))
                q_func()->SetParameter(4, QString::number(TrancheTrigger_SizeSideCombo->itemData(0).toInt()));
        });
        q->connect(TrancheTrigger_SizeSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex)
        {
            if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeSideCombo))
                q_func()->SetParameter(4, QString::number(TrancheTrigger_SizeSideCombo->itemData(NewIndex).toInt()));
        });
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal)
        {
            if (parIdx == 4) TrancheTrigger_SizeSideCombo->setCurrentIndex(TrancheTrigger_SizeSideCombo->findData(parVal.toInt()));
        });
        ResLay->addWidget(SideLabel, CountRow, 0);
        ResLay->addWidget(TrancheTrigger_SizeSideCombo, CountRow, 1);
        CountRow++;
    }



    ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
    return Result;
}

QWidget* TriggerHelperDialogPrivate::CreateDeferredInterestTriggerWidget()
{
    Q_Q(TriggerHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	{
		QLabel* VecLabel = new QLabel(Result);
		VecLabel->setText(q->tr("Seniority group that represent the target of the trigger"));
		DeferredTrigger_SenEditor = new QLineEdit(Result);
        DeferredTrigger_SenEditor->setToolTip(q->tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
		DeferredTrigger_SenEditor->setValidator(IntegerVector().GetValidator(DeferredTrigger_SenEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			DeferredTrigger_SenEditor->clear();
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SenEditor))
                q_func()->SetParameter(0, QString());
		});
        q->connect(DeferredTrigger_SenEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SenEditor))
                q_func()->SetParameter(0, NewVec);
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 0) DeferredTrigger_SenEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(DeferredTrigger_SenEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Level of seniority the seniority group refers to"));
		DeferredTrigger_SenLvlEditor = new QLineEdit(Result);
        DeferredTrigger_SenLvlEditor->setToolTip(q->tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
		DeferredTrigger_SenLvlEditor->setValidator(IntegerVector().GetValidator(DeferredTrigger_SenLvlEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			DeferredTrigger_SenLvlEditor->setText("1");
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SenLvlEditor))
                q_func()->SetParameter(1, "1");
		});
        q->connect(DeferredTrigger_SenLvlEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SenLvlEditor))
                q_func()->SetParameter(1, NewVec);
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 1) DeferredTrigger_SenLvlEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(DeferredTrigger_SenLvlEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Target size of the Deferred Interest"));
		DeferredTrigger_SizeEditor = new QLineEdit(Result);
		DeferredTrigger_SizeEditor->setValidator(BloombergVector().GetValidator(DeferredTrigger_SizeEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			DeferredTrigger_SizeEditor->setText("0");
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SizeEditor))
                q_func()->SetParameter(2, "0");
		});
        q->connect(DeferredTrigger_SizeEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SizeEditor))
                q_func()->SetParameter(2, NewVec);
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 2) DeferredTrigger_SizeEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(DeferredTrigger_SizeEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Target Coupon that is deferring interest"));
		DeferredTrigger_CouponEditor = new QLineEdit(Result);
		DeferredTrigger_CouponEditor->setValidator(IntegerVector().GetValidator(DeferredTrigger_CouponEditor));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			DeferredTrigger_CouponEditor->setText("1");
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_CouponEditor))
                q_func()->SetParameter(6, "1");
		});
        q->connect(DeferredTrigger_CouponEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_CouponEditor))
                q_func()->SetParameter(6, NewVec);
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 6) DeferredTrigger_CouponEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(DeferredTrigger_CouponEditor, CountRow, 1);
		DeferredTrigger_CouponEditor->setEnabled(false);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
        VecLabel->setText(q->tr("Multiplier that will be applied to the target size"));
		DeferredTrigger_SizeMultEditor = new QDoubleSpinBox(Result);
		DeferredTrigger_SizeMultEditor->setSingleStep(0.5);
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			DeferredTrigger_SizeMultEditor->setValue(1.0);
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SizeMultEditor))
                q_func()->SetParameter(5, "1.0");
		});
        q->connect(DeferredTrigger_SizeMultEditor, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double NewVal) {
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SizeMultEditor))
                q_func()->SetParameter(5, QString::number(NewVal, 'f'));
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 5) DeferredTrigger_SizeMultEditor->setValue(parVal.toDouble());
		});

		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(DeferredTrigger_SizeMultEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* SideLabel = new QLabel(Result);
        SideLabel->setText(q->tr("Side of the seniority ladder the trigger applies too"));
		DeferredTrigger_SeniorSideCombo = new QComboBox(Result);
        DeferredTrigger_SeniorSideCombo->addItem(q->tr("Senior"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Senior));
        DeferredTrigger_SeniorSideCombo->addItem(q->tr("Junior"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Junior));
        DeferredTrigger_SeniorSideCombo->addItem(q->tr("Exactly"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Exactly));
        DeferredTrigger_SeniorSideCombo->addItem(q->tr("Senior or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::SeniorOrEqual));
        DeferredTrigger_SeniorSideCombo->addItem(q->tr("Junior or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::JuniorOrEqual));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			DeferredTrigger_SeniorSideCombo->setCurrentIndex(0);
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SeniorSideCombo))
                q_func()->SetParameter(3, QString::number(DeferredTrigger_SeniorSideCombo->itemData(0).toInt()));
		});
        q->connect(DeferredTrigger_SeniorSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SeniorSideCombo))
                q_func()->SetParameter(3, QString::number(DeferredTrigger_SeniorSideCombo->itemData(NewIndex).toInt()));
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 3) DeferredTrigger_SeniorSideCombo->setCurrentIndex(DeferredTrigger_SeniorSideCombo->findData(parVal.toInt()));
		});
		ResLay->addWidget(SideLabel, CountRow, 0);
		ResLay->addWidget(DeferredTrigger_SeniorSideCombo, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* SideLabel = new QLabel(Result);
        SideLabel->setText(q->tr("Side of the size where the trigger passes"));
		DeferredTrigger_SizeSideCombo = new QComboBox(Result);
        DeferredTrigger_SizeSideCombo->addItem(q->tr("Bigger"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Bigger));
        DeferredTrigger_SizeSideCombo->addItem(q->tr("Smaller"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Smaller));
        DeferredTrigger_SizeSideCombo->addItem(q->tr("Exactly"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Exactly));
        DeferredTrigger_SizeSideCombo->addItem(q->tr("Bigger or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::BiggerOrEqual));
        DeferredTrigger_SizeSideCombo->addItem(q->tr("Smaller or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::SmallerOrEqual));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			DeferredTrigger_SizeSideCombo->setCurrentIndex(0);
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SizeSideCombo))
                q_func()->SetParameter(4, QString::number(DeferredTrigger_SizeSideCombo->itemData(0).toInt()));
		});
        q->connect(DeferredTrigger_SizeSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_SizeSideCombo))
                q_func()->SetParameter(4, QString::number(DeferredTrigger_SizeSideCombo->itemData(NewIndex).toInt()));
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 4) DeferredTrigger_SizeSideCombo->setCurrentIndex(DeferredTrigger_SizeSideCombo->findData(parVal.toInt()));
		});
		ResLay->addWidget(SideLabel, CountRow, 0);
		ResLay->addWidget(DeferredTrigger_SizeSideCombo, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* SideLabel = new QLabel(Result);
        SideLabel->setText(q->tr("Side of the coupon that the trigger should consider"));
		DeferredTrigger_CouponSideCombo = new QComboBox(Result);
        DeferredTrigger_CouponSideCombo->addItem(q->tr("Bigger"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Bigger));
        DeferredTrigger_CouponSideCombo->addItem(q->tr("Smaller"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Smaller));
        DeferredTrigger_CouponSideCombo->addItem(q->tr("Exactly"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Exactly));
        DeferredTrigger_CouponSideCombo->addItem(q->tr("Bigger or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::BiggerOrEqual));
        DeferredTrigger_CouponSideCombo->addItem(q->tr("Smaller or Equal"), static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::SmallerOrEqual));
        q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			DeferredTrigger_CouponSideCombo->setCurrentIndex(2);
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_CouponSideCombo))
                q_func()->SetParameter(7, QString::number(DeferredTrigger_CouponSideCombo->itemData(2).toInt()));
		});
        q->connect(DeferredTrigger_CouponSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
			if (TriggerBuilderBase->currentWidget()->children().contains(DeferredTrigger_CouponSideCombo))
                q_func()->SetParameter(7, QString::number(DeferredTrigger_CouponSideCombo->itemData(NewIndex).toInt()));
		});
        q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 7) DeferredTrigger_CouponSideCombo->setCurrentIndex(DeferredTrigger_CouponSideCombo->findData(parVal.toInt()));
		});
		ResLay->addWidget(SideLabel, CountRow, 0);
		ResLay->addWidget(DeferredTrigger_CouponSideCombo, CountRow, 1);
		DeferredTrigger_CouponSideCombo->setEnabled(false);
		CountRow++;
	}



	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}


QWidget* TriggerHelperDialogPrivate::CreateDelinquencyTriggerWidget()
{
    Q_Q(TriggerHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* VecLabel = new QLabel(Result);
    VecLabel->setText(q->tr("Percentage of the pool in arrears"));
	DelinqTrigger_VecEditor = new QLineEdit(Result);
    DelinqTrigger_VecEditor->setToolTip(q->tr("Accepts vectors. The trigger will pass if the delinquency are less than this target value."));
	DelinqTrigger_VecEditor->setValidator(BloombergVector().GetValidator(DelinqTrigger_VecEditor));
    q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		DelinqTrigger_VecEditor->clear();
		if (TriggerBuilderBase->currentWidget()->children().contains(DelinqTrigger_VecEditor))
			q_func()->SetParameter(0, QString());
	});
    q->connect(DelinqTrigger_VecEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
		if (TriggerBuilderBase->currentWidget()->children().contains(DelinqTrigger_VecEditor))
            q_func()->SetParameter(0, NewVec);
	});
    q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) DelinqTrigger_VecEditor->setText(parVal);
	});
	ResLay->addWidget(VecLabel, CountRow, 0);
	ResLay->addWidget(DelinqTrigger_VecEditor, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

void TriggerHelperDialog::ClearParameters()
{
    Q_D(TriggerHelperDialog);
	d->ParamColumns.clear();
}

void TriggerHelperDialog::SetCurrentPars(const QString& pars)
{
    Q_D(TriggerHelperDialog);
    d->Cleared = false;
	if (pars.isEmpty()) return;
	auto AllPars = pars.split('#');
    d->TriggerTypeCombo->setCurrentIndex(d->TriggerTypeCombo->findData(AllPars.first().toInt()));
	if (AllPars.size()>1){
        if (AllPars.at(1).isEmpty()) d->TriggerLabelEdit->setText("Trigger");
        else d->TriggerLabelEdit->setText(AllPars.at(1));
		int CountPars = 0;
		for (auto i = AllPars.constBegin() + 2; i != AllPars.constEnd(); ++i, ++CountPars) {
			emit ImportParam(CountPars, *i);
		}
		ClearParameters();
		AllPars.removeFirst();
		AllPars.removeFirst();
        d->ParamColumns = AllPars;
	}
	else {
        if (AllPars.at(1).isEmpty())
            d->TriggerLabelEdit->setText("Trigger");
	}
}

QWidget* TriggerHelperDialogPrivate::CreateCumulativeLossTriggerWidget()
{
    Q_Q(TriggerHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* VecLabel = new QLabel(Result);
	VecLabel->setText(q->tr("Size cumulative losses that the trigger targets"));
	CumLoss_VecEditor = new QLineEdit(Result);
	CumLoss_VecEditor->setToolTip(q->tr("Accepts Vectors"));
	CumLoss_VecEditor->setValidator(BloombergVector().GetValidator(PoolTrigger_VecEditor));
    q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		CumLoss_VecEditor->clear();
		if (TriggerBuilderBase->currentWidget()->children().contains(CumLoss_VecEditor))
			q_func()->SetParameter(0, QString());
	});
    q->connect(CumLoss_VecEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
		if (TriggerBuilderBase->currentWidget()->children().contains(CumLoss_VecEditor))
            q_func()->SetParameter(0, NewVec);
	});
    q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) CumLoss_VecEditor->setText(parVal);
	});
	ResLay->addWidget(VecLabel, CountRow, 0);
	ResLay->addWidget(CumLoss_VecEditor, CountRow, 1);
	CountRow++;

	QLabel* SideLabel = new QLabel(Result);
    SideLabel->setText(q->tr("Side of the cumulative losses where the trigger passes"));
	CumLoss_SideCombo = new QComboBox(Result);
	CumLoss_SideCombo->addItem("Bigger", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Bigger));
	CumLoss_SideCombo->addItem("Smaller", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Smaller));
	CumLoss_SideCombo->addItem("Exactly", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Exactly));
	CumLoss_SideCombo->addItem("Bigger or Equal", static_cast<quint8>(PoolSizeTrigger::TriggerSide::BiggerOrEqual));
	CumLoss_SideCombo->addItem("Smaller or Equal", static_cast<quint8>(PoolSizeTrigger::TriggerSide::SmallerOrEqual));
    q->connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		CumLoss_SideCombo->setCurrentIndex(0);
		if (TriggerBuilderBase->currentWidget()->children().contains(CumLoss_SideCombo))
            q_func()->SetParameter(1, QString::number(CumLoss_SideCombo->itemData(0).toInt()));
	});
    q->connect(CumLoss_SideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
		if (TriggerBuilderBase->currentWidget()->children().contains(CumLoss_SideCombo))
            q_func()->SetParameter(1, QString::number(CumLoss_SideCombo->itemData(NewIndex).toInt()));
	});
    q->connect(q, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 1) CumLoss_SideCombo->setCurrentIndex(CumLoss_SideCombo->findData(parVal.toInt()));
	});
	ResLay->addWidget(SideLabel, CountRow, 0);
	ResLay->addWidget(CumLoss_SideCombo, CountRow, 1);
	CountRow++;


	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}
