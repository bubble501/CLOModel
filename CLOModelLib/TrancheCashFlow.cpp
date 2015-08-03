#include "TrancheCashFlow.h"
#include "Private/TrancheCashFlow_p.h"
#include "Private/InternalItems.h"
#include <QMap>
#include <QDataStream>
#include "MtgCashFlow.h"
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#endif
#ifndef NO_BLOOMBERG
#include <QBbgReferenceDataResponse.h>
#endif // !NO_BLOOMBERG
DEFINE_PUBLIC_COMMONS(TrancheCashFlow)
DEFINE_PUBLIC_COMMONS_COPY(TrancheCashFlow)
TrancheCashFlowPrivate::TrancheCashFlowPrivate(TrancheCashFlow *q,const TrancheCashFlowPrivate& other)
	:GenericCashFlowPrivate(q,other)
    , OutstandingAmt(other.OutstandingAmt)
    , StartingDeferredInterest(other.StartingDeferredInterest)
{}
TrancheCashFlowPrivate::TrancheCashFlowPrivate(TrancheCashFlow *q)
	:GenericCashFlowPrivate(q)
    , OutstandingAmt(0.0)
{}
TrancheCashFlow::TrancheCashFlow(TrancheCashFlowPrivate *d, const TrancheCashFlow& other)
	:GenericCashFlow(d,other)
{}
TrancheCashFlow::TrancheCashFlow(TrancheCashFlowPrivate *d)
	:GenericCashFlow(d)
{
    for (qint32 i = 0; i < (1 << MaximumInterestsTypes); ++i) {
        SetStock(TrancheFlowType::DeferredFlow | i);
    }
    SetLabel(TrancheFlowType::PrincipalFlow, "Principal");
    SetLabel(TrancheFlowType::AmountOutstandingFlow, "Amount Outstanding");
    SetLabel(TrancheFlowType::OCFlow, "OC Test");
    SetLabel(TrancheFlowType::ICFlow, "IC Test");
    SetLabel(TrancheFlowType::OCTarget, "OC Target");
    SetLabel(TrancheFlowType::ICTarget, "IC Target");
    SetLabel(TrancheFlowType::PDLOutstanding, "PDL Outstanding");
    SetLabel(TrancheFlowType::PDLCured, "PDL Cured");

    SetStock(TrancheFlowType::AmountOutstandingFlow);
    SetStock(TrancheFlowType::PDLOutstanding);
    Aggregate(Monthly);
}


TrancheCashFlow& TrancheCashFlow::operator=(const TrancheCashFlow& other)
{
    GenericCashFlow::operator=(other);
    Q_D(TrancheCashFlow);
    d->OutstandingAmt = other.d_func()->OutstandingAmt;
    d->StartingDeferredInterest = other.d_func()->StartingDeferredInterest;
    return *this;
}

TrancheCashFlow::TrancheCashFlow(double ThrancheOutstanding)
    :TrancheCashFlow()
{
    Q_D(TrancheCashFlow);
    d->OutstandingAmt = ThrancheOutstanding;
}
QString TrancheCashFlow::GetLabel(qint32 FlowTpe, const QString& DefaultLab /*= QString()*/) const {
    Q_D(const TrancheCashFlow);
	if (d->m_CashFlowLabels.contains(FlowTpe)) 
        return GenericCashFlow::GetLabel(FlowTpe, DefaultLab);
	if (FlowTpe >= TrancheFlowType::InterestFlow && FlowTpe < (TrancheFlowType::InterestFlow << 1))
		return QString("Interest %1").arg((FlowTpe & ~TrancheFlowType::InterestFlow) + 1);
	if (FlowTpe >= TrancheFlowType::DeferredFlow && FlowTpe <( TrancheFlowType::DeferredFlow | TrancheFlowType::InterestFlow))
		return QString("Deferred Interest %1").arg((FlowTpe & ~TrancheFlowType::DeferredFlow) + 1);
	if (FlowTpe >= TrancheFlowType::AccruedFlow && FlowTpe < (TrancheFlowType::AccruedFlow | TrancheFlowType::InterestFlow))
		return QString("Accrued Interest %1").arg((FlowTpe & ~TrancheFlowType::AccruedFlow) + 1);
	return DefaultLab;
}




QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows){
	stream
		<< flows.d_func()->OutstandingAmt
        << flows.d_func()->StartingDeferredInterest
		<< static_cast<const GenericCashFlow&>(flows)
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows) { return flows.LoadOldVersion(stream); }
QDataStream& TrancheCashFlow::LoadOldVersion(QDataStream& stream) {
    Q_D(TrancheCashFlow);
	stream
		>> d->OutstandingAmt
        >> d->StartingDeferredInterest
		;
	return GenericCashFlow::LoadOldVersion(stream);
}

