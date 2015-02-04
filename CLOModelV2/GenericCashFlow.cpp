#include "GenericCashFlow.h"
#include <QDataStream>
#include <QSet>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QStack>
#include <algorithm>
#include <functional>  
GenericCashFlow::GenericCashFlow() 
	: m_AggregationLevel(NoAggregation)
	, m_AdjustHolidays(false)
{}
GenericCashFlow::GenericCashFlow(const GenericCashFlow& a)
	: m_AggregationLevel(a.m_AggregationLevel) 
	, m_AdjustHolidays(a.m_AdjustHolidays)
	, m_CashFlowLabels(a.m_CashFlowLabels)
	, m_Stocks(a.m_Stocks)
{
	SetFlow(a);
}

GenericCashFlow::~GenericCashFlow() {
	Clear();
}
void GenericCashFlow::AddFlow(QDate Dte, double Amt, qint32 FlowTpe) {
	if (Dte.isNull()) return;
	if (qAbs(Amt) < 0.01) Amt = 0.0;
	if (m_AdjustHolidays) { while (IsHoliday(Dte)) Dte=Dte.addDays(1); }
	QMap<QDate, QHash<qint32, double>* >::iterator index = m_CashFlows.begin();
	for (; index != m_CashFlows.end(); ++index) {
		if (SamePeriod(Dte, index.key(), m_AggregationLevel)) break;
	}
	if (index != m_CashFlows.end()) {
		//if (Amt== 0.0) return;
		Amt += GetFlow(index.key(), FlowTpe);
		if (qAbs(Amt) < 0.01) Amt = 0.0;
		if (index.value()->contains(FlowTpe)) {
			if (Amt == 0.0 && !m_Stocks.contains(FlowTpe)) index.value()->remove(FlowTpe);
			else index.value()->operator[](FlowTpe) = Amt;
		}
		else {
			if (qAbs(Amt) > 0.0 || m_Stocks.contains(FlowTpe)) index.value()->insert(FlowTpe, Amt);
		}
	}
	else {
		m_CashFlows.insert(Dte, new QHash<qint32, double>());
		//if (qAbs(Amt) > 0.0) {
			Amt += GetFlow(Dte, FlowTpe);
			if (qAbs(Amt) < 0.01) Amt = 0.0;
			if (qAbs(Amt) > 0.0 || m_Stocks.contains(FlowTpe)) m_CashFlows[Dte]->insert(FlowTpe, Amt);
		//}
	}
}

void GenericCashFlow::SetFlow(QDate Dte, double Amt, qint32 FlowTpe) {
	if (Dte.isNull()) return;
	if (m_AdjustHolidays) { while (IsHoliday(Dte)) Dte = Dte.addDays(1); }
	if (qAbs(Amt) < 0.01) Amt = 0.0;
	QMap<QDate, QHash<qint32, double>* >::iterator index = m_CashFlows.begin();
	for (; index != m_CashFlows.end(); ++index) {
		if (SamePeriod(Dte, index.key(), m_AggregationLevel)) break;
	}
	if (index != m_CashFlows.end()) {
		if (index.value()->contains(FlowTpe)) {
			if (Amt == 0.0 && !m_Stocks.contains(FlowTpe)) index.value()->remove(FlowTpe);
			else {
				index.value()->operator[](FlowTpe) = Amt;
			}
		}
		else {
			if (qAbs(Amt) > 0.0 || m_Stocks.contains(FlowTpe)) {
				index.value()->insert(FlowTpe, Amt);
			}
		}
	}
	else {
		m_CashFlows.insert(Dte, new QHash<qint32, double>());
		if (qAbs(Amt) > 0.0 || m_Stocks.contains(FlowTpe)) {
			m_CashFlows[Dte]->insert(FlowTpe, Amt);
		}
	}
}


