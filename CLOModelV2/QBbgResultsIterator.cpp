#ifndef NO_BLOOMBERG
#include "QBbgResultsIterator.h"
using namespace QBloombergLib;
bool QBbgResultsIterator::IsValid() const {
	if (m_Source==NULL) return false;
	QMap<qint64, QSingleBbgResult* >::const_iterator i = m_Source->constBegin();
	for (quint32 j = 0; j < m_Iterator; j++) {
		++i;
		if (i == m_Source->end()) return false;
	}
	return true;
}
qint64 QBbgResultsIterator::ResultID() const {
	if (m_Source == NULL) return -1i64;
	QMap<qint64, QSingleBbgResult* >::const_iterator i = m_Source->constBegin();
	for (quint32 j = 0; j < m_Iterator; j++) {
		++i;
		if (i == m_Source->end()) return -1i64;
	}
	return i.key();
}
const QSingleBbgResult* QBbgResultsIterator::operator*() const {
	if (m_Source == NULL) return NULL;
	QMap<qint64, QSingleBbgResult* >::const_iterator i = m_Source->constBegin();
	for (quint32 j = 0; j < m_Iterator; j++) {
		++i;
		if (i == m_Source->end()) return NULL;
	}
	return i.value();
}
const QSingleBbgResult* QBbgResultsIterator::operator->() const {
	if (m_Source == NULL) return NULL;
	QMap<qint64, QSingleBbgResult* >::const_iterator i = m_Source->constBegin();
	for (quint32 j = 0; j < m_Iterator; j++) {
		++i;
		if (i == m_Source->end()) return NULL;
	}
	return i.value();
}

const QBbgResultsIterator QBbgResultsIterator::operator++(int) {
	QBbgResultsIterator Result(*this);  ++m_Iterator;  return Result;
}

QBbgResultsIterator& QBbgResultsIterator::operator++() {
	m_Iterator++; return *this;
}

const QBbgResultsIterator QBbgResultsIterator::operator--(int) {
	QBbgResultsIterator Result(*this);  --m_Iterator;  return Result;
}

QBbgResultsIterator& QBbgResultsIterator::operator--() {
	m_Iterator--; return *this;
}

QBbgResultsIterator::QBbgResultsIterator(const QBbgResultsIterator& a) : m_Iterator(a.m_Iterator), m_Source(a.m_Source) {

}

QBbgResultsIterator& QBbgResultsIterator::operator=(const QBbgResultsIterator& a){
	m_Iterator = a.m_Iterator;
	m_Source = a.m_Source;
	return *this;
}
#endif