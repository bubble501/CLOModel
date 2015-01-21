#ifndef PoolTableProxy_h__
#define PoolTableProxy_h__
#include <QSortFilterProxyModel>
#include <QSet>
class ReadOnlyColProxy : public QSortFilterProxyModel {
public:
	void SetReadOnlyCol(qint32 index) { m_ReadOnlyColums.insert(index); }
	void SetReadOnlyCol(QList<qint32> indexes) { foreach(qint32 index, indexes) { SetReadOnlyCol(index); } }
	void RemoveReadOnlyCol(qint32 index) { m_ReadOnlyColums.remove(index); }
	void ClearReadOnly() { m_ReadOnlyColums.clear(); }
	ReadOnlyColProxy(QObject* parent = 0) :QSortFilterProxyModel(parent) {}
	virtual Qt::ItemFlags flags(const QModelIndex & index) const override {
		if (m_ReadOnlyColums.contains(index.column())) return QSortFilterProxyModel::flags(index) & ~Qt::ItemIsEditable;
		return QSortFilterProxyModel::flags(index);
	}
protected:
	QSet<qint32> m_ReadOnlyColums;
};
#endif // PoolTableProxy_h__