bool TrancheCashFlow::HasAnyInterest(TrancheFlowType base) const
{
    if (base != TrancheFlowType::InterestFlow && base != TrancheFlowType::DeferredFlow && base != TrancheFlowType::AccruedFlow)
        return false;
    for (qint32 i = 0; i < (1 << MaximumInterestsTypes); ++i) {
        if (HasFlowType(static_cast<qint32>(base) | i))
            return true;
    }
    return false;
}

QDate TrancheCashFlow::GetLastFlowDate(bool IncludeDeferred) const
{
    Q_D(const TrancheCashFlow);
    for (auto i = d->m_CashFlows.constEnd() - 1; ; --i) {
		for (auto j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
			if ((
				(j.key() & TrancheFlowType::InterestFlow) 
				|| (j.key() == TrancheFlowType::PrincipalFlow)
				|| ((j.key() & TrancheFlowType::DeferredFlow) && IncludeDeferred)
			) && j.value() >= 0.01) return i.key();

		}
        if (i == d->m_CashFlows.constBegin()) break;
	}
	return QDate();
}

int TrancheCashFlow::GetLastFlowIndex(bool IncludeDeferred) const {
    Q_D(const TrancheCashFlow);
	QDate Target = GetLastFlowDate(IncludeDeferred);
	if (!Target.isValid()) return -1;
	int Counter = 0;
    for (auto i = d->m_CashFlows.constBegin(); i != d->m_CashFlows.constEnd(); ++i) {
		if (i.key() == Target)
            return Counter;
		++Counter;
	}
	return -1;
}

double TrancheCashFlow::GetStartingDeferredInterest(qint32 CoupIdx/*=0*/) const
{
    Q_D(const TrancheCashFlow);
    return d->StartingDeferredInterest.value(CoupIdx, 0.0);
}


double TrancheCashFlow::GetTotalFlow(int index) const {
	QList<qint32> FlowsType;
	FlowsType.append(TrancheFlowType::PrincipalFlow);
	for (qint32 i = TrancheFlowType::InterestFlow; i < (TrancheFlowType::InterestFlow << 1); ++i)
		FlowsType.append(i);
	return GenericCashFlow::GetTotalFlow(index,FlowsType);
}
double TrancheCashFlow::GetTotalFlow(const QDate& a) const {
	QList<qint32> FlowsType;
	FlowsType.append(TrancheFlowType::PrincipalFlow);
	for (qint32 i = TrancheFlowType::InterestFlow; i < (TrancheFlowType::InterestFlow << 1); ++i)
		FlowsType.append(i);
	return GenericCashFlow::GetTotalFlow(a, FlowsType);
}

bool TrancheCashFlow::GetCashFlowsDatabase(const QString& TrancheID) {
#ifdef NO_DATABASE
    return false;
#else
	if (TrancheID.isEmpty()) return false;
	QMutexLocker DbLocker(&Db_Mutex);
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + GetFromConfig("Database", "Driver")
			+ "}; "
			+ GetFromConfig("Database", "DataSource")
			);
		
	}
	bool DbOpen = db.isOpen();
	if (!DbOpen) DbOpen = db.open();
	if (DbOpen) {
		QSqlQuery query(db);
		query.setForwardOnly(true);
		query.prepare("CALL " + GetFromConfig("Database", "CashFlowsStoredProc") + "(?)");
		query.bindValue(0,TrancheID);
		if (query.exec()) {
			bool Cleared = false;
			while (query.next()) {
				if (!Cleared) {
					Cleared = true;
					Clear();
					SetInitialOutstanding(query.value(5).toDouble() + query.value(4).toDouble());
					ResetStartingDeferredInterest();
				}
				AddFlow(query.value(0).toDate(), query.value(3).toDouble(), TrancheFlowType::InterestFlow);
				AddFlow(query.value(0).toDate(), query.value(4).toDouble(), TrancheFlowType::PrincipalFlow);
				AddFlow(query.value(0).toDate(), -query.value(4).toDouble(), TrancheFlowType::AmountOutstandingFlow);
				AddFlow(query.value(0).toDate(), query.value(2).toDouble(), TrancheFlowType::DeferredFlow);
			}
			if (!Cleared) return false;
			return true;
		}
	}
	return false;
#endif
}


