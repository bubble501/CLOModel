#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include "BloombergVector.h"
#include "IntegerVector.h"
#include "DayCountVect.h"
#include "LoanAssumptionDelegate.h"
LoanAssumptionDelegate::LoanAssumptionDelegate(QObject *parent) 
	: QItemDelegate(parent) 
{}
QWidget *LoanAssumptionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex & index) const {
	switch (index.data(Qt::UserRole).toInt()) {
	case static_cast<qint8>(AssumptionType::DoubleAssumption) : {
		QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
		editor->setMinimum(0.0);
		editor->setSingleStep(5.0);
		return editor;
	}
	case static_cast<qint8>(AssumptionType::DoubleAssumption0To100) : {
		QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
		editor->setMinimum(0.0);
		editor->setSingleStep(5.0);
		editor->setMaximum(100.0);
		return editor;
	}
	case static_cast<qint8>(AssumptionType::IntegerAssumption) : {
		QSpinBox *editor = new QSpinBox(parent);
		editor->setMinimum(0);
		editor->setSingleStep(1);
		return editor;
	}
	case static_cast<qint8>(AssumptionType::BloombergVectorAssumption) : {
		QLineEdit *editor = new QLineEdit(parent);
		editor->setValidator(BloombergVector().GetValidator(editor));
		return editor;
	}
	case static_cast<qint8>(AssumptionType::IntegerVectorAssumption) : {
		QLineEdit *editor = new QLineEdit(parent);
		editor->setValidator(IntegerVector().GetValidator(editor));
		return editor;
	}
	case static_cast<qint8>(AssumptionType::DayCountVectorAssumption) : {
		QLineEdit *editor = new QLineEdit(parent);
		editor->setValidator(DayCountVector().GetValidator(editor));
		return editor;
	}
	}
	return new QWidget();
}
void LoanAssumptionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	switch (index.data(Qt::UserRole).toInt()) {
	case static_cast<qint8>(AssumptionType::DoubleAssumption) :
	case static_cast<qint8>(AssumptionType::DoubleAssumption0To100) :
		return qobject_cast<QDoubleSpinBox*>(editor)->setValue(index.model()->data(index, Qt::EditRole).toDouble());
	case static_cast<qint8>(AssumptionType::IntegerAssumption):
		return qobject_cast<QSpinBox*>(editor)->setValue(index.model()->data(index, Qt::EditRole).toInt());
	case static_cast<qint8>(AssumptionType::BloombergVectorAssumption) :
	case static_cast<qint8>(AssumptionType::IntegerVectorAssumption) :
	case static_cast<qint8>(AssumptionType::DayCountVectorAssumption) :
		return qobject_cast<QLineEdit*>(editor)->setText(index.model()->data(index, Qt::EditRole).toString());
	}
}

void LoanAssumptionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	switch (index.data(Qt::UserRole).toInt()) {
	case static_cast<qint8>(AssumptionType::DoubleAssumption) :
	case static_cast<qint8>(AssumptionType::DoubleAssumption0To100) : {
		QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
		spinBox->interpretText();
		model->setData(index, spinBox->value(), Qt::EditRole);
		break;
	}
	case static_cast<qint8>(AssumptionType::IntegerAssumption) : {
		QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);
		spinBox->interpretText();
		model->setData(index, spinBox->value(), Qt::EditRole);
		break;
	}
	case static_cast<qint8>(AssumptionType::BloombergVectorAssumption) :
	case static_cast<qint8>(AssumptionType::IntegerVectorAssumption) :
	case static_cast<qint8>(AssumptionType::DayCountVectorAssumption) :
		model->setData(index, qobject_cast<QLineEdit*>(editor)->text(), Qt::EditRole);
	}
}

void LoanAssumptionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
	editor->setGeometry(option.rect);
}