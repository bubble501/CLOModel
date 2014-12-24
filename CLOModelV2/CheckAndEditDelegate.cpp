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
	QPalette palette(option.palette);
	palette.setColor(QPalette::Active, QPalette::Window,selected ? option.palette.highlight().color(): option.palette.base().color());
	palette.setColor(QPalette::Active, QPalette::WindowText,selected? option.palette.highlightedText().color(): option.palette.text().color());

	Styler->SetText(index.model()->data(index, Qt::DisplayRole).toString());
	Styler->setPalette(palette);
	Styler->SetChecked(index.model()->data(index,Qt::UserRole+ Qt::CheckStateRole).toInt() == Qt::Checked);
	Styler->setFixedSize(option.rect.size());
	QPixmap pixmap(option.rect.size());
	QString cacheKey = QString("CHECKANDEDIT:%1;%2;%3;%4;%5").arg(selected).arg(Styler->IsChecked()).arg(Styler->GetText()).arg(option.rect.width()).arg(option.rect.height());
	if (!QPixmapCache::find(cacheKey, &pixmap)) {
		Styler->render(&pixmap);
		QPixmapCache::insert(cacheKey, pixmap);
	}
	painter->drawPixmap(option.rect, pixmap);


	/*QRect checkboxRect(option.rect.x(), option.rect.y() + (checkbox->height() < option.rect.height() ? (option.rect.height() - checkbox->height()) / 2 : 0), checkbox->width(), checkbox->height());
	checkbox->setPalette(palette);
	bool checked = index.model()->data(index, Qt::CheckStateRole).toInt() == Qt::Checked;
	checkbox->setChecked(checked);

	QRect labelRect(option.rect.x() + checkbox->width(),option.rect.y(), option.rect.width() - checkbox->width(),option.rect.height());
	label->setPalette(palette);
	label->setFixedSize(qMax(0, labelRect.width()),labelRect.height());
	QString html = index.model()->data(index, Qt::DisplayRole).toString();
	label->setText(html);

	QString checkboxKey = QString("CHECKBOX:%1.%2").arg(selected).arg(checked);
	paintWidget(painter, checkboxRect, checkboxKey, checkbox);
	QString labelKey = QString("LABEL:%1.%2.%3x%4").arg(selected).arg(html).arg(labelRect.width()).arg(labelRect.height());
	paintWidget(painter, labelRect, labelKey, label);*/
}


QSize CheckAndEditDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const {
	QPalette palette(option.palette);
	bool selected = option.state & QStyle::State_Selected;
	palette.setColor(QPalette::Active, QPalette::Window, selected ? option.palette.highlight().color() : option.palette.base().color());
	palette.setColor(QPalette::Active, QPalette::WindowText, selected ? option.palette.highlightedText().color() : option.palette.text().color());
	Styler->SetText(index.model()->data(index, Qt::DisplayRole).toString());
	Styler->setPalette(palette);
	Styler->SetChecked(index.model()->data(index, Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked);
	auto testa = Styler->sizeHint();
	return Styler->sizeHint();
}
QWidget *CheckAndEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex&index) const {
	auto CurrEdit = new CheckAndEdit(parent);
	CurrEdit->setFixedSize(option.rect.size());
	CurrEdit->SetReadOnly(index.model()->data(index, Qt::UserRole + 24).toBool());
	connect(CurrEdit, SIGNAL(CheckChanged(int)), this, SLOT(CommitData()));
	return CurrEdit;
}


void CheckAndEditDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const {
	auto CurrEdit = qobject_cast<CheckAndEdit*>(editor);
	CurrEdit->SetText(index.model()->data(index, Qt::DisplayRole).toString());
	CurrEdit->SetChecked(index.model()->data(index, Qt::UserRole + Qt::CheckStateRole).toInt() == Qt::Checked);
}


void CheckAndEditDelegate::setModelData(QWidget *editor,QAbstractItemModel *model, const QModelIndex &index) const {
	auto CurrEdit = qobject_cast<CheckAndEdit*>(editor);
	model->setData(index, CurrEdit->GetText(),Qt::EditRole);
	model->setData(index, CurrEdit->IsChecked() ? Qt::Checked : Qt::Unchecked, Qt::UserRole + Qt::CheckStateRole);
}

CheckAndEditDelegate::~CheckAndEditDelegate() {
	Styler->deleteLater();
}

void CheckAndEditDelegate::CommitData() {
	emit commitData(qobject_cast<CheckAndEdit*>(sender()));
}
