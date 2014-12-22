#include <QItemDelegate>
#include <QMap>
#include "LoanAssumption.h"
class AssumptionsComboDelegate : public QItemDelegate {
	Q_OBJECT
public:
	AssumptionsComboDelegate(QObject *parent = 0);
	QString ComboElementforIndex(const QModelIndex &index);
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
signals:
	void Edited();
private:
	QMap<qint16, QString> ComboElements;
};