#include "CheckAndEditDelegate.h"
#include "CheckAndEdit.h"
#include <QModelIndex>
#include <QPainter>
#include <QPixmapCache>
#include <QLineEdit>
CheckAndEditDelegate::CheckAndEditDelegate(QObject *parent)
	: QStyledItemDelegate(parent) {
	Styler = new CheckAndEdit;
}
void CheckAndEditDelegate::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
	bool selected = option.state & QStyle::State_Selected;
	bool checked = index.model()->data(index, Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked;
	QString editText = index.model()->data(index, Qt::DisplayRole).toString();
	QString cacheKey = QString("CHECKANDEDIT:%1;%2;%3;%4;%5").arg(selected).arg(checked).arg(editText).arg(option.rect.width()).arg(option.rect.height());
	QPixmap pixmap(option.rect.size());
	if (!QPixmapCache::find(cacheKey, &pixmap)) {
		QPalette palette(option.palette);
		palette.setColor(QPalette::Active, QPalette::Window, selected ? option.palette.highlight().color() : option.palette.base().color());
		palette.setColor(QPalette::Active, QPalette::WindowText, selected ? option.palette.highlightedText().color() : option.palette.text().color());
		Styler->SetText(editText);
		Styler->setPalette(palette);
		Styler->SetChecked(checked);
		Styler->setFixedSize(option.rect.size());	
		Styler->render(&pixmap);
		QPixmapCache::insert(cacheKey, pixmap);
	}
	painter->drawPixmap(option.rect, pixmap);
}


QSize CheckAndEditDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const {
	Styler->SetText(index.model()->data(index, Qt::DisplayRole).toString());
	Styler->setPalette(option.palette);
	Styler->SetChecked(index.model()->data(index, Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked);
	return Styler->sizeHint();
}
QWidget *CheckAndEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex&index) const {
	auto CurrEdit = new CheckAndEdit(parent);
	CurrEdit->setFixedSize(option.rect.size());
	CurrEdit->SetReadOnly(index.model()->data(index, Qt::UserRole + 24).toBool());
    connect(CurrEdit, &CheckAndEdit::CheckChanged, this, &CheckAndEditDelegate::CommitData);
	return CurrEdit;
}


void CheckAndEditDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const {
	auto CurrEdit = qobject_cast<CheckAndEdit*>(editor);
	CurrEdit->SetText(index.model()->data(index, Qt::DisplayRole).toString());
	CurrEdit->SetCheckState(static_cast<Qt::CheckState>(index.model()->data(index, Qt::UserRole + Qt::CheckStateRole).toInt()));
}


void CheckAndEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	auto CurrEdit = qobject_cast<CheckAndEdit*>(editor);
	auto CurrCheck = CurrEdit->CheckState();
	QString CurrTxt = CurrEdit->GetText();
	if (index.isValid()) {
		model->setData(index, CurrCheck, Qt::UserRole + Qt::CheckStateRole);
		model->setData(index, CurrTxt, Qt::EditRole);
	}
}

CheckAndEditDelegate::~CheckAndEditDelegate() {
	Styler->deleteLater();
}

void CheckAndEditDelegate::CommitData() {
	emit commitData(qobject_cast<CheckAndEdit*>(sender()));
}
