/*
Copyright (c) 2009-10 Qtrac Ltd. All rights reserved.

This program or module is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version. It is provided
for educational purposes and is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
the GNU General Public License for more details.
*/
#ifndef RichTextDelegate_h__
#define RichTextDelegate_h__

#include <QLabel>
#include <QStyledItemDelegate>
#include <QTextDocument>
class QModelIndex;
class QPainter;
class QStyleOptionViewItem;


class RichTextDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:
	enum DocDirtyEnum { NotDirty=0,Dirty = 1, Erased = 2, Added = 3 };
	explicit RichTextDelegate(QObject *parent = 0);
	~RichTextDelegate() { label->deleteLater(); }

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
private:
	
	enum {LabelPadding=1};
	void paintWidget(QPainter *painter, const QRect &rect,const QString &cacheKey, QWidget *widget) const;
	QLabel *label;
	mutable QTextDocument document;
};
#endif // RichTextDelegate_h__