bool TrancheCashFlow::GetCashFlowsBloomberg(const QBbgLib::QBbgAbstractResponse* a) {
#ifdef NO_BLOOMBERG
    return false;
#else
    Q_D(TrancheCashFlow);
    if (a->hasErrors()) 
        return false;
    if (a->responseType() == QBbgLib::QBbgAbstractResponse::ReferenceDataResponse) {
        const QBbgLib::QBbgReferenceDataResponse* const casted = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(a);
        Q_ASSERT(casted);
        if (casted->header() != "MTG_CASH_FLOW" && casted->header() != "EXTENDED_CASH_FLOW" && casted->header() != "DES_CASH_FLOW" && casted->header() != "HIST_CASH_FLOW")
            return false;
        if (casted->rows() <= 0)
            return false;
        Clear();
        if (casted->header() == "DES_CASH_FLOW") {
            d->OutstandingAmt = 0.0;
            for (int i = 0; i < casted->rows(); ++i)
                d->OutstandingAmt += casted->getTableValue(i, 2)->value().toDouble();
            for (int i = 0; i < casted->rows(); ++i) {
                const QDate CurrentDate = casted->getTableValue(i, 0)->value().toDate();
                AddFlow(CurrentDate, casted->getTableValue(i, 1)->value().toDouble(), TrancheFlowType::InterestFlow);
                AddFlow(CurrentDate, casted->getTableValue(i, 2)->value().toDouble(), TrancheFlowType::PrincipalFlow);
                AddFlow(CurrentDate, -casted->getTableValue(i, 2)->value().toDouble(), TrancheFlowType::AmountOutstandingFlow);
            }
        }
        else { // "MTG_CASH_FLOW", "HIST_CASH_FLOW" or "EXTENDED_CASH_FLOW"
            d->OutstandingAmt = casted->getTableValue(0, 4)->value().toDouble() + casted->getTableValue(0, 5)->value().toDouble();
            for (int i = 0; i < casted->rows(); ++i) {
                const QDate CurrentDate = casted->getTableValue(i, 1)->value().toDate();
                AddFlow(CurrentDate, casted->getTableValue(i, 3)->value().toDouble(), TrancheFlowType::InterestFlow);
                AddFlow(CurrentDate, casted->getTableValue(i, 4)->value().toDouble(), TrancheFlowType::PrincipalFlow);
                SetFlow(CurrentDate, casted->getTableValue(i, 5)->value().toDouble(), TrancheFlowType::AmountOutstandingFlow);
            }
        }
        return true;
    }
	return false;
#endif
}

TrancheCashFlow TrancheCashFlow::ScaledCashFlows(double OldSize,double NewSize) const {
    Q_D(const TrancheCashFlow);
	TrancheCashFlow Result;
	if (NewSize == 0.0 || OldSize == 0.0) return Result;
    Result.SetInitialOutstanding(d->OutstandingAmt * NewSize / OldSize);
    for (auto i = Result.d_func()->StartingDeferredInterest.begin(); i != Result.d_func()->StartingDeferredInterest.end(); ++i) {
		i.value() *= NewSize / OldSize;
	}
	Result.AddFlow(GenericCashFlow::ScaledCashFlows(OldSize, NewSize, QList<qint32>(), QList<qint32>()
		 << TrancheFlowType::OCFlow
		 << TrancheFlowType::ICFlow
		 << TrancheFlowType::OCTarget
		 << TrancheFlowType::ICTarget
		));
	return Result;
}


TrancheCashFlow TrancheCashFlow::ScaledCashFlows(double NewSize) const
{
    Q_D(const TrancheCashFlow);
    return ScaledCashFlows(d->OutstandingAmt, NewSize);
}

void TrancheCashFlow::LoadFromXML(const QString& Source)
{
    Q_D(TrancheCashFlow);
	GenericCashFlow::LoadFromXML(Source);
    if (d->m_CashFlows.isEmpty()) {
        d->OutstandingAmt = 0.0;
		return;
	}
    d->OutstandingAmt =
        d->m_CashFlows.constBegin().value()->value(TrancheFlowType::AmountOutstandingFlow, 0.0)
        + d->m_CashFlows.constBegin().value()->value(TrancheFlowType::PrincipalFlow, 0.0);
}

