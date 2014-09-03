#ifndef NO_BLOOMBERG
#ifndef BbgResultsIterator_h__
#define BbgResultsIterator_h__
#include "QProjectGlobals.h"
#include "QSingleBbgResult.h"
#include <QMap>
namespace QBloombergLib {
	class QBbgWorker;
	class BLOOMBERG_EXPORT QBbgResultsIterator {
	public:
		QBbgResultsIterator(const QBbgResultsIterator& a);
		QBbgResultsIterator& operator=(const QBbgResultsIterator& a);
		QBbgResultsIterator& operator++();
		QBbgResultsIterator& operator--();
		const QBbgResultsIterator operator++(int);
		const QBbgResultsIterator operator--(int);
		bool IsValid() const;
		qint64 ResultID() const;
		const QSingleBbgResult* operator*() const;
		const QSingleBbgResult* operator->() const;
#ifdef QBBG_PRIVATE
	private:
		const QMap<qint64, QSingleBbgResult* >  * m_Source;
		quint32 m_Iterator;
		QBbgResultsIterator(const QMap<qint64, QSingleBbgResult* >& a) : m_Iterator(0), m_Source(&a) {}
		QBbgResultsIterator& operator=(const QMap<qint64, QSingleBbgResult* >& a) { m_Iterator = 0; m_Source = &a; return *this; }
		friend class QBbgWorker;
#endif 
	};
}
#endif // BbgResultsIterator_h__
#endif