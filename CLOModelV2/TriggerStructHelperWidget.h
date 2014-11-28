#ifndef TRIGGERSTRUCTHELPERWIDGET_H
#define TRIGGERSTRUCTHELPERWIDGET_H

#include <QWidget>
#include "AbstractTrigger.h"
#include <QSharedPointer>
#include <QList>
class TriggerStructHelperWidget : public QWidget
{
	Q_OBJECT

public:
	TriggerStructHelperWidget(const QList<QSharedPointer<AbstractTrigger> >& AvailableTriggers, QWidget *parent = 0);

private:
	
};

#endif // TRIGGERSTRUCTHELPERWIDGET_H