void TrancheCashFlow::AddFlow(const QDate& Dte, double Amt, TrancheFlowType FlwTpe) {
	AddFlow(Dte, Amt, static_cast<qint32>(FlwTpe));
}
void TrancheCashFlow::SetFlow(const QDate& Dte, double Amt, TrancheFlowType FlwTpe) {
	SetFlow(Dte, Amt, static_cast<qint32>(FlwTpe));
}
void TrancheCashFlow::AddFlow(QDate Dte, double Amt, qint32 FlwTpe) {
    Q_D(TrancheCashFlow);
    if (d->m_CashFlows.isEmpty()) {
        if (d->OutstandingAmt>0.0)
            GenericCashFlow::SetFlow(Dte, d->OutstandingAmt, TrancheFlowType::AmountOutstandingFlow);
        for (auto i = d->StartingDeferredInterest.constBegin(); i != d->StartingDeferredInterest.constEnd(); ++i) {
			if (i.value()>0.0) GenericCashFlow::SetFlow(Dte, i.value(), i.key());
		}
	}
	GenericCashFlow::AddFlow(Dte, Amt, FlwTpe);
}

void TrancheCashFlow::SetFlow(QDate Dte, double Amt, qint32 FlwTpe) {
    Q_D(TrancheCashFlow);
    if (d->m_CashFlows.isEmpty()) {
        if (d->OutstandingAmt > 0.0) 
            GenericCashFlow::SetFlow(Dte, d->OutstandingAmt, TrancheFlowType::AmountOutstandingFlow);
        for (auto i = d->StartingDeferredInterest.constBegin(); i != d->StartingDeferredInterest.constEnd(); ++i) {
			if (i.value() > 0.0) GenericCashFlow::SetFlow(Dte, i.value(), i.key());
		}
	}
	GenericCashFlow::SetFlow(Dte, Amt, FlwTpe);
}

void TrancheCashFlow::SetStartingDeferredInterest(const double& val, qint32 CoupIdx ) {
    Q_D(TrancheCashFlow);
	if (CoupIdx >= 0 && CoupIdx < (1 << MaximumInterestsTypes)) {
        if (val == 0.0) d->StartingDeferredInterest.remove(CoupIdx);
        else d->StartingDeferredInterest[CoupIdx] = val;
	}
}

void TrancheCashFlow::ResetStartingDeferredInterest()
{
    Q_D(TrancheCashFlow);
    d->StartingDeferredInterest.clear();
}

bool TrancheCashFlow::HasFlowType(qint32 FlowTpe) const
{
    Q_D(const TrancheCashFlow);
    if (d->StartingDeferredInterest.contains(FlowTpe))
        return true;
    // Almost equal to the inherited version
    for (auto MainIter = d->m_CashFlows.constBegin(); MainIter != d->m_CashFlows.constEnd(); ++MainIter) {
        if (MainIter.value()->contains(FlowTpe)) {
            if (d->m_Stocks.contains(FlowTpe) && FlowTpe!=TrancheFlowType::AmountOutstandingFlow) {
                if (MainIter.value()->value(FlowTpe) >= 0.01)
                    return true;
            }
            else return true;
        }
    }
    return false;
}

bool TrancheCashFlow::HasInterest() const
{
    return HasAnyInterest(TrancheFlowType::InterestFlow);
}

bool TrancheCashFlow::HasDeferred() const
{
    return HasAnyInterest(TrancheFlowType::DeferredFlow);
}

bool TrancheCashFlow::HasAccrued() const
{
    return HasAnyInterest(TrancheFlowType::AccruedFlow);
}

double TrancheCashFlow::GetInitialOutstanding() const
{
    Q_D(const TrancheCashFlow);
    return d->OutstandingAmt;
}

void TrancheCashFlow::SetInitialOutstanding(double a)
{
    Q_D(TrancheCashFlow);
    if (a == d->OutstandingAmt) return;
	/*auto OldOut = SingleFlow(TrancheFlowType::AmountOutstandingFlow)).ScaledCashFlows(OutstandingAmt,a;
	RemoveFlow(TrancheFlowType::AmountOutstandingFlow);
	for (int i = 0; i < OldOut.Count(); ++i) {
		GenericCashFlow::SetFlow(
			OldOut.GetDate(i)
			, OldOut.GetFlow(i, TrancheFlowType::AmountOutstandingFlow)
			, TrancheFlowType::AmountOutstandingFlow
		);
	}*/
	TrancheCashFlow Tempfl;
    Tempfl.AddFlow(GenericCashFlow::ScaledCashFlows(d->OutstandingAmt, a));
    Tempfl.d_func()->OutstandingAmt = a;
    Tempfl.d_func()->StartingDeferredInterest = d->StartingDeferredInterest;
	operator=(Tempfl);
}




