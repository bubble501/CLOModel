#include "GenericCashFlow.h"
#include "Private/GenericCashFlow_p.h"
#include <QDataStream>
#include <QSet>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QStack>
#include <algorithm>
#include <functional>  
#include "CommonFunctions.h"
DEFINE_PUBLIC_COMMONS(GenericCashFlow)
DEFINE_PUBLIC_COMMONS_COPY(GenericCashFlow)
GenericCashFlowPrivate::GenericCashFlowPrivate(GenericCashFlow *q,const GenericCashFlowPrivate& other)
	:BackwardInterfacePrivate(q,other)
    , m_AggregationLevel(other.m_AggregationLevel)
    , m_AdjustHolidays(other.m_AdjustHolidays)
    , m_CashFlowLabels(other.m_CashFlowLabels)
    , m_Stocks(other.m_Stocks)
{}
GenericCashFlowPrivate::GenericCashFlowPrivate(GenericCashFlow *q)
	:BackwardInterfacePrivate(q)
    , m_AggregationLevel(GenericCashFlow::NoAggregation)
    , m_AdjustHolidays(false)
{}
GenericCashFlow::GenericCashFlow(GenericCashFlowPrivate *d, const GenericCashFlow& other)
	:BackwardInterface(d,other)
{
    SetFlow(other);
}
GenericCashFlow::GenericCashFlow(GenericCashFlowPrivate *d)
	:BackwardInterface(d)
{}

GenericCashFlow& GenericCashFlow::operator=(const GenericCashFlow& other)
{
    BackwardInterface::operator=(other);
    Aggregate(other.d_func()->m_AggregationLevel);
    SetFlow(other);
    return *this;
}
GenericCashFlow::~GenericCashFlow() {
	Clear();
}

bool GenericCashFlow::operator!=(const GenericCashFlow& a) const
{
    return !(this->operator==(a));
}

void GenericCashFlow::AddFlow(QDate Dte, double Amt, qint32 FlowTpe)
{
    Q_D(GenericCashFlow);
	if (Dte.isNull()) return;
	if (qAbs(Amt) < 0.01) Amt = 0.0;
	if (d->m_AdjustHolidays) {
        while (IsHoliday(Dte))
            Dte=Dte.addDays(1); 
    }
    auto index = d->m_CashFlows.begin();
    for (; index != d->m_CashFlows.end(); ++index) {
        if (d->SamePeriod(Dte, index.key(), d->m_AggregationLevel))
            break;
	}
    if (index != d->m_CashFlows.end()) {
		//if (Amt== 0.0) return;
		Amt += GetFlow(index.key(), FlowTpe);
		if (qAbs(Amt) < 0.01)
            Amt = 0.0;
		if (index.value()->contains(FlowTpe)) {
            if (Amt == 0.0 && !d->m_Stocks.contains(FlowTpe))
                index.value()->remove(FlowTpe);
			else
                index.value()->operator[](FlowTpe) = Amt;
		}
		else {
            if (qAbs(Amt) > 0.0 || d->m_Stocks.contains(FlowTpe)) 
                index.value()->insert(FlowTpe, Amt);
		}
	}
	else {
        d->m_CashFlows.insert(Dte, new QHash<qint32, double>());
		//if (qAbs(Amt) > 0.0) {
			Amt += GetFlow(Dte, FlowTpe);
			if (qAbs(Amt) < 0.01) 
                Amt = 0.0;
            if (qAbs(Amt) > 0.0 || d->m_Stocks.contains(FlowTpe)) 
                d->m_CashFlows[Dte]->insert(FlowTpe, Amt);
		//}
	}
}

void GenericCashFlow::SetFlow(QDate Dte, double Amt, qint32 FlowTpe) {
    Q_D(GenericCashFlow);
	if (Dte.isNull()) return;
    if (d->m_AdjustHolidays) { while (IsHoliday(Dte)) Dte = Dte.addDays(1); }
	if (qAbs(Amt) < 0.01) Amt = 0.0;
    auto index = d->m_CashFlows.begin();
    for (; index != d->m_CashFlows.end(); ++index) {
        if (d->SamePeriod(Dte, index.key(), d->m_AggregationLevel)) break;
	}
    if (index != d->m_CashFlows.end()) {
		if (index.value()->contains(FlowTpe)) {
            if (Amt == 0.0 && !d->m_Stocks.contains(FlowTpe)) index.value()->remove(FlowTpe);
			else {
				index.value()->operator[](FlowTpe) = Amt;
			}
		}
		else {
            if (qAbs(Amt) > 0.0 || d->m_Stocks.contains(FlowTpe)) {
				index.value()->insert(FlowTpe, Amt);
			}
		}
	}
	else {
        d->m_CashFlows.insert(Dte, new QHash<qint32, double>());
        if (qAbs(Amt) > 0.0 || d->m_Stocks.contains(FlowTpe)) {
            d->m_CashFlows[Dte]->insert(FlowTpe, Amt);
		}
	}
}


void GenericCashFlow::AddFlow(const GenericCashFlow& a) {
    Q_D(GenericCashFlow);
	QSet<qint32> StocksToCalculate;
	QHash<qint32,QSet<QDate> >DatesOfStockChange;
    d->m_Stocks.unite(a.d_func()->m_Stocks);
	qint32 FakeFlow = 0;
    while (d->m_Stocks.contains(FakeFlow)) ++FakeFlow;

	//Aggregate Flows
    for (auto i = a.d_func()->m_CashFlows.constBegin(); i != a.d_func()->m_CashFlows.constEnd(); ++i) {
		if (i.value()->isEmpty()) AddFlow(i.key(), 0.0, FakeFlow);
		else {
			for (QHash<qint32, double>::const_iterator j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
                if (d->m_Stocks.contains(j.key())) {
					//Save stocks types to calculate later
					AddFlow(i.key(), 0.0, FakeFlow);
					StocksToCalculate.insert(j.key());
					DatesOfStockChange[j.key()].insert(i.key());
				}
				else AddFlow(i.key(), j.value(), j.key());
			}
		}
	}
    for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd(); ++i) {
		for (auto j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
            if (d->m_Stocks.contains(j.key())) {
				//Add stocks types already in cash flows
				StocksToCalculate.insert(j.key());
				DatesOfStockChange[j.key()].insert(i.key());
			}
		}
	}
	//Aggregate Stocks
    for (auto SingleStock = StocksToCalculate.constBegin(); SingleStock != StocksToCalculate.constEnd(); ++SingleStock) {
		auto CurrDates = DatesOfStockChange.value(*SingleStock, QSet<QDate>()).toList();
		std::sort(CurrDates.begin(), CurrDates.end(), std::greater<QDate>());
		for (auto i = CurrDates.constBegin(); i != CurrDates.constEnd(); ++i) {
			SetFlow(*i, GetFlow(*i, *SingleStock) + a.GetFlow(*i, *SingleStock), *SingleStock);
		}
	}
	//Aggregate Labels
    for (auto i = a.d_func()->m_CashFlowLabels.constBegin(); i != a.d_func()->m_CashFlowLabels.constEnd(); ++i) {
        if (!d->m_CashFlowLabels.contains(i.key()))
            d->m_CashFlowLabels.insert(i.key(), i.value());
	}
}
void GenericCashFlow::SetFlow(const GenericCashFlow& a) {
    Q_D(GenericCashFlow);
	Clear();
    d->m_Stocks = a.d_func()->m_Stocks;
    d->m_CashFlowLabels = a.d_func()->m_CashFlowLabels;
    if (!a.d_func()->m_CashFlows.isEmpty()) {
        for (auto i = a.d_func()->m_CashFlows.constEnd() - 1; ; --i) {
			if (i.value()->isEmpty()) SetFlow(i.key(), 0.0, 0);
			for (auto j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
				SetFlow(i.key(), j.value(), j.key());
			}
            if (i == a.d_func()->m_CashFlows.constBegin()) break;
		}
	}
}

void GenericCashFlow::Clear() {
    Q_D(GenericCashFlow);
    for (auto i = d->m_CashFlows.begin(); i != d->m_CashFlows.end(); i++) {
		delete (i.value());
	}
    d->m_CashFlows.clear();
}

QDate GenericCashFlow::GetDate(int index) const {
    Q_D(const GenericCashFlow);
    if (index < 0 || index >= d->m_CashFlows.size()) 
        return QDate();
    return (d->m_CashFlows.constBegin() + index).key();
}

double GenericCashFlow::GetFlow(const QDate& index, qint32 FlowTpe) const {
    Q_D(const GenericCashFlow);
	if (index.isNull()) 
        return 0.0;
    if (d->m_Stocks.contains(FlowTpe)) {
        auto FoundIt = d->m_CashFlows.constFind(index);
        if (FoundIt == d->m_CashFlows.constEnd()) {
            FoundIt = d->m_CashFlows.lowerBound(index);
            if (FoundIt == d->m_CashFlows.constBegin())
                return 0.0;
			--FoundIt;
		}
		for (; ; --FoundIt) {
			if (FoundIt.value()->contains(FlowTpe))
				return FoundIt.value()->value(FlowTpe);
            if (FoundIt == d->m_CashFlows.constBegin()) 
                break;
		}
		return 0.0;
	}
    if (!d->m_CashFlows.contains(index)) 
        return 0.0;
    return d->m_CashFlows.value(index)->value(FlowTpe, 0.0);
}

double GenericCashFlow::GetFlow(int index, qint32 FlowTpe) const {
	return GetFlow(GetDate(index), FlowTpe);
}

int GenericCashFlow::FindDate(const QDate& a) const {
    Q_D(const GenericCashFlow);
    auto TempFind = d->m_CashFlows.find(a);
    if (TempFind == d->m_CashFlows.constEnd()) return -1;
	int Result = 0;
    for (auto i = d->m_CashFlows.constBegin(); i != TempFind; ++i) 
        ++Result;
	return Result;
}

void GenericCashFlow::ReplaceDate(const QDate& OriginalDate, const QDate& NewDate) {
    Q_D(GenericCashFlow);
    auto TempIter = d->m_CashFlows.find(OriginalDate);
    if (TempIter != d->m_CashFlows.end()) {
		QHash<qint32, double>* TempPoint = TempIter.value();
        d->m_CashFlows.erase(TempIter);
        d->m_CashFlows.insert(NewDate, TempPoint);
	}
}

void GenericCashFlow::Aggregate(CashFlowAggregation Freq) {
    Q_D(GenericCashFlow);
    d->m_AggregationLevel = Freq;
	if (Freq == NoAggregation) return;
    for (auto MainIter = d->m_CashFlows.begin(); MainIter != d->m_CashFlows.end(); ++MainIter) {
        for (auto SecondIter = MainIter + 1; SecondIter != d->m_CashFlows.end();) {
            if (d->SamePeriod(MainIter.key(), SecondIter.key(), Freq)) {
				QHash<qint32, double>* TempMain = MainIter.value();
				const QHash<qint32, double>* TempSecond = SecondIter.value();
				for (auto i = TempSecond->constBegin(); i != TempSecond->constEnd(); ++i) {
					if (!TempMain->contains(i.key())) 
						TempMain->insert(i.key(), 0.0); //Should never happen but just to be safe
                    if (d->m_Stocks.contains(i.key())) TempMain->operator[](i.key()) = i.value();
					else TempMain->operator[](i.key()) += i.value();
				}
				delete (SecondIter.value());
                SecondIter = d->m_CashFlows.erase(SecondIter);
			}
			else break;
		}
	}
}

bool GenericCashFlowPrivate::SamePeriod(const QDate& a, const QDate& b, GenericCashFlow::CashFlowAggregation Freq)
{
	int YearA=0, YearB=0;
	bool Result;
	switch (Freq) {
    case GenericCashFlow::TotalAggragate:
		return true;
    case GenericCashFlow::Annually:
		return a.year() == b.year();
    case GenericCashFlow::SemiAnnually:
		return a.year() == b.year() && (a.month() - 1) / 6 == (b.month() - 1) / 6;
    case GenericCashFlow::Quarterly:
		return a.year() == b.year() && (a.month() - 1) / 3 == (b.month() - 1) / 3;
    case GenericCashFlow::Monthly:
		return a.year() == b.year() && a.month() == b.month();
    case GenericCashFlow::Weekly:
		Result = a.weekNumber(&YearA) == b.weekNumber(&YearB);
		return Result && YearA == YearB;
    case GenericCashFlow::NoAggregation:
	default:
		return a==b;
	}
}
QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows) {
	stream 
        << flows.d_func()->m_AdjustHolidays 
        << flows.d_func()->m_CashFlowLabels
        << flows.d_func()->m_Stocks
        << static_cast<qint32>(flows.d_func()->m_AggregationLevel)
        << static_cast<qint32>(flows.d_func()->m_CashFlows.size());
    for (auto MainIter = flows.d_func()->m_CashFlows.constBegin(); MainIter != flows.d_func()->m_CashFlows.constEnd(); ++MainIter) {
        stream << MainIter.key() << *(MainIter.value());
	}
	return stream;
}
QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows) { return flows.LoadOldVersion(stream); }
QDataStream& GenericCashFlow::LoadOldVersion(QDataStream& stream) {
    Q_D(GenericCashFlow);
	quint32 TempSize;
	QDate TempDate;
	QHash<qint32, double> TempMain;
	stream 
        >> d->m_AdjustHolidays 
        >> d->m_CashFlowLabels
        >> d->m_Stocks
        >> TempSize;
    d->m_AggregationLevel = static_cast<CashFlowAggregation>(TempSize);
	stream >> TempSize;
	for (quint32 i = 0; i < TempSize; i++) {
		stream >> TempDate >> TempMain;
        d->m_CashFlows.insert(TempDate, new QHash<qint32, double>(TempMain));
		TempMain.clear();
	}
	ResetProtocolVersion();
	return stream;
}



QDate GenericCashFlow::MaturityDate() const {
    Q_D(const GenericCashFlow);
    if (d->m_CashFlows.isEmpty()) return QDate();
    for (auto i = d->m_CashFlows.constEnd() - 1; ; --i) {
		if (!i.value()->isEmpty()) return i.key();
        if (i == d->m_CashFlows.constBegin()) return QDate();
	}
	return QDate();
}

int GenericCashFlow::Count() const
{
    Q_D(const GenericCashFlow);
    return d->m_CashFlows.size();
}

bool GenericCashFlow::operator==(const GenericCashFlow& a) const
{
    Q_D(const GenericCashFlow);
    if (d->m_AggregationLevel != a.d_func()->m_AggregationLevel) return false;
    if (d->m_CashFlows.size() != a.d_func()->m_CashFlows.size()) return false;
    for (auto MainIter = d->m_CashFlows.constBegin(); MainIter != d->m_CashFlows.constEnd(); ++MainIter) {
		if (!a.d_func()->m_CashFlows.contains(MainIter.key())) return false;
		const QHash<qint32, double>* TempMain = MainIter.value();
        const QHash<qint32, double>* TempSec = a.d_func()->m_CashFlows.value(MainIter.key());
		if (TempMain->size() != TempSec->size()) return false;

		for (auto SecIter = TempMain->constBegin(); SecIter != TempMain->constEnd(); ++SecIter) {
			if (!TempSec->contains(SecIter.key())) return false;
			if (qAbs(TempSec->value(SecIter.key()) - SecIter.value()) >= 0.01) return false;
		}
	}
	return true;
}

GenericCashFlow& GenericCashFlow::operator+=(const GenericCashFlow& a)
{
    AddFlow(a); 
    return *this;
}

GenericCashFlow GenericCashFlow::operator+(const GenericCashFlow& a) const
{
	GenericCashFlow Result(*this); 
	Result.AddFlow(a); 
	return Result;
}


QString GenericCashFlow::ToString() const {
    Q_D(const GenericCashFlow);
	QSet<qint32> FlowsTypes;
    for (auto MainIter = d->m_CashFlows.constBegin(); MainIter != d->m_CashFlows.constEnd(); ++MainIter) {
		FlowsTypes.unite(MainIter.value()->keys().toSet());
	}
	QString Result("Date");
	QList<qint32> FlowsTypesList = FlowsTypes.toList();
	std::sort(FlowsTypesList.begin(), FlowsTypesList.end());
	for (auto SecIter = FlowsTypesList.constBegin(); SecIter != FlowsTypesList.constEnd(); ++SecIter) {
		Result += '\t' + GetLabel(*SecIter, QString("Flow %1").arg(*SecIter));
	}
    for (auto MainIter = d->m_CashFlows.constBegin(); MainIter != d->m_CashFlows.constEnd(); ++MainIter) {
		Result += '\n' + MainIter.key().toString("dd-MM-yyyy");
		for (QList<qint32>::const_iterator SecIter = FlowsTypesList.constBegin(); SecIter != FlowsTypesList.constEnd(); ++SecIter) {
			Result += '\t' + QString::number(GetFlow(MainIter.key(), *SecIter), 'f', 2);
		}
	}
	return Result;
}

