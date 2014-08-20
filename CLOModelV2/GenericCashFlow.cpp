#include "GenericCashFlow.h"
#include <QDataStream>
GenericCashFlow::GenericCashFlow(): m_AggregationLevel(NoAggregation) {}
GenericCashFlow::GenericCashFlow(const GenericCashFlow& a) 
	: m_AggregationLevel(a.m_AggregationLevel) 
{
	AddFlow(a);
}

GenericCashFlow::~GenericCashFlow() {
	Clear();
}
void GenericCashFlow::AddFlow(const QDate& Dte, double Amt, qint32 FlowTpe) {
	if (Dte.isNull()) return;
	if (qAbs(Amt) < 0.01) Amt = 0.0;

	if (!m_UsedFlowsTypes.contains(FlowTpe)) {
		for (QMap<QDate, QHash<qint32, double>* >::iterator i = m_CashFlows.begin(); i != m_CashFlows.end(); i++) {
			i.value()->operator[](FlowTpe) = 0.0;
		}
		m_UsedFlowsTypes.insert(FlowTpe);
	}
	QMap<QDate, QHash<qint32, double>* >::const_iterator index = m_CashFlows.constBegin();
	for (; index != m_CashFlows.constEnd(); ++index) {
		if (SamePeriod(Dte, index.key(), m_AggregationLevel)) break;
	}
	if (index != m_CashFlows.constEnd()) {
		m_CashFlows[index.key()]->operator[](FlowTpe) += Amt;
	}
	else {
		m_CashFlows.insert(Dte, new QHash<qint32, double>());
		foreach(qint32 SinglFlowType, m_UsedFlowsTypes) {
			m_CashFlows[Dte]->operator[](SinglFlowType) = 0.0;
		}
		m_CashFlows[Dte]->operator[](FlowTpe) += Amt;
	}
}

void GenericCashFlow::AddFlow(const GenericCashFlow& a) {
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = a.m_CashFlows.constBegin(); i != a.m_CashFlows.constEnd(); i++) {
		QHash<qint32, double>* const & TempHash = i.value();
		for (QHash<qint32, double>::const_iterator j = TempHash->constBegin(); j != TempHash->constEnd(); j++) {
			AddFlow(i.key(), j.value(), j.key());
		}
	}
}

void GenericCashFlow::Clear() {
	for (QMap<QDate, QHash<qint32, double>* >::iterator i = m_CashFlows.begin(); i != m_CashFlows.end(); i++) {
		delete (i.value());
	}
	m_CashFlows.clear();
	m_UsedFlowsTypes.clear();
}

QDate GenericCashFlow::GetDate(int index) const {
	if (index<0 || index>=m_CashFlows.size()) return QDate();
	return (m_CashFlows.constBegin() + index).key();
}

double GenericCashFlow::GetFlow(const QDate& index, qint32 FlowTpe) const {
	if (!m_CashFlows.contains(index)) return 0.0;
	if (!m_CashFlows.value(index)->contains(FlowTpe)) return 0.0;
	return m_CashFlows.value(index)->value(FlowTpe);
}

double GenericCashFlow::GetFlow(int index, qint32 FlowTpe) const {
	if (index<0 || index>=m_CashFlows.size()) return 0.0;
	if (!(m_CashFlows.constBegin() + index).value()->contains(FlowTpe))  return 0.0;
	return (m_CashFlows.constBegin() + index).value()->value(FlowTpe);
}
double GenericCashFlow::GetPreviousFlow(int index, qint32 FlowTpe) const {
	if (m_CashFlows.isEmpty() || index<0) return 0.0;
	const QHash<qint32, double>* TempFlows;
	if (index<1 || index>=m_CashFlows.size())  TempFlows = (m_CashFlows.constEnd() - 1).value();
	else  TempFlows = (m_CashFlows.constBegin() + index - 1).value();
	if (!TempFlows->contains(FlowTpe))  return 0.0;
	return TempFlows->value(FlowTpe);
}

double GenericCashFlow::GetPreviousFlow(const QDate& index, qint32 FlowTpe) const {
	if (m_CashFlows.isEmpty() || index.isNull()) return 0.0;
	QMap<QDate, QHash<qint32, double>*	>::const_iterator TempFind = m_CashFlows.find(index);
	const QHash<qint32, double>* TempFlows;
	if (TempFind == m_CashFlows.constEnd() || TempFind == m_CashFlows.constBegin())  TempFlows = (m_CashFlows.constEnd() - 1).value();
	else TempFlows = (--TempFind).value();
	if (!TempFlows->contains(FlowTpe))  return 0.0;
	return TempFlows->value(FlowTpe);
}

int GenericCashFlow::GetPaymentFrequency() const {
	if (m_CashFlows.size() > 1)
		return qAbs(MonthDiff((m_CashFlows.end() - 1).key(), (m_CashFlows.end() - 2).key()));
	else
		return 0;
}

