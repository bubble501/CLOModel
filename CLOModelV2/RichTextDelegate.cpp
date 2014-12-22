#include "RichTextDelegate.h"
#include <QModelIndex>
#include <QPainter>
#include <QPixmapCache>
RichTextDelegate::RichTextDelegate(QObject *parent)
	: QStyledItemDelegate(parent) {
	label = new QLabel;
	label->setTextFormat(Qt::RichText);
	label->setWordWrap(false);
}
void RichTextDelegate::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
	bool selected = option.state & QStyle::State_Selected;
	QPalette palette(option.palette);
	palette.setColor(QPalette::Active, QPalette::Window,
		selected ? option.palette.highlight().color()
		: option.palette.base().color());
	palette.setColor(QPalette::Active, QPalette::WindowText,
		selected
		? option.palette.highlightedText().color()
		: option.palette.text().color());
	QRect labelRect(option.rect.x(),option.rect.y(), option.rect.width(),option.rect.height());
	labelRect.adjust(LabelPadding, LabelPadding, -(2 * LabelPadding), -(2 * LabelPadding));
	label->setPalette(palette);
	label->setFixedSize(qMax(0, labelRect.width()),labelRect.height());
	QString html = index.model()->data(index, Qt::DisplayRole).toString();
	if (index.model()->data(index, Qt::UserRole).toInt()==Erased) {
		html.prepend("<font color='red'><s>");
		html.append("</s></font>");
	}
	else if (index.model()->data(index, Qt::UserRole).toInt() == Added) {
		html.prepend("<font color='green'>");
		html.append("</font>");
	}
	else if (index.model()->data(index, Qt::UserRole).toInt() == Dirty) {
		html.prepend("<font color='blue'>");
		html.append("*</font>");
	}
	label->setText(html);

	QString labelKey = QString("LABEL:%1.%2.%3x%4").arg(selected)
		.arg(html).arg(labelRect.width()).arg(labelRect.height());
	paintWidget(painter, labelRect, labelKey, label);
}
QSize RichTextDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const {
	QString html = index.model()->data(index, Qt::DisplayRole)
		.toString();
	document.setDefaultFont(option.font);
	document.setHtml(html);
	return QSize(document.idealWidth(), option.fontMetrics.height());
}

void RichTextDelegate::paintWidget(QPainter *painter,const QRect &rect, const QString &cacheKey,QWidget *widget) const {
	QPixmap pixmap(widget->size());
	if (!QPixmapCache::find(cacheKey, &pixmap)) {
		widget->render(&pixmap);
		QPixmapCache::insert(cacheKey, pixmap);
	}
	painter->drawPixmap(rect, pixmap);
}