void GenericCashFlow::AddFlow(const GenericCashFlow& a) {
	QSet<qint32> StocksToCalculate;
	QHash<qint32,QSet<QDate> >DatesOfStockChange;
	m_Stocks.unite(a.m_Stocks);
	qint32 FakeFlow = 0;
	while (m_Stocks.contains(FakeFlow)) ++FakeFlow;

	//Aggregate Flows
	for (auto i = a.m_CashFlows.constBegin(); i != a.m_CashFlows.constEnd(); ++i) {
		if (i.value()->isEmpty()) AddFlow(i.key(), 0.0, FakeFlow);
		else {
			for (QHash<qint32, double>::const_iterator j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
				if (m_Stocks.contains(j.key())) {
					//Save stocks types to calculate later
					AddFlow(i.key(), 0.0, FakeFlow);
					StocksToCalculate.insert(j.key());
					DatesOfStockChange[j.key()].insert(i.key());
				}
				else AddFlow(i.key(), j.value(), j.key());
			}
		}
	}
	for (auto i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		for (QHash<qint32, double>::const_iterator j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
			if (m_Stocks.contains(j.key())) {
				//Add stocks types already in cash flows
				StocksToCalculate.insert(j.key());
				DatesOfStockChange[j.key()].insert(i.key());
			}
		}
	}
	//Aggregate Stocks
	foreach(qint32 SingleStock, StocksToCalculate) {
		auto CurrDates = DatesOfStockChange.value(SingleStock, QSet<QDate>()).toList();
		std::sort(CurrDates.begin(), CurrDates.end(), std::greater<QDate>());
		for (auto i = CurrDates.constBegin(); i != CurrDates.constEnd(); ++i) {
			SetFlow(*i, GetFlow(*i, SingleStock) + a.GetFlow(*i, SingleStock), SingleStock);
		}
	}
	//Aggregate Labels
	for (auto i = a.m_CashFlowLabels.constBegin(); i != a.m_CashFlowLabels.constEnd(); ++i) {
		if (!m_CashFlowLabels.contains(i.key())) m_CashFlowLabels.insert(i.key(), i.value());
	}
}
void GenericCashFlow::SetFlow(const GenericCashFlow& a) {
	Clear();
	m_Stocks = a.m_Stocks;
	m_CashFlowLabels = a.m_CashFlowLabels;
	if (!a.m_CashFlows.isEmpty()) {
		for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = a.m_CashFlows.constEnd()-1;true; --i) {
			if (i.value()->isEmpty()) SetFlow(i.key(), 0.0, 0);
			for (QHash<qint32, double>::const_iterator j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
				SetFlow(i.key(), j.value(), j.key());
			}
			if (i == a.m_CashFlows.constBegin()) break;
		}
	}
}

void GenericCashFlow::Clear() {
	for (QMap<QDate, QHash<qint32, double>* >::iterator i = m_CashFlows.begin(); i != m_CashFlows.end(); i++) {
		delete (i.value());
	}
	m_CashFlows.clear();
}

QDate GenericCashFlow::GetDate(int index) const {
	if (index < 0 || index >= m_CashFlows.size()) return QDate();
	return (m_CashFlows.constBegin() + index).key();
}

double GenericCashFlow::GetFlow(const QDate& index, qint32 FlowTpe) const {
	if (index.isNull()) return 0.0;
	if (m_Stocks.contains(FlowTpe)) {
		QMap<QDate, QHash<qint32, double>*	>::const_iterator FoundIt = m_CashFlows.constFind(index);
		if (FoundIt == m_CashFlows.constEnd()) {
			FoundIt = m_CashFlows.lowerBound(index);
			if (FoundIt == m_CashFlows.constBegin()) return 0.0;
			--FoundIt;
		}
		for (; true; --FoundIt) {
			if (FoundIt.value()->contains(FlowTpe))
				return FoundIt.value()->value(FlowTpe);
			if (FoundIt == m_CashFlows.constBegin()) break;
		}
		return 0.0;
	}
	if (!m_CashFlows.contains(index)) return 0.0;
	return m_CashFlows.value(index)->value(FlowTpe,0.0);
}

