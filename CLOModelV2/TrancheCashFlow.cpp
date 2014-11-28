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
	, StartingDeferredInterest(0.0)
{
	for (qint32 i = static_cast<qint32>(TrancheFlowType::InterestFlow); i < (static_cast<qint32>(TrancheFlowType::InterestFlow) << 1); ++i) {
		SetLabel(i, QString("Interest %1").arg(i - static_cast<qint32>(TrancheFlowType::InterestFlow) + 1));
	}
	for (qint32 i = static_cast<qint32>(TrancheFlowType::DeferredFlow); i < (static_cast<qint32>(TrancheFlowType::DeferredFlow) << 1); ++i) {
		SetLabel(i, QString("Deferred Interest %1").arg(i - static_cast<qint32>(TrancheFlowType::DeferredFlow) + 1));
	}
	SetLabel(static_cast<qint32>(TrancheFlowType::PrincipalFlow), "Principal");
	SetLabel(static_cast<qint32>(TrancheFlowType::OCFlow), "OC Test");
	SetLabel(static_cast<qint32>(TrancheFlowType::ICFlow), "IC Test");
	SetLabel(static_cast<qint32>(TrancheFlowType::OCTarget), "OC Target");
	SetLabel(static_cast<qint32>(TrancheFlowType::ICTarget), "IC Target");
	SetLabel(static_cast<qint32>(TrancheFlowType::PDLOutstanding), "PDL Outstanding");
	SetLabel(static_cast<qint32>(TrancheFlowType::PDLCured), "PDL Cured");
	Aggregate(Monthly);
}

double TrancheCashFlow::GetAmountOutstanding(int index)const{
	if(index<0 || index>=Count()) return 0.0;
	double Result=0.0;
	if(OutstandingAmt>0){
		Result=OutstandingAmt;
		for (int i = 0; i <= index; i++) Result -= GetFlow(i, static_cast<qint32>(TrancheFlowType::PrincipalFlow));
		if (qAbs(Result)<0.01) return 0.0;
		return Result;
	}
	for (int i = index; i<Count(); i++) Result += GetFlow(i, static_cast<qint32>(TrancheFlowType::PrincipalFlow));
	return Result;
}
TrancheCashFlow::TrancheCashFlow(const TrancheCashFlow& a)
	:OutstandingAmt(a.OutstandingAmt)
	,StartingDeferredInterest(a.StartingDeferredInterest)
	,GenericCashFlow(a)
{}

double TrancheCashFlow::GetPreviousDeferred(const QDate& CurrentDate){
	if(m_CashFlows.isEmpty()) return StartingDeferredInterest;
	return GetPreviousFlow(CurrentDate, static_cast<qint32>(TrancheFlowType::DeferredFlow));
}
double TrancheCashFlow::GetPreviousOutstanding(const QDate& CurrentDate){
	int Index=FindDate(CurrentDate);
	if(Index>0) return GetAmountOutstanding(Index-1);
	return GetAmountOutstanding(Count()-1);
}
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
	QSettings ConfigIni(":/Configs/GlobalConfigs.ini", QSettings::IniFormat);
	ConfigIni.beginGroup("Database");
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(ConfigIni.value("DBtype", "QODBC").toString(), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + ConfigIni.value("Driver", "SQL Server").toString()
			+ "}; "
			+ ConfigIni.value("DataSource", "Server=SYNSERVER2\\SQLExpress;Initial Catalog=ABSDB;Integrated Security=SSPI;Trusted_Connection=Yes;").toString()
			);
	}
	if (db.open()) {
		QSqlQuery query(db);
		query.setForwardOnly(true);
		query.prepare("CALL " + ConfigIni.value("CashFlowsStoredProc", "getCashFlows").toString() + "(?)");
		query.bindValue(0,TrancheID);
		if (query.exec()) {
			bool Cleared = false;
			while (query.next()) {
				if (!Cleared) {
					Cleared = true;
					Clear();
					SetInitialOutstanding(query.value(5).toDouble() + query.value(4).toDouble());
					SetStartingDeferredInterest(0.0);
				}
				AddFlow(query.value(0).toDate(), query.value(3).toDouble(), TrancheFlowType::InterestFlow);
				AddFlow(query.value(0).toDate(), query.value(4).toDouble(), TrancheFlowType::PrincipalFlow);
				AddFlow(query.value(0).toDate(), query.value(2).toDouble(), TrancheFlowType::DeferredFlow);
			}
			db.close();
			ConfigIni.endGroup();
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
		if (i == 0) {
			OutstandingAmt = a.GetTableResult(0, 4)->GetDouble() + a.GetTableResult(0, 5)->GetDouble();
		}
	}
	return true;
}
#endif
TrancheCashFlow TrancheCashFlow::ScaledCashFlows(double NewSize, double OldSize) const {
	TrancheCashFlow Result;
	Result.SetInitialOutstanding(OutstandingAmt * NewSize/OldSize);
	Result.StartingDeferredInterest *= NewSize / OldSize;
	Result.AddFlow(GenericCashFlow::ScaledCashFlows(OldSize, NewSize, QList<qint32>(), QList<qint32>()
		 << static_cast<qint32>(TrancheFlowType::OCFlow)
		 << static_cast<qint32>(TrancheFlowType::ICFlow)
		 << static_cast<qint32>(TrancheFlowType::OCTarget)
		 << static_cast<qint32>(TrancheFlowType::ICTarget)
		));
	return Result;
}

double TrancheCashFlow::GetInterest(int index) const {
	double Result = 0;
	for (qint32 i = static_cast<qint32>(TrancheFlowType::InterestFlow); i < (static_cast<qint32>(TrancheFlowType::InterestFlow) << 1); ++i) {
		Result+=GetFlow(index, i);
	}
	return Result;
}

double TrancheCashFlow::GetInterest(const QDate& a) const {
	double Result = 0;
	for (qint32 i = static_cast<qint32>(TrancheFlowType::InterestFlow); i < (static_cast<qint32>(TrancheFlowType::InterestFlow) << 1); ++i) {
		Result += GetFlow(a, i);
	}
	return Result;
}
double TrancheCashFlow::GetDeferred(int index) const {
	double Result = 0;
	for (qint32 i = static_cast<qint32>(TrancheFlowType::DeferredFlow); i < (static_cast<qint32>(TrancheFlowType::DeferredFlow) << 1); ++i) {
		Result += GetFlow(index, i);
	}
	return Result;
}

double TrancheCashFlow::GetDeferred(const QDate& a) const {
	double Result = 0;
	for (qint32 i = static_cast<qint32>(TrancheFlowType::DeferredFlow); i < (static_cast<qint32>(TrancheFlowType::DeferredFlow) << 1); ++i) {
		Result += GetFlow(a, i);
	}
	return Result;
}

QString TrancheCashFlow::ToXML() const {
	GenericCashFlow NewFlow(*this);
	NewFlow.SetLabel(static_cast<qint32>(MtgCashFlow::MtgFlowType::AmountOutstandingFlow), "Outstanding");
	for (auto i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		NewFlow.AddFlow(i.key(), GetAmountOutstanding(i.key()), static_cast<qint32>(MtgCashFlow::MtgFlowType::AmountOutstandingFlow));
	}
	return NewFlow.ToXML();
}

QString TrancheCashFlow::ToPlainText(bool UseHeaders /*= true*/) const {
	GenericCashFlow NewFlow(*this);
	NewFlow.SetLabel(static_cast<qint32>(MtgCashFlow::MtgFlowType::AmountOutstandingFlow), "Outstanding");
	for (auto i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		NewFlow.AddFlow(i.key(), GetAmountOutstanding(i.key()), static_cast<qint32>(MtgCashFlow::MtgFlowType::AmountOutstandingFlow));
	}
	return NewFlow.ToPlainText(UseHeaders);
}


