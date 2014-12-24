#include <QStyledItemDelegate>
class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class CheckAndEdit;
class CheckAndEditDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:
	CheckAndEditDelegate(QObject *parent = 0);
	virtual ~CheckAndEditDelegate();
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
private slots:
	void CommitData();
private:
	CheckAndEdit* Styler;
};