double GenericCashFlow::GetFlow(int index, qint32 FlowTpe) const {
	return GetFlow(GetDate(index), FlowTpe);
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
			if (SamePeriod(MainIter.key(), SecondIter.key(), Freq)) {
				QHash<qint32, double>* TempMain = MainIter.value();
				const QHash<qint32, double>* TempSecond = SecondIter.value();
				for (QHash<qint32, double>::const_iterator i = TempSecond->constBegin(); i != TempSecond->constEnd(); ++i) {
					if (!TempMain->contains(i.key())) 
						TempMain->insert(i.key(), 0.0); //Should never happen but just to be safe
					if (m_Stocks.contains(i.key())) TempMain->operator[](i.key()) = i.value();
					else TempMain->operator[](i.key()) += i.value();
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
		return a.year() == b.year() && (a.month() - 1) / 6 == (b.month() - 1) / 6;
	case Quarterly:
		return a.year() == b.year() && (a.month() - 1) / 3 == (b.month() - 1) / 3;
	case Monthly:
		return a.year() == b.year() && a.month() == b.month();
	case Weekly:
		Result = a.weekNumber(&YearA) && b.weekNumber(&YearB);
		return Result && YearA == YearB;
	case NoAggregation:
	default:
		return a==b;
	}
}
QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows) {
	stream << flows.m_AdjustHolidays << flows.m_CashFlowLabels << flows.m_Stocks << static_cast<qint32>(flows.m_AggregationLevel) << static_cast<qint32>(flows.m_CashFlows.size());
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
	stream >> m_AdjustHolidays >> m_CashFlowLabels >> m_Stocks >> TempSize;
	m_AggregationLevel = static_cast<CashFlowAggregation>(TempSize);
	stream >> TempSize;
	for (quint32 i = 0; i < TempSize; i++) {
		stream >> TempDate >> TempMain;
		m_CashFlows.insert(TempDate, new QHash<qint32, double>(TempMain));
		TempMain.clear();
	}
	ResetProtocolVersion();
	return stream;
}

GenericCashFlow& GenericCashFlow::operator=(const GenericCashFlow& a) {
	Aggregate(a.m_AggregationLevel);  
	SetFlow(a); 
	return *this;
}

QDate GenericCashFlow::MaturityDate() const {
	if (m_CashFlows.isEmpty()) return QDate();
	for (auto i = m_CashFlows.constEnd() - 1; true; --i) {
		if (!i.value()->isEmpty()) return i.key();
		if (i == m_CashFlows.constBegin()) return QDate();
	}
	return QDate();
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
			if (qAbs(TempSec->value(SecIter.key()) - SecIter.value()) >= 0.01) return false;
		}
	}
	return true;
}
GenericCashFlow GenericCashFlow::operator+(const GenericCashFlow& a) const {
	GenericCashFlow Result(*this); 
	Result.AddFlow(a); 
	return Result;
}


QString GenericCashFlow::ToString() const {
	QSet<qint32> FlowsTypes;
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator MainIter = m_CashFlows.constBegin(); MainIter != m_CashFlows.constEnd(); ++MainIter) {
		FlowsTypes.unite(MainIter.value()->keys().toSet());
	}
	QString Result("Date");
	QList<qint32> FlowsTypesList = FlowsTypes.toList();
	std::sort(FlowsTypesList.begin(), FlowsTypesList.end());
	for (QList<qint32>::const_iterator SecIter = FlowsTypesList.constBegin(); SecIter != FlowsTypesList.constEnd(); ++SecIter) {
		Result += '\t' + GetLabel(*SecIter, QString("Flow %1").arg(*SecIter));
	}
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator MainIter = m_CashFlows.constBegin(); MainIter != m_CashFlows.constEnd(); ++MainIter) {
		Result += '\n' + MainIter.key().toString("dd-MM-yyyy");
		for (QList<qint32>::const_iterator SecIter = FlowsTypesList.constBegin(); SecIter != FlowsTypesList.constEnd(); ++SecIter) {
			Result += '\t' + QString::number(GetFlow(MainIter.key(), *SecIter), 'f', 2);
		}
	}
	return Result;
}

