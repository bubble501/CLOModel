#ifndef TRIGGERSTRUCTHELPERWIDGET_H
#define TRIGGERSTRUCTHELPERWIDGET_H
#include <QWidget>
#include <memory>
#include <QHash>
#include "clomodellib_global.h"
class AbstractTrigger;
class TriggerStructHelperWidgetPrivate;
class CLOMODELLIB_EXPORT TriggerStructHelperWidget : public QWidget
{
	Q_OBJECT
protected:
    TriggerStructHelperWidgetPrivate* d_ptr;
    Q_DECLARE_PRIVATE(TriggerStructHelperWidget)
    TriggerStructHelperWidget(TriggerStructHelperWidgetPrivate* d, QWidget *parent = nullptr);
public:
    virtual ~TriggerStructHelperWidget();
    TriggerStructHelperWidget(QWidget *parent = nullptr);
    TriggerStructHelperWidget(const QHash<quint32, std::shared_ptr<AbstractTrigger> >& AvailableTriggers, QWidget *parent = nullptr);
	QString GetResult() const;
public slots:
    void SetAvailableTriggers(const QHash<quint32, std::shared_ptr<AbstractTrigger> >& AvailableTriggers);
	void SetCurrentStructure(const QString& a);
protected slots :
	void Negate();
	void InsertOperator();
	void OpenParenthesis();
	void CloseParenthesis();
	void InsertTrigger(const QModelIndex & index);
	void Del();
	void Clear();
	void DecriptTriggers(QString encr);
};

#endif // TRIGGERSTRUCTHELPERWIDGET_H