GenericCashFlow GenericCashFlow::SingleFlow(qint32 FlowTpe) const {
    Q_D(const GenericCashFlow);
	GenericCashFlow Result;
    Result.Aggregate(d->m_AggregationLevel);
	if (IsStock(FlowTpe)) Result.SetStock(FlowTpe);
    for (auto MainIter = d->m_CashFlows.constBegin(); MainIter != d->m_CashFlows.constEnd(); ++MainIter) {
		Result.SetFlow(MainIter.key(), GetFlow(MainIter.key(),FlowTpe), FlowTpe);
	}
	if (!GetLabel(FlowTpe).isEmpty()) Result.SetLabel(FlowTpe, GetLabel(FlowTpe));
	return Result;
}
GenericCashFlow GenericCashFlow::SingleDate(const QDate& a) const {
	GenericCashFlow Result;
    Q_D(const GenericCashFlow);
    Result.Aggregate(d->m_AggregationLevel);
    Result.d_func()->m_Stocks = d->m_Stocks;
    for (auto MainIter = d->m_CashFlows.constBegin(); MainIter != d->m_CashFlows.constEnd(); ++MainIter) {
        if (d->SamePeriod(a, MainIter.key(), d->m_AggregationLevel)) {
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
    Q_D(const GenericCashFlow);
    for (auto MainIter = d->m_CashFlows.constBegin(); MainIter != d->m_CashFlows.constEnd(); ++MainIter) {
		if (MainIter.value()->contains(FlowTpe)){
            if (d->m_Stocks.contains(FlowTpe)) {
				if (MainIter.value()->value(FlowTpe) >= 0.01) return true; 
			}
			else return true;
		}
	}
	return false;
}

QList<qint32> GenericCashFlow::AvailableFlows(const QDate& a) const {
    Q_D(const GenericCashFlow);
	QSet<qint32> Result;
	if (a.isNull()) {
        for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd(); ++i) {
			for (auto j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
				Result.insert(j.key());
			}
		}
		return Result.toList();
	}
	
    auto TempIter = d->m_CashFlows.constFind(a);
    if (TempIter != d->m_CashFlows.constEnd())
		Result.unite(TempIter.value()->keys().toSet());
    for (TempIter = d->m_CashFlows.constBegin(); TempIter != d->m_CashFlows.constEnd(); ++TempIter) {
		if (TempIter.key() > a) break;
        for (auto SingleStock = d->m_Stocks.constBegin(); SingleStock != d->m_Stocks.constEnd(); ++SingleStock)
        {
			if (TempIter.value()->contains(*SingleStock))
                Result.insert(*SingleStock);
		}
	}
	return Result.toList();
}

bool GenericCashFlow::GetAdjustHolidays() const
{
    Q_D(const GenericCashFlow);
    return d->m_AdjustHolidays;
}

void GenericCashFlow::SetAdjustHolidays(bool val)
{
    Q_D(GenericCashFlow);
    d->m_AdjustHolidays = val;
    if (val) {
        for (auto i = d->m_CashFlows.begin(); i != d->m_CashFlows.end(); ++i) {
			while (IsHoliday(i.key())) {
                d->m_CashFlows.insert(i.key().addDays(1), i.value());
                i = d->m_CashFlows.erase(i);
			}
		}
	}
}

double GenericCashFlow::GetTotalFlow(const QDate& a, const QList<qint32>& Groups /*= QList<qint32>()*/) const {
    Q_D(const GenericCashFlow);
	if (a.isNull()) return 0.0;
    auto TempIter = d->m_CashFlows.constFind(a);
    if (TempIter == d->m_CashFlows.constEnd()) return 0.0;
	double Result = 0.0;
	for (QHash<qint32, double>::const_iterator i = TempIter.value()->constBegin(); i != TempIter.value()->constEnd(); ++i) {
		if (Groups.isEmpty() || Groups.contains(i.key())) Result += i.value();
	}
	return Result;
}
double GenericCashFlow::GetTotalFlow(int index, const QList<qint32>& Groups /*= QList<qint32>()*/) const {
	return GetTotalFlow(GetDate(index), Groups);
}

double GenericCashFlow::GetTotalFlow(const QDate& a) const
{
    return GetTotalFlow(a, QList<qint32>());
}

double GenericCashFlow::GetTotalFlow(int index) const
{
    return GetTotalFlow(index, QList<qint32>());
}

bool GenericCashFlow::IsEmpty() const
{
    Q_D(const GenericCashFlow);
    for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd(); ++i) {
		if (!i.value()->isEmpty()) return false;
	}
	return true;
}

GenericCashFlow GenericCashFlow::AggregateRange(const QDate& StartDate, const QDate& EndDate) const{
    Q_D(const GenericCashFlow);
	GenericCashFlow Results;
    Results.d_func()->m_Stocks = d->m_Stocks;
    Results.d_func()->m_CashFlowLabels = d->m_CashFlowLabels;
    Results.d_func()->m_AdjustHolidays = d->m_AdjustHolidays;
    Results.Aggregate(d->m_AggregationLevel);
    for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd(); ++i) {
		if (i.key()>EndDate) break;
		if (i.key() >= StartDate) {
			for (QHash<qint32, double>::const_iterator j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
                if (d->m_Stocks.contains(j.key())) Results.SetFlow(EndDate, j.value(), j.key());
				else Results.AddFlow(EndDate, j.value(), j.key());
			}
		}
	}
	return Results;
}

void GenericCashFlow::RemoveFlow(qint32 FlowTpe) {
    Q_D(GenericCashFlow);
    for (auto i = d->m_CashFlows.begin(); i != d->m_CashFlows.end(); ++i) {
		i.value()->remove(FlowTpe);
	}
}

void GenericCashFlow::RemoveAllFlows()
{
    Clear();
}

void GenericCashFlow::ResetFlows()
{
    Clear();
}

GenericCashFlow GenericCashFlow::ScaledCashFlows(double OriginalRefSize, double ResultSize, const QList<qint32>& Groups, const QList<qint32>& ExcludeGroups) const
{
    Q_D(const GenericCashFlow);
	GenericCashFlow Result;
    Result.d_func()->m_Stocks = d->m_Stocks;
    Result.d_func()->m_AdjustHolidays = d->m_AdjustHolidays;
    Result.d_func()->m_CashFlowLabels = d->m_CashFlowLabels;
    Result.Aggregate(d->m_AggregationLevel);
	if (ResultSize == 0.0 || OriginalRefSize == 0.0) return Result;
	const double ScaleRatio = ResultSize/OriginalRefSize ;
    for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd(); ++i) {
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
    Q_D(const GenericCashFlow);
	if (IsEmpty()) return false;
	QString Result("Date");
	auto AllFlows = AvailableFlows();
	std::sort(AllFlows.begin(),AllFlows.end());
	for (auto j = AllFlows.constBegin(); j != AllFlows.constEnd(); ++j) {
		if (UseHeaders) Result += '\t' + GetLabel(*j, QString("Flow %1").arg(*j));
		else  Result += QString("%1").arg(*j);
	}
    for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd(); ++i) {
		Result += '\n' + i.key().toString("yyyy-MM-dd");
		for (auto j = AllFlows.constBegin(); j != AllFlows.constEnd(); ++j) {
			Result += '\t' + QString::number(GetFlow(i.key(), *j), 'f');
		}
	}
	return Result;
}

QString GenericCashFlow::ToXML() const {
    Q_D(const GenericCashFlow);
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
        for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd(); ++i) {
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
    Q_D(GenericCashFlow);
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
                if (xml.text().isEmpty()) d->m_CashFlowLabels.remove(*CurrrentId);
                else d->m_CashFlowLabels[*CurrrentId] = xml.text().toString();
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

void GenericCashFlow::SetStock(qint32 a)
{
    Q_D(GenericCashFlow);
    d->m_Stocks.insert(a);
}

void GenericCashFlow::RemoveStock(qint32 a)
{
    Q_D(GenericCashFlow);
    d->m_Stocks.remove(a);
}

void GenericCashFlow::ClearStocks()
{
    Q_D(GenericCashFlow);
    d->m_Stocks.clear();
}

bool GenericCashFlow::IsStock(qint32 a) const
{
    Q_D(const GenericCashFlow);
    return d->m_Stocks.contains(a);
}

const QSet<qint32>& GenericCashFlow::GetStocks() const
{
    Q_D(const GenericCashFlow);
    return d->m_Stocks;
}

QDate GenericCashFlow::nextFlowDate(const QDate& currDt) const
{
    Q_D(const GenericCashFlow);
    for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd();++i){
        if (i.key() > currDt)
            return i.key();
    }
    return QDate();
}

QDate GenericCashFlow::prevFlowDate(const QDate& currDt) const
{
    Q_D(const GenericCashFlow);
    for (auto i = d->m_CashFlows.constEnd()-1; ; --i) {
        if (i.key() < currDt)
            return i.key();
        if (i == d->m_CashFlows.constBegin())
            break;
    }
    return QDate();
}

void GenericCashFlow::SetLabel(qint32 FlowTpe, const QString& Lab)
{
    Q_D(GenericCashFlow);
    if (Lab.isEmpty()) 
        d->m_CashFlowLabels.remove(FlowTpe);
    else d->m_CashFlowLabels[FlowTpe] = Lab;
}

QString GenericCashFlow::GetLabel(qint32 FlowTpe, const QString& DefaultLab /*= QString()*/) const
{
    Q_D(const GenericCashFlow);
    return d->m_CashFlowLabels.value(FlowTpe, DefaultLab);
}

void GenericCashFlow::RemoveLabel(qint32 FlowTp)
{
    Q_D(GenericCashFlow);
    d->m_CashFlowLabels.remove(FlowTp);
}

void GenericCashFlow::ClearLabels()
{
    Q_D(GenericCashFlow);
    d->m_CashFlowLabels.clear();
}