GenericCashFlow GenericCashFlow::SingleFlow(qint32 FlowTpe) const {
	GenericCashFlow Result;
	Result.Aggregate(m_AggregationLevel);
	if (IsStock(FlowTpe)) Result.SetStock(FlowTpe);
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator MainIter = m_CashFlows.constBegin(); MainIter != m_CashFlows.constEnd(); ++MainIter) {
		Result.SetFlow(MainIter.key(), GetFlow(MainIter.key(),FlowTpe), FlowTpe);
	}
	if (!GetLabel(FlowTpe).isEmpty()) Result.SetLabel(FlowTpe, GetLabel(FlowTpe));
	return Result;
}
GenericCashFlow GenericCashFlow::SingleDate(const QDate& a) const {
	GenericCashFlow Result;
	Result.Aggregate(m_AggregationLevel);
	Result.m_Stocks = m_Stocks;
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator MainIter = m_CashFlows.constBegin(); MainIter != m_CashFlows.constEnd(); ++MainIter) {
		if (SamePeriod(a, MainIter.key(), m_AggregationLevel)) {
			auto AllFlws = AvailableFlows(a);
			for (auto SecondIter = AllFlws.constBegin(); SecondIter != AllFlws.constEnd(); ++SecondIter) {
				Result.SetFlow(a, GetFlow(a, *SecondIter), *SecondIter);
				if (!GetLabel(*SecondIter).isEmpty()) 
					Result.SetLabel(*SecondIter, GetLabel(*SecondIter));
			}
			return Result;
		}
	}
	return Result;
}
bool GenericCashFlow::HasFlowType(qint32 FlowTpe) const {
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator MainIter = m_CashFlows.constBegin(); MainIter != m_CashFlows.constEnd(); ++MainIter) {
		if (MainIter.value()->contains(FlowTpe)){
			if (m_Stocks.contains(FlowTpe)) { 
				if (MainIter.value()->value(FlowTpe) >= 0.01) return true; 
			}
			else return true;
		}
	}
	return false;
}

QList<qint32> GenericCashFlow::AvailableFlows(const QDate& a) const {
	QSet<qint32> Result;
	if (a.isNull()) {
		for (auto i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
			for (auto j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
				Result.insert(j.key());
			}
		}
		return Result.toList();
	}
	
	QMap<QDate, QHash<qint32, double>*	>::const_iterator TempIter = m_CashFlows.constFind(a);
	if (TempIter != m_CashFlows.constEnd())
		Result.unite(TempIter.value()->keys().toSet());
	for (TempIter = m_CashFlows.constBegin(); TempIter != m_CashFlows.constEnd(); ++TempIter) {
		if (TempIter.key() > a) break;
		foreach(qint32 SingleStock, m_Stocks) {
			if (TempIter.value()->contains(SingleStock)) Result.insert(SingleStock);
		}
	}
	return Result.toList();
}

void GenericCashFlow::SetAdjustHolidays(bool val) {
	m_AdjustHolidays = val;
	if (m_AdjustHolidays) {
		for (QMap<QDate, QHash<qint32, double>* >::iterator i = m_CashFlows.begin(); i != m_CashFlows.end(); ++i) {
			while (IsHoliday(i.key())) {
				m_CashFlows.insert(i.key().addDays(1), i.value());
				i = m_CashFlows.erase(i);
			}
		}
	}
}

