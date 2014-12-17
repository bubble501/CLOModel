#include "TrancheCashFlow.h"
#include <QMap>
#include <QDataStream>
#include "MtgCashFlow.h"
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSettings>
#include <QVariant>
#include "QSingleBbgResult.h"
#endif
TrancheCashFlow::TrancheCashFlow(double ThrancheOutstanding)
	:OutstandingAmt(ThrancheOutstanding)
{
	for (qint32 i = static_cast<qint32>(TrancheFlowType::InterestFlow); i < (static_cast<qint32>(TrancheFlowType::InterestFlow) << 1); ++i) {
		SetLabel(i, QString("Interest %1").arg(i - static_cast<qint32>(TrancheFlowType::InterestFlow) + 1));
	}
	for (qint32 i = static_cast<qint32>(TrancheFlowType::DeferredFlow); i < (static_cast<qint32>(TrancheFlowType::DeferredFlow) << 1); ++i) {
		SetLabel(i, QString("Deferred Interest %1").arg(i - static_cast<qint32>(TrancheFlowType::DeferredFlow) + 1));
		SetStock(i);
	}
	for (qint32 i = static_cast<qint32>(TrancheFlowType::AccruedFlow); i < (static_cast<qint32>(TrancheFlowType::AccruedFlow) << 1); ++i) {
		SetLabel(i, QString("Accrued Interest %1").arg(i - static_cast<qint32>(TrancheFlowType::AccruedFlow) + 1));
	}
	SetLabel(static_cast<qint32>(TrancheFlowType::PrincipalFlow), "Principal");
	SetLabel(static_cast<qint32>(TrancheFlowType::AmountOutstandingFlow), "Amount Outstanding");
	SetLabel(static_cast<qint32>(TrancheFlowType::OCFlow), "OC Test");
	SetLabel(static_cast<qint32>(TrancheFlowType::ICFlow), "IC Test");
	SetLabel(static_cast<qint32>(TrancheFlowType::OCTarget), "OC Target");
	SetLabel(static_cast<qint32>(TrancheFlowType::ICTarget), "IC Target");
	SetLabel(static_cast<qint32>(TrancheFlowType::PDLOutstanding), "PDL Outstanding");
	SetLabel(static_cast<qint32>(TrancheFlowType::PDLCured), "PDL Cured");
	
	SetStock(static_cast<qint32>(TrancheFlowType::AmountOutstandingFlow));
	SetStock(static_cast<qint32>(TrancheFlowType::PDLOutstanding));
	Aggregate(Monthly);
}


TrancheCashFlow::TrancheCashFlow(const TrancheCashFlow& a)
	:OutstandingAmt(a.OutstandingAmt)
	,StartingDeferredInterest(a.StartingDeferredInterest)
	,GenericCashFlow(a)
{}


QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows){
	stream
		<< flows.OutstandingAmt
		<< flows.StartingDeferredInterest
		<< static_cast<const GenericCashFlow&>(flows)
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows) { return flows.LoadOldVersion(stream); }
QDataStream& TrancheCashFlow::LoadOldVersion(QDataStream& stream) {
	stream
		>> OutstandingAmt
		>> StartingDeferredInterest
		;
	return GenericCashFlow::LoadOldVersion(stream);
}

QDate TrancheCashFlow::GetLastFlowDate(bool IncludeDeferred ) const {
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = m_CashFlows.constEnd() - 1; true; --i) {
		for (QHash<qint32, double>::const_iterator j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
			if ((
				(j.key() & static_cast<qint32>(TrancheFlowType::InterestFlow)) 
				|| (j.key() == static_cast<qint32>(TrancheFlowType::PrincipalFlow))
				|| ((j.key() & static_cast<qint32>(TrancheFlowType::DeferredFlow)) && IncludeDeferred)
			) && j.value() >= 0.01) return i.key();

		}
		if (i == m_CashFlows.constBegin()) break;
	}
	return QDate();
}

int TrancheCashFlow::GetLastFlowIndex(bool IncludeDeferred) const {
	QDate Target = GetLastFlowDate(IncludeDeferred);
	if (!Target.isValid()) return -1;
	int Counter = 0;
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		if (i.key() == Target) return Counter;
		++Counter;
	}
	return -1;
}

TrancheCashFlow& TrancheCashFlow::operator=(const TrancheCashFlow& a) {
	OutstandingAmt = a.OutstandingAmt; 
	StartingDeferredInterest = a.StartingDeferredInterest; 
	GenericCashFlow::operator=(a);
	return *this;
}

double TrancheCashFlow::GetTotalFlow(int index) const {
	QList<qint32> FlowsType;
	FlowsType.append(static_cast<qint32>(TrancheFlowType::PrincipalFlow));
	for (qint32 i = static_cast<qint32>(TrancheFlowType::InterestFlow); i < (static_cast<qint32>(TrancheFlowType::InterestFlow) << 1); ++i)
		FlowsType.append(i);
	return GenericCashFlow::GetTotalFlow(index,FlowsType);
}
double TrancheCashFlow::GetTotalFlow(const QDate& a) const {
	QList<qint32> FlowsType;
	FlowsType.append(static_cast<qint32>(TrancheFlowType::PrincipalFlow));
	for (qint32 i = static_cast<qint32>(TrancheFlowType::InterestFlow); i < (static_cast<qint32>(TrancheFlowType::InterestFlow) << 1); ++i)
		FlowsType.append(i);
	return GenericCashFlow::GetTotalFlow(a, FlowsType);
}
#ifndef NO_DATABASE
bool TrancheCashFlow::GetCashFlowsDatabase(const QString& TrancheID) {
	if (TrancheID.isEmpty()) return false;
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype", "QODBC"), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + GetFromConfig("Database", "Driver", "SQL Server")
			+ "}; "
			+ GetFromConfig("Database", "DataSource", R"(Server=SYNSERVER2\SQLExpress;Initial Catalog=ABSDB;Integrated Security=SSPI;Trusted_Connection=Yes;)")
			);
	}
	if (db.open()) {
		QSqlQuery query(db);
		query.setForwardOnly(true);
		query.prepare("CALL " + GetFromConfig("Database", "CashFlowsStoredProc", "getCashFlows") + "(?)");
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
			db.close();
			if (!Cleared) return false;
			return true;
		}
		db.close();
	}
	return false;
}
#endif
#ifndef NO_BLOOMBERG
bool TrancheCashFlow::GetCashFlowsBloomberg(const QBloombergLib::QSingleBbgResult& a) {
	if (
		(
			a.GetHeader() != "MTG_CASH_FLOW" 
			&&  a.GetHeader() != "EXTENDED_CASH_FLOW"
			&&  a.GetHeader() != "HIST_CASH_FLOW"
		)
		|| a.HasErrors()
		|| a.GetNumRows() == 0
	) return false;
	for (int i = 0; i < a.GetNumRows(); ++i) {
		const QDate CurrentDate = a.GetTableResult(i, 1)->GetDate();
		AddFlow(CurrentDate, a.GetTableResult(i, 3)->GetDouble(), TrancheFlowType::InterestFlow);
		AddFlow(CurrentDate, a.GetTableResult(i, 4)->GetDouble(), TrancheFlowType::PrincipalFlow);
		AddFlow(CurrentDate, -a.GetTableResult(i, 4)->GetDouble(), TrancheFlowType::AmountOutstandingFlow);
		if (i == 0) {
			OutstandingAmt = a.GetTableResult(0, 4)->GetDouble() + a.GetTableResult(0, 5)->GetDouble();
		}
	}
	return true;
}
#endif
TrancheCashFlow TrancheCashFlow::ScaledCashFlows(double OldSize,double NewSize) const {
	TrancheCashFlow Result;
	if (NewSize == 0.0 || OldSize == 0.0) return Result;
	Result.SetInitialOutstanding(OutstandingAmt * NewSize/OldSize);
	for (auto i = Result.StartingDeferredInterest.begin(); i != Result.StartingDeferredInterest.end(); ++i) {
		i.value() *= NewSize / OldSize;
	}
	Result.AddFlow(GenericCashFlow::ScaledCashFlows(OldSize, NewSize, QList<qint32>(), QList<qint32>()
		 << static_cast<qint32>(TrancheFlowType::OCFlow)
		 << static_cast<qint32>(TrancheFlowType::ICFlow)
		 << static_cast<qint32>(TrancheFlowType::OCTarget)
		 << static_cast<qint32>(TrancheFlowType::ICTarget)
		));
	return Result;
}


void TrancheCashFlow::LoadFromXML(const QString& Source) {
	GenericCashFlow::LoadFromXML(Source);
	if (m_CashFlows.isEmpty()) {
		OutstandingAmt = 0.0;
		return;
	}
	OutstandingAmt =
		m_CashFlows.constBegin().value()->value(static_cast<qint32>(TrancheFlowType::AmountOutstandingFlow), 0.0)
		+ m_CashFlows.constBegin().value()->value(static_cast<qint32>(TrancheFlowType::PrincipalFlow), 0.0);
}

void TrancheCashFlow::AddFlow(const QDate& Dte, double Amt, TrancheFlowType FlwTpe) {
	AddFlow(Dte, Amt, static_cast<qint32>(FlwTpe));
}
void TrancheCashFlow::SetFlow(const QDate& Dte, double Amt, TrancheFlowType FlwTpe) {
	SetFlow(Dte, Amt, static_cast<qint32>(FlwTpe));
}
void TrancheCashFlow::AddFlow(QDate Dte, double Amt, qint32 FlwTpe) {
	if (m_CashFlows.isEmpty()) {
		if (OutstandingAmt>0.0) GenericCashFlow::SetFlow(Dte, OutstandingAmt, static_cast<qint32>(TrancheFlowType::AmountOutstandingFlow));
		for (auto i = StartingDeferredInterest.constBegin(); i!=StartingDeferredInterest.constEnd();++i) {
			if (i.value()>0.0) GenericCashFlow::SetFlow(Dte, i.value(), i.key());
		}
	}
	GenericCashFlow::AddFlow(Dte, Amt, FlwTpe);
}
void TrancheCashFlow::SetFlow(QDate Dte, double Amt, qint32 FlwTpe) {
	if (m_CashFlows.isEmpty()) {
		if (OutstandingAmt > 0.0) GenericCashFlow::SetFlow(Dte, OutstandingAmt, static_cast<qint32>(TrancheFlowType::AmountOutstandingFlow));
		for (auto i = StartingDeferredInterest.constBegin(); i != StartingDeferredInterest.constEnd(); ++i) {
			if (i.value() > 0.0) GenericCashFlow::SetFlow(Dte, i.value(), i.key());
		}
	}
	GenericCashFlow::SetFlow(Dte, Amt, FlwTpe);
}

void TrancheCashFlow::SetStartingDeferredInterest(const double& val, qint32 CoupIdx ) {
	if (CoupIdx >= 0 && CoupIdx < (1 << MaximumInterestsTypes)) {
		if (val == 0.0) StartingDeferredInterest.remove(CoupIdx);
		else StartingDeferredInterest[CoupIdx] = val;
	}
}

bool TrancheCashFlow::HasFlowType(qint32 FlowTpe) const {
	return GenericCashFlow::HasFlowType(FlowTpe) || StartingDeferredInterest.contains(FlowTpe);
}


