
#ifndef LoanAssumptionDelegate_h__
#define LoanAssumptionDelegate_h__

#include <QItemDelegate>
class LoanAssumptionDelegate : public QItemDelegate {
	Q_OBJECT
public:
	enum class AssumptionType : qint8 { 
		IntegerAssumption
		, DoubleAssumption 
		, DayCountVectorAssumption
		, BloombergVectorAssumption
		, IntegerVectorAssumption
		, DoubleAssumption0To100
	};
	LoanAssumptionDelegate(QObject *parent = 0);
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
signals:
	void Edited();
};
#endif // LoanAssumptionDelegate_h__



