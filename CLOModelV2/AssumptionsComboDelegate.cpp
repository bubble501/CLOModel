#include "AssumptionsComboDelegate.h"
#include <QComboBox>

AssumptionsComboDelegate::AssumptionsComboDelegate(QObject *parent) : QItemDelegate(parent) {
	ComboElements.insert(LoanAssumption::MaturityExtension, "Maturity Extension");
	ComboElements.insert(LoanAssumption::InitialHaircut, "Initial Haircut");
	ComboElements.insert(LoanAssumption::PrepaymentFee, "Prepayment Fee");
	ComboElements.insert(LoanAssumption::DayCount, "Day Count");
	ComboElements.insert(LoanAssumption::CPR, "CPR");
	ComboElements.insert(LoanAssumption::CDR, "CDR");
	ComboElements.insert(LoanAssumption::LS, "LS");
	ComboElements.insert(LoanAssumption::RecoveryLag, "Recovery Lag");
	ComboElements.insert(LoanAssumption::Delinquency, "Delinquency");
	ComboElements.insert(LoanAssumption::DelinquencyLag, "Delinquency Lag");
	ComboElements.insert(LoanAssumption::Price, "Price");
	ComboElements.insert(LoanAssumption::Haircut, "Haircut");
	ComboElements.insert(LoanAssumption::PrepayMultiplier, "Prepay Multiplier");
	ComboElements.insert(LoanAssumption::LossMultiplier, "Loss Multiplier");
}
QWidget *AssumptionsComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
	QComboBox *editor = new QComboBox(parent);
	for (auto i = ComboElements.constBegin(); i != ComboElements.constEnd(); ++i) {
		editor->addItem(i.value(), i.key());
	}
	connect(editor, SIGNAL(currentIndexChanged(int)), this, SIGNAL(Edited()));
	return editor;
}
void AssumptionsComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	static_cast<QComboBox*>(editor)->setCurrentIndex(index.model()->data(index, Qt::UserRole).toInt());
}

void AssumptionsComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	model->setData(index, (static_cast<QComboBox*>(editor))->currentData(), Qt::UserRole);
	model->setData(index, (static_cast<QComboBox*>(editor))->currentText(), Qt::EditRole);
}
void AssumptionsComboDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
	editor->setGeometry(option.rect);
}

QString AssumptionsComboDelegate::ComboElementforIndex(const QModelIndex &index) {
	return ComboElements.value(index.model()->data(index, Qt::UserRole).toInt(), QString());
}