double GenericCashFlow::GetTotalFlow(const QDate& a, const QList<qint32>& Groups /*= QList<qint32>()*/) const {
	if (a.isNull()) return 0.0;
	QMap<QDate, QHash<qint32, double>*	>::const_iterator TempIter = m_CashFlows.constFind(a);
	if (TempIter == m_CashFlows.constEnd()) return 0.0;
	double Result = 0.0;
	for (QHash<qint32, double>::const_iterator i = TempIter.value()->constBegin(); i != TempIter.value()->constEnd(); ++i) {
		if (Groups.isEmpty() || Groups.contains(i.key())) Result += i.value();
	}
	return Result;
}
double GenericCashFlow::GetTotalFlow(int index, const QList<qint32>& Groups /*= QList<qint32>()*/) const {
	return GetTotalFlow(GetDate(index), Groups);
}
bool GenericCashFlow::IsEmpty() const {
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		if (!i.value()->isEmpty()) return false;
	}
	return true;
}

GenericCashFlow GenericCashFlow::AggregateRange(const QDate& StartDate, const QDate& EndDate) const{
	GenericCashFlow Results;
	Results.m_Stocks = m_Stocks;
	Results.m_CashFlowLabels = m_CashFlowLabels;
	Results.m_AdjustHolidays = m_AdjustHolidays;
	Results.Aggregate(m_AggregationLevel);
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		if (i.key()>EndDate) break;
		if (i.key() >= StartDate) {
			for (QHash<qint32, double>::const_iterator j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
				if (m_Stocks.contains(j.key())) Results.SetFlow(EndDate, j.value(), j.key());
				else Results.AddFlow(EndDate, j.value(), j.key());
			}
		}
	}
	return Results;
}

void GenericCashFlow::RemoveFlow(qint32 FlowTpe) {
	for (auto i = m_CashFlows.begin(); i != m_CashFlows.end(); ++i) {
		i.value()->remove(FlowTpe);
	}
}

GenericCashFlow GenericCashFlow::ScaledCashFlows(double OriginalRefSize, double ResultSize, const QList<qint32>& Groups, const QList<qint32>& ExcludeGroups) const {
	GenericCashFlow Result;
	Result.m_Stocks = m_Stocks;
	Result.m_AdjustHolidays = m_AdjustHolidays;
	Result.m_CashFlowLabels = m_CashFlowLabels;
	Result.Aggregate(m_AggregationLevel);
	if (ResultSize == 0.0 || OriginalRefSize == 0.0) return Result;
	const double ScaleRatio = ResultSize/OriginalRefSize ;
	for (auto i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		auto FlwTpe = this->AvailableFlows(i.key());
		for (auto j = FlwTpe.constBegin(); j != FlwTpe.constEnd(); ++j) {
		//for (auto j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
			if ((Groups.isEmpty() || Groups.contains(*j)) && !ExcludeGroups.contains(*j)) {
				Result.SetFlow(i.key(), GetFlow(i.key(), *j)*ScaleRatio, *j);
			}
			else {
				Result.SetFlow(i.key(), GetFlow(i.key(), *j), *j);
			}
		}
	}
	return Result;
}

QString GenericCashFlow::ToPlainText(bool UseHeaders /*= true*/) const {
	if (IsEmpty()) return false;
	QString Result("Date");
	auto AllFlows = AvailableFlows();
	std::sort(AllFlows.begin(),AllFlows.end());
	for (auto j = AllFlows.constBegin(); j != AllFlows.constEnd(); ++j) {
		if (UseHeaders) Result += '\t' + GetLabel(*j, QString("Flow %1").arg(*j));
		else  Result += QString("%1").arg(*j);
	}
	for (auto i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		Result += '\n' + i.key().toString("yyyy-MM-dd");
		for (auto j = AllFlows.constBegin(); j != AllFlows.constEnd(); ++j) {
			Result += '\t' + QString::number(GetFlow(i.key(), *j), 'f');
		}
	}
	return Result;
}