int GenericCashFlow::FindDate(const QDate& a) const {
	QMap<QDate, QHash<qint32, double>*	>::const_iterator TempFind = m_CashFlows.find(a);
	if (TempFind == m_CashFlows.constEnd()) return -1;
	int Result = 0;
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = m_CashFlows.constBegin(); i != TempFind; ++i) ++Result;
	return Result;
}

void GenericCashFlow::ReplaceDate(const QDate& OriginalDate, const QDate& NewDate) {
	QMap<QDate, QHash<qint32, double>* >::iterator TempIter = m_CashFlows.find(OriginalDate);
	if (TempIter != m_CashFlows.end()) {
		QHash<qint32, double>* TempPoint = TempIter.value();
		m_CashFlows.erase(TempIter);
		m_CashFlows.insert(NewDate, TempPoint);
	}
}

void GenericCashFlow::Aggregate(CashFlowAggregation Freq) {
	m_AggregationLevel = Freq;
	if (Freq == NoAggregation) return;
	for (QMap<QDate, QHash<qint32, double>* >::iterator MainIter = m_CashFlows.begin(); MainIter != m_CashFlows.end(); ++MainIter) {
		for (QMap<QDate, QHash<qint32, double>* >::iterator SecondIter = MainIter + 1; SecondIter != m_CashFlows.end();) {
			if (SamePeriod(MainIter.key(), SecondIter.key(),Freq)) {
				QHash<qint32, double>* TempMain = MainIter.value();
				const QHash<qint32, double>* TempSecond = SecondIter.value();
				for (QHash<qint32, double>::const_iterator i = TempSecond->constBegin(); i != TempSecond->constEnd(); ++i) {
					if (!TempMain->contains(i.key())) TempMain->insert(i.key(), 0.0); //Should never happen but just to be safe
					TempMain->operator[](i.key()) += i.value();
				}
				delete (SecondIter.value());
				SecondIter = m_CashFlows.erase(SecondIter);
			}
			else break;
		}
	}
}

bool GenericCashFlow::SamePeriod(const QDate& a, const QDate& b, CashFlowAggregation Freq) {
	int YearA, YearB;
	bool Result;
	switch (Freq) {
	case TotalAggragate:
		return true;
	case Annually:
		return a.year() == b.year();
	case SemiAnnually:
		return a.year() == b.year() && (a.month()-1) / 6 == (b.month()-1) / 6;
	case Quarterly:
		return a.year() == b.year() && (a.month() - 1) / 3 == (b.month() - 1) / 3;
	case Monthly:
		return a.year() == b.year() && a.month() == b.month();
	case Weekly:
		Result = a.weekNumber(&YearA) && b.weekNumber(&YearB);
		return Result && YearA == YearB;
	case NoAggregation:
	default:
		return false;
	}
}
QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows) {
	stream << flows.m_UsedFlowsTypes;
	stream << quint32(flows.m_CashFlows.size());
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator MainIter = flows.m_CashFlows.constBegin(); MainIter != flows.m_CashFlows.constEnd(); ++MainIter) {
		const QHash<qint32, double>* TempMain = MainIter.value();
		stream << MainIter.key() << (*TempMain);
	}
	return stream;
}
QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows) { return flows.LoadOldVersion(stream); }
QDataStream& GenericCashFlow::LoadOldVersion(QDataStream& stream) {
	quint32 TempSize;
	QDate TempDate;
	QHash<qint32, double> TempMain;
	stream >> m_UsedFlowsTypes >> TempSize;
	for (quint32 i = 0; i < TempSize; i++) {
		stream >> TempDate >> TempMain;
		m_CashFlows.insert(TempDate, new QHash<qint32, double>(TempMain));
		TempMain.clear();
	}
	ResetProtocolVersion();
	return stream;
}

GenericCashFlow& GenericCashFlow::operator=(const GenericCashFlow& a) {
	Clear(); m_AggregationLevel = a.m_AggregationLevel;  AddFlow(a); return *this;
}

QDate GenericCashFlow::MaturityDate() const {
	if (m_CashFlows.isEmpty()) return QDate(); return (m_CashFlows.end() - 1).key();
}

bool GenericCashFlow::operator==(const GenericCashFlow& a) const {
	if (m_AggregationLevel != a.m_AggregationLevel) return false;
	if (m_CashFlows.size() != a.m_CashFlows.size()) return false;
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator MainIter = m_CashFlows.constBegin(); MainIter != m_CashFlows.constEnd(); ++MainIter) {
		if (!a.m_CashFlows.contains(MainIter.key())) return false;
		const QHash<qint32, double>* TempMain = MainIter.value();
		const QHash<qint32, double>* TempSec = a.m_CashFlows.value(MainIter.key());
		if (TempMain->size() != TempSec->size()) return false;
		for (QHash<qint32, double>::const_iterator SecIter = TempMain->constBegin(); SecIter != TempMain->constEnd(); ++SecIter) {
			if (!TempSec->contains(SecIter.key())) return false;
			if (qAbs(TempSec->value(SecIter.key()) - SecIter.value())>=0.01) return false;
		}
	}
	return true;
}
