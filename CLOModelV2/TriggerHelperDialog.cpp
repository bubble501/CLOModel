#include "TriggerHelperDialog.h"
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
#include "TrancheTrigger.h"
TriggerHelperDialog::TriggerHelperDialog(QDialog *parent)
	: QDialog(parent)
	, FirstCombodeleted(false)
	, Cleared(false)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("Edit trigger");
	QLabel *TriggerSelectLabel = new QLabel(this);
	TriggerSelectLabel->setText(tr("Select Trigger Type"));
	TriggerTypeCombo = new QComboBox(this);
	TriggerTypeCombo->addItem("", -1);
	TriggerTypeCombo->addItem("Date", static_cast<quint8>(AbstractTrigger::TriggerType::DateTrigger));
	TriggerTypeCombo->addItem("Vector", static_cast<quint8>(AbstractTrigger::TriggerType::VectorTrigger));
	TriggerTypeCombo->addItem("Pool Size", static_cast<quint8>(AbstractTrigger::TriggerType::PoolSizeTrigger));
	TriggerTypeCombo->addItem("Tranche", static_cast<quint8>(AbstractTrigger::TriggerType::TrancheTrigger));
	TriggerTypeCombo->addItem("Delinquencies", static_cast<quint8>(AbstractTrigger::TriggerType::DelinquencyTrigger));
	TriggerTypeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	
	QLabel *TriggerLabelLabel = new QLabel(this);
	TriggerLabelLabel->setText(tr("Insert Trigger Label"));
	TriggerLabelEdit = new QLineEdit(this);
	TriggerLabelEdit->setValidator(new QRegExpValidator(QRegExp(".+"), TriggerLabelEdit));
	TriggerLabelEdit->setText("Trigger");
	TriggerLabelEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	TriggerBuilderBase = new QStackedWidget(this);
	connect(TriggerTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ClearParameters()));
	connect(TriggerTypeCombo, SIGNAL(currentIndexChanged(int)), TriggerBuilderBase, SLOT(setCurrentIndex(int)));
	connect(TriggerTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int)));
	TriggerBuilderBase->setMinimumSize(200, 200);
	TriggerBuilderBase->setStyleSheet("QLabel { qproperty-alignment: 'AlignRight | AlignVCenter'; }");
	TriggerBuilderBase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	TriggerBuilderBase->addWidget(new QWidget(this)); //No Trigger Selected
	TriggerBuilderBase->addWidget(CreateDateTriggerWidget());
	TriggerBuilderBase->addWidget(CreateVectorTriggerWidget());
	TriggerBuilderBase->addWidget(CreatePoolSizeTriggerWidget());
	TriggerBuilderBase->addWidget(CreateTrancheTriggerWidget());
	TriggerBuilderBase->addWidget(CreateDelinquencyTriggerWidget());
	
	

	AcceptButton = new QPushButton(this);
	AcceptButton->setText(tr("OK"));
	AcceptButton->setDefault(true);
	AcceptButton->setEnabled(false);
	connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);
	QPushButton*  CancelButton = new QPushButton(this);
	CancelButton->setText(tr("Cancel"));
	CancelButton->setDefault(false);
	connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);
	QPushButton* ClearAcceptButton = new QPushButton(this);
	ClearAcceptButton->setText(tr("Clear"));
	ClearAcceptButton->setDefault(false);
	connect(ClearAcceptButton, &QPushButton::clicked, this, [&]() { Cleared = true; accept(); });

	QGroupBox *TopGroup = new QGroupBox(this);
	TopGroup->setTitle("Trigger");
	QGroupBox *ParamGroup = new QGroupBox(this);
	ParamGroup->setTitle("Parameters");
	ParamGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	QHBoxLayout* ParamLay = new QHBoxLayout(ParamGroup);
	ParamLay->addWidget(TriggerBuilderBase);
	QGridLayout* TopLay = new QGridLayout(TopGroup);
	TopLay->addWidget(TriggerSelectLabel,0,0);
	TopLay->addWidget(TriggerTypeCombo,0,1);
	TopLay->addWidget(TriggerLabelLabel, 1, 0);
	TopLay->addWidget(TriggerLabelEdit, 1, 1);

	QHBoxLayout* ButtonsLay = new QHBoxLayout;
	ButtonsLay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	ButtonsLay->addWidget(AcceptButton);
	ButtonsLay->addWidget(ClearAcceptButton);
	ButtonsLay->addWidget(CancelButton);

	QVBoxLayout* mainlay = new QVBoxLayout(this);
	mainlay->addWidget(TopGroup);
	mainlay->addWidget(ParamGroup);
	mainlay->addLayout(ButtonsLay);
}

void TriggerHelperDialog::CheckOkEnabled(int index) {
	if (FirstCombodeleted) return;
	AcceptButton->setEnabled(index != 0);
	if (index != 0) {
		disconnect(TriggerTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int)));
		QWidget* EmptyWidg = TriggerBuilderBase->widget(0);
		TriggerBuilderBase->removeWidget(EmptyWidg);
		TriggerBuilderBase->setCurrentIndex(index - 1);
		EmptyWidg->deleteLater();
		FirstCombodeleted = true;
		TriggerTypeCombo->removeItem(0);
		
	}
}

void TriggerHelperDialog::SetParameter(int parIdx, const QString& parVal) {
	if (parIdx<0 || (parVal.isEmpty() && parIdx >= ParamColumns.size())) return;
	while (ParamColumns.size()<=parIdx) {
		ParamColumns.append(QString());
	}
	ParamColumns[parIdx] = parVal;
	while (ParamColumns.last().isEmpty()) {
		ParamColumns.removeLast();
		if (ParamColumns.isEmpty()) return;
	}
}

QString TriggerHelperDialog::GetParameters() const {
	if (Cleared) return QString("#");
	return 
		QString::number(TriggerTypeCombo->currentData().toInt()) + '#' +
		TriggerLabelEdit->text() + '#' +
		ParamColumns.join('#')
	;
}

QWidget* TriggerHelperDialog::CreateDateTriggerWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* DateLabel = new QLabel(Result);
	DateLabel->setText(tr("Threshold date for the trigger"));
	DateTrigger_dateEdit = new QDateEdit(Result);
	DateTrigger_dateEdit->setDate(QDate::currentDate());
	DateTrigger_dateEdit->setCalendarPopup(true);
	connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		DateTrigger_dateEdit->setDate(QDate::currentDate());
		if (TriggerBuilderBase->currentWidget()->children().contains(DateTrigger_dateEdit))
			SetParameter(0, QDate::currentDate().toString("yyyy-MM-dd"));
	});
	connect(DateTrigger_dateEdit, &QDateEdit::dateChanged, [&](const QDate& NewDate) {
		if (TriggerBuilderBase->currentWidget()->children().contains(DateTrigger_dateEdit))
		{
			if (NewDate.isNull()) SetParameter(0, QString());
			else SetParameter(0, NewDate.toString("yyyy-MM-dd"));
		}
	});
	connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) {
			if (parVal.isEmpty()) DateTrigger_dateEdit->setDate(QDate::currentDate());
			else DateTrigger_dateEdit->setDate(QDate::fromString(parVal, "yyyy-MM-dd"));
		}
	});
	ResLay->addWidget(DateLabel, CountRow, 0);
	ResLay->addWidget(DateTrigger_dateEdit, CountRow, 1);
	CountRow++;

	QLabel* SideLabel = new QLabel(Result);
	SideLabel->setText(tr("Side of the date where the trigger passes"));
	DateTrigger_SideCombo = new QComboBox(Result);
	DateTrigger_SideCombo->addItem("Before Excluding", static_cast<quint8>(DateTrigger::TriggerSide::BeforeExcluding));
	DateTrigger_SideCombo->addItem("After Excluding", static_cast<quint8>(DateTrigger::TriggerSide::AfterExcluding));
	DateTrigger_SideCombo->addItem("Exactly", static_cast<quint8>(DateTrigger::TriggerSide::Exactly));
	DateTrigger_SideCombo->addItem("Before Including", static_cast<quint8>(DateTrigger::TriggerSide::BeforeIncluding));
	DateTrigger_SideCombo->addItem("After Including", static_cast<quint8>(DateTrigger::TriggerSide::AfterIncluding));
	connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		DateTrigger_SideCombo->setCurrentIndex(0);
		if (TriggerBuilderBase->currentWidget()->children().contains(DateTrigger_SideCombo))
			SetParameter(1, QString::number(DateTrigger_SideCombo->itemData(0).toInt()));
	});
	connect(DateTrigger_SideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
		if (TriggerBuilderBase->currentWidget()->children().contains(DateTrigger_SideCombo))
			SetParameter(1, QString::number(DateTrigger_SideCombo->itemData(NewIndex).toInt())); 
	});
	connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 1) DateTrigger_SideCombo->setCurrentIndex(DateTrigger_SideCombo->findData(parVal.toInt()));
	});
	ResLay->addWidget(SideLabel, CountRow, 0);
	ResLay->addWidget(DateTrigger_SideCombo, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* TriggerHelperDialog::CreateVectorTriggerWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* VecLabel = new QLabel(Result);
	VecLabel->setText(tr("Vector that determines if the trigger is passing"));
	VectorTrigger_VecEditor = new QLineEdit(Result);
	VectorTrigger_VecEditor->setToolTip(tr("Accepted values are T or Y for passing and F or N for failing"));
	VectorTrigger_VecEditor->setValidator(BoolVector().GetValidator(VectorTrigger_VecEditor));
	connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		VectorTrigger_VecEditor->clear();
		if (TriggerBuilderBase->currentWidget()->children().contains(VectorTrigger_VecEditor))
			SetParameter(0, QString());
	});
	connect(VectorTrigger_VecEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
		if (TriggerBuilderBase->currentWidget()->children().contains(VectorTrigger_VecEditor))
			SetParameter(0, NewVec);
	});
	connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) VectorTrigger_VecEditor->setText(parVal);
	});
	ResLay->addWidget(VecLabel, CountRow, 0);
	ResLay->addWidget(VectorTrigger_VecEditor, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* TriggerHelperDialog::CreatePoolSizeTriggerWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* VecLabel = new QLabel(Result);
	VecLabel->setText(tr("Size of the pool that the trigger targets"));
	PoolTrigger_VecEditor = new QLineEdit(Result);
	PoolTrigger_VecEditor->setToolTip(tr("Accepts Vectors"));
	PoolTrigger_VecEditor->setValidator(BloombergVector().GetValidator(PoolTrigger_VecEditor));
	connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		PoolTrigger_VecEditor->clear();
		if (TriggerBuilderBase->currentWidget()->children().contains(PoolTrigger_VecEditor))
			SetParameter(0, QString());
	});
	connect(PoolTrigger_VecEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
		if (TriggerBuilderBase->currentWidget()->children().contains(PoolTrigger_VecEditor))
			SetParameter(0, NewVec);
	});
	connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) PoolTrigger_VecEditor->setText(parVal);
	});
	ResLay->addWidget(VecLabel, CountRow, 0);
	ResLay->addWidget(PoolTrigger_VecEditor, CountRow, 1);
	CountRow++;

	QLabel* SideLabel = new QLabel(Result);
	SideLabel->setText(tr("Side of the pool size where the trigger passes"));
	PoolTrigger_SideCombo = new QComboBox(Result);
	PoolTrigger_SideCombo->addItem("Bigger", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Bigger));
	PoolTrigger_SideCombo->addItem("Smaller", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Smaller));
	PoolTrigger_SideCombo->addItem("Exactly", static_cast<quint8>(PoolSizeTrigger::TriggerSide::Exactly));
	PoolTrigger_SideCombo->addItem("Bigger or Equal", static_cast<quint8>(PoolSizeTrigger::TriggerSide::BiggerOrEqual));
	PoolTrigger_SideCombo->addItem("Smaller or Equal", static_cast<quint8>(PoolSizeTrigger::TriggerSide::SmallerOrEqual));
	connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		PoolTrigger_SideCombo->setCurrentIndex(0);
		if (TriggerBuilderBase->currentWidget()->children().contains(PoolTrigger_SideCombo))
			SetParameter(1, QString::number(PoolTrigger_SideCombo->itemData(0).toInt()));
	});
	connect(PoolTrigger_SideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
		if (TriggerBuilderBase->currentWidget()->children().contains(PoolTrigger_SideCombo))
			SetParameter(1, QString::number(PoolTrigger_SideCombo->itemData(NewIndex).toInt()));
	});
	connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 1) PoolTrigger_SideCombo->setCurrentIndex(PoolTrigger_SideCombo->findData(parVal.toInt()));
	});
	ResLay->addWidget(SideLabel, CountRow, 0);
	ResLay->addWidget(PoolTrigger_SideCombo, CountRow, 1);
	CountRow++;
	

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* TriggerHelperDialog::CreateTrancheTriggerWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	{
		QLabel* VecLabel = new QLabel(Result);
		VecLabel->setText(tr("Seniority group that represent the target of the trigger"));
		TrancheTrigger_SenEditor = new QLineEdit(Result);
		TrancheTrigger_SenEditor->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
		TrancheTrigger_SenEditor->setValidator(IntegerVector().GetValidator(TrancheTrigger_SenEditor));
		connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SenEditor->clear();
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenEditor))
				SetParameter(0, QString());
		});
		connect(TrancheTrigger_SenEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenEditor))
				SetParameter(0, NewVec);
		});
		connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 0) TrancheTrigger_SenEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SenEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
		VecLabel->setText(tr("Level of seniority the seniority group refers to"));
		TrancheTrigger_SenLvlEditor = new QLineEdit(Result);
		TrancheTrigger_SenLvlEditor->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
		TrancheTrigger_SenLvlEditor->setValidator(IntegerVector().GetValidator(TrancheTrigger_SenLvlEditor));
		connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SenLvlEditor->setText("1");
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenLvlEditor))
				SetParameter(1, "1");
		});
		connect(TrancheTrigger_SenLvlEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SenLvlEditor))
				SetParameter(1, NewVec);
		});
		connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 1) TrancheTrigger_SenLvlEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SenLvlEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
		VecLabel->setText(tr("Target size of the pro-rata group"));
		TrancheTrigger_SizeEditor = new QLineEdit(Result);
		TrancheTrigger_SizeEditor->setValidator(BloombergVector().GetValidator(TrancheTrigger_SizeEditor));
		connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SizeEditor->setText("0");
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeEditor))
				SetParameter(2, "0");
		});
		connect(TrancheTrigger_SizeEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeEditor))
				SetParameter(2, NewVec);
		});
		connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 2) TrancheTrigger_SizeEditor->setText(parVal);
		});
		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SizeEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* VecLabel = new QLabel(Result);
		VecLabel->setText(tr("Multiplier that will be applied to the target size"));
		TrancheTrigger_SizeMultEditor = new QDoubleSpinBox(Result);
		TrancheTrigger_SizeMultEditor->setSingleStep(0.5);
		connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SizeMultEditor->setValue(1.0);
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeMultEditor))
				SetParameter(5, "1.0");
		});
		connect(TrancheTrigger_SizeMultEditor, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double NewVal) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeMultEditor))
				SetParameter(5, QString::number(NewVal, 'f'));
		});
		connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 5) TrancheTrigger_SizeMultEditor->setValue(parVal.toDouble());
		});

		ResLay->addWidget(VecLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SizeMultEditor, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* SideLabel = new QLabel(Result);
		SideLabel->setText(tr("Side of the seniority ladder where the trigger passes"));
		TrancheTrigger_SeniorSideCombo = new QComboBox(Result);
		TrancheTrigger_SeniorSideCombo->addItem("Senior", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Senior));
		TrancheTrigger_SeniorSideCombo->addItem("Junior", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Junior));
		TrancheTrigger_SeniorSideCombo->addItem("Exactly", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::Exactly));
		TrancheTrigger_SeniorSideCombo->addItem("Senior or Equal", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::SeniorOrEqual));
		TrancheTrigger_SeniorSideCombo->addItem("Junior or Equal", static_cast<quint8>(TrancheTrigger::TriggerSenioritySide::JuniorOrEqual));
		connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SeniorSideCombo->setCurrentIndex(0);
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SeniorSideCombo))
				SetParameter(3, QString::number(TrancheTrigger_SeniorSideCombo->itemData(0).toInt()));
		});
		connect(TrancheTrigger_SeniorSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SeniorSideCombo))
				SetParameter(3, QString::number(TrancheTrigger_SeniorSideCombo->itemData(NewIndex).toInt()));
		});
		connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 3) TrancheTrigger_SeniorSideCombo->setCurrentIndex(TrancheTrigger_SeniorSideCombo->findData(parVal.toInt()));
		});
		ResLay->addWidget(SideLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SeniorSideCombo, CountRow, 1);
		CountRow++;
	}
	{
		QLabel* SideLabel = new QLabel(Result);
		SideLabel->setText(tr("Side of the group size where the trigger passes"));
		TrancheTrigger_SizeSideCombo = new QComboBox(Result);
		TrancheTrigger_SizeSideCombo->addItem("Bigger", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Bigger));
		TrancheTrigger_SizeSideCombo->addItem("Smaller", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Smaller));
		TrancheTrigger_SizeSideCombo->addItem("Exactly", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::Exactly));
		TrancheTrigger_SizeSideCombo->addItem("Bigger or Equal", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::BiggerOrEqual));
		TrancheTrigger_SizeSideCombo->addItem("Smaller or Equal", static_cast<quint8>(TrancheTrigger::TriggerSizeSide::SmallerOrEqual));
		connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
			TrancheTrigger_SizeSideCombo->setCurrentIndex(0);
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeSideCombo))
				SetParameter(4, QString::number(TrancheTrigger_SizeSideCombo->itemData(0).toInt()));
		});
		connect(TrancheTrigger_SizeSideCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int NewIndex) {
			if (TriggerBuilderBase->currentWidget()->children().contains(TrancheTrigger_SizeSideCombo))
				SetParameter(4, QString::number(TrancheTrigger_SizeSideCombo->itemData(NewIndex).toInt()));
		});
		connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
			if (parIdx == 4) TrancheTrigger_SizeSideCombo->setCurrentIndex(TrancheTrigger_SizeSideCombo->findData(parVal.toInt()));
		});
		ResLay->addWidget(SideLabel, CountRow, 0);
		ResLay->addWidget(TrancheTrigger_SizeSideCombo, CountRow, 1);
		CountRow++;
	}

	

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* TriggerHelperDialog::CreateDelinquencyTriggerWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* VecLabel = new QLabel(Result);
	VecLabel->setText(tr("Percentage of the pool in arrears"));
	DelinqTrigger_VecEditor = new QLineEdit(Result);
	DelinqTrigger_VecEditor->setToolTip(tr("Accepts vectors. The trigger will pass if the delinquency are less than this target value."));
	DelinqTrigger_VecEditor->setValidator(BloombergVector().GetValidator(DelinqTrigger_VecEditor));
	connect(TriggerBuilderBase, &QStackedWidget::currentChanged, [&](int) {
		DelinqTrigger_VecEditor->clear();
		if (TriggerBuilderBase->currentWidget()->children().contains(DelinqTrigger_VecEditor))
			SetParameter(0, QString());
	});
	connect(DelinqTrigger_VecEditor, &QLineEdit::textChanged, [&](const QString& NewVec) {
		if (TriggerBuilderBase->currentWidget()->children().contains(DelinqTrigger_VecEditor))
		SetParameter(0, NewVec);
	});
	connect(this, &TriggerHelperDialog::ImportParam, [&](int parIdx, const QString& parVal) {
		if (parIdx == 0) DelinqTrigger_VecEditor->setText(parVal);
	});
	ResLay->addWidget(VecLabel, CountRow, 0);
	ResLay->addWidget(DelinqTrigger_VecEditor, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

void TriggerHelperDialog::ClearParameters() {
	ParamColumns.clear();
}

void TriggerHelperDialog::SetCurrentPars(const QString& pars) {
	Cleared = false;
	if (pars.isEmpty()) return;
	auto AllPars = pars.split('#');
	TriggerTypeCombo->setCurrentIndex(TriggerTypeCombo->findData(AllPars.first().toInt()));
	if (AllPars.size()>1){
		if (AllPars.at(1).isEmpty()) TriggerLabelEdit->setText("Trigger");
		else TriggerLabelEdit->setText(AllPars.at(1));
		int CountPars = 0;
		for (auto i = AllPars.constBegin() + 2; i != AllPars.constEnd(); ++i, ++CountPars) {
			emit ImportParam(CountPars, *i);
		}
		ClearParameters();
		AllPars.removeFirst();
		AllPars.removeFirst();
		ParamColumns = AllPars;
	}
	else {
		if (AllPars.at(1).isEmpty()) TriggerLabelEdit->setText("Trigger");
	}
}
