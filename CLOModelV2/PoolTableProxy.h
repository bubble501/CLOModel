#ifndef PoolTableProxy_h__
#define PoolTableProxy_h__
#include <QSortFilterProxyModel>
class PoolTableProxy : public QSortFilterProxyModel {
public:
	PoolTableProxy(QObject* parent = 0) :QSortFilterProxyModel(parent) {}
	virtual Qt::ItemFlags flags(const QModelIndex & index) const override {
		if (index.column() < 2) return Qt::ItemIsSelectable | Qt::ItemNeverHasChildren | Qt::ItemIsEnabled;
		return QSortFilterProxyModel::flags(index) | Qt::ItemNeverHasChildren;
	}
};
#endif // PoolTableProxy_h__