QString GenericCashFlow::ToXML() const {
	QString Result;
	QXmlStreamWriter Writer(&Result);
	Writer.setAutoFormatting(false);
#ifdef _DEBUG
	Writer.writeStartDocument();
#endif // _DEBUG
	Writer.writeStartElement("CashFlow");
	auto AllFlows = AvailableFlows();
	std::sort(AllFlows.begin(),AllFlows.end());
	for (auto j = AllFlows.constBegin(); j != AllFlows.constEnd(); ++j) {
		Writer.writeStartElement("Flow");
		Writer.writeAttribute("id", QString::number(*j));
		if (!GetLabel(*j).isEmpty()) Writer.writeTextElement("Label", GetLabel(*j));
		else Writer.writeEmptyElement("Label");
		for (auto i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
			Writer.writeStartElement("SingleFlow");
			Writer.writeTextElement("Date", i.key().toString("yyyy-MM-dd"));
			Writer.writeTextElement("Amount", QString::number(GetFlow(i.key(), *j), 'f', 2));
			Writer.writeEndElement();
		}
		Writer.writeEndElement();
	}
	Writer.writeEndElement();
#ifdef _DEBUG
	Writer.writeEndDocument();
#endif // _DEBUG
	return Result;
}

void GenericCashFlow::LoadFromXML(const QString& Source) {
	Clear();
	QScopedPointer<qint32> CurrrentId(nullptr);
	QScopedPointer<QDate> CurrentDate ( nullptr);
	QScopedPointer<double> CurrentAmt ( nullptr);
	QStack<QString> CurrentElement;
	bool NoErrorsOccured = true;
	QXmlStreamReader xml(Source);
	while (!xml.atEnd() && !xml.hasError() && NoErrorsOccured) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "Flow") {
				if (CurrrentId) {
					NoErrorsOccured = false;
					continue;
				}
				CurrrentId.reset(new qint32(xml.attributes().value("id").toInt(&NoErrorsOccured)));
			}
			CurrentElement.push(xml.name().toString());
		}
		else if (xml.isEndElement()) {
			if (xml.name() == "Flow") {
				if (CurrrentId) CurrrentId.reset();
				if (CurrentDate) {
					NoErrorsOccured = false;
					continue;
				}
				if (CurrentAmt) {
					NoErrorsOccured = false;
					continue;
				}
			}
			else if (xml.name() == "SingleFlow") {
				if (CurrentDate) CurrentDate.reset();
				else {
					NoErrorsOccured = false;
					continue;
				}
				if (CurrentAmt) CurrentAmt.reset();
				else {
					NoErrorsOccured = false;
					continue;
				}
			}
			if (CurrentElement.isEmpty()) {
				NoErrorsOccured = false;
				continue;
			}
			CurrentElement.pop();
		}
		else if (xml.isCharacters()) {
			if (!CurrrentId) {
				NoErrorsOccured = false;
				continue;
			}
			if (CurrentElement.top() == "Label") {
				if (xml.text().isEmpty()) m_CashFlowLabels.remove(*CurrrentId);
				else m_CashFlowLabels[*CurrrentId] = xml.text().toString();
			}
			else if (CurrentElement.top() == "Date") {
				if (CurrentDate) {
					NoErrorsOccured = false;
					continue;
				}
				CurrentDate.reset(new QDate(QDate::fromString(xml.text().toString(), "yyyy-MM-dd")));
			}
			else if (CurrentElement.top() == "Amount") {
				if (CurrentAmt) {
					NoErrorsOccured = false;
					continue;
				}
				CurrentAmt.reset(new double(xml.text().toDouble(&NoErrorsOccured)));
			}
			if (CurrentDate && CurrentAmt) {
				AddFlow(*CurrentDate, *CurrentAmt, *CurrrentId);
			}
		}
	}
	if (xml.hasError() || !NoErrorsOccured) {
		Clear();
	}
	xml.clear();
}

void GenericCashFlow::SetLabel(qint32 FlowTpe, const QString& Lab) {
	if (Lab.isEmpty()) m_CashFlowLabels.remove(FlowTpe);
	else m_CashFlowLabels[FlowTpe] = Lab;
}
