#ifndef TRIGGERSTRUCTHELPERWIDGET_H
#define TRIGGERSTRUCTHELPERWIDGET_H
#include <QWidget>
#include "AbstractTrigger.h"
#include <QSharedPointer>
#include <QHash>
class QStandardItemModel;
class QLineEdit;
class QTableView;
class QPushButton;
class QModelIndex;
class TriggerStructHelperWidget : public QWidget
{
	Q_OBJECT

public:
	TriggerStructHelperWidget(const QHash<quint32, QSharedPointer<AbstractTrigger> >& AvailableTriggers = QHash<quint32, QSharedPointer<AbstractTrigger> >(), QWidget *parent = 0);
	QString GetResult() const;
public slots:
	void SetAvailableTriggers(const QHash<quint32, QSharedPointer<AbstractTrigger> >& AvailableTriggers);
	void SetCurrentStructure(const QString& a);
private:
	QLineEdit* EncriptedTriggers;
	QLineEdit* DecriptedTriggers;
	QStandardItemModel* TriggersModel;
	QTableView *TriggerTable;
	QPushButton* AndButton;
	QPushButton* OrButton;
	QPushButton* NandButton;
	QPushButton* NorButton;
	QPushButton* OpenParButton;
	QPushButton* CloseParButton;
	QPushButton* DelButton;
	QPushButton* ClearButton;
	QPushButton* NegateButton;
	void EnableOperators(bool a = true);
private slots:
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
