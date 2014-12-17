#include "MtgCalculator.h"
#include "Mortgage.h"
#include <QSettings>
#include <QDir>
#include "Waterfall.h"
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSettings>
#include <QVariant>
#endif
MtgCalculator::MtgCalculator(QObject* parent)
	:TemplAsyncCalculator <MtgCalculatorThread, MtgCashFlow>(parent)
	,m_UseStoredCashFlows(true)
{}
void MtgCalculator::AddLoan(const Mortgage& a, qint32 Index) {
	auto FoundLn = Loans.find(Index);
	if (FoundLn != Loans.end()) {
		delete FoundLn.value();
		Loans.erase(FoundLn);
	}
	Loans.insert(Index, new Mortgage(a));
}
bool MtgCalculator::StartCalculation() {
	if (m_ContinueCalculation) return false;
	if (!ReadyToCalculate().isEmpty()) return false;
	{//Check if all base rates are valid
		bool CheckAgain = false;
		ConstantBaseRateTable TempTable;
		for (auto i = Loans.constBegin(); i != Loans.constEnd(); ++i) {
			if (i.value()->GetFloatingRateValue().IsEmpty()) {
				i.value()->CompileReferenceRateValue(TempTable);
				CheckAgain = true;
			}
		}
		for (auto i = Loans.constBegin(); CheckAgain && i != Loans.constEnd(); ++i) {
			if (i.value()->GetFloatingRateValue().IsEmpty()) return false;
		}
	}

	/*if (m_UseStoredCashFlows){//Check if CF are available
		QString DealName,TrancheName;
		bool CashFound=false;
#ifndef NO_DATABASE
		bool DBAvailable;
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
		DBAvailable = db.open();
		ConfigIni.endGroup();
#endif
		MtgCashFlow ResultingFlow;
		ConfigIni.beginGroup("Folders");
		QDir CLOMsFolder(ConfigIni.value("UnifiedResultsFolder", "Z:/24AM/Monitoring/Model Results").toString());
		ConfigIni.endGroup();
		for (auto i = Loans.constBegin(); i != Loans.constEnd(); ++i) {
			ResultingFlow.Clear();
			if (CLOMsFolder.exists()) {
				DealName = i.value()->GetProperty("Issuer");
				TrancheName = i.value()->GetProperty("Facility");
				if (DealName.isEmpty()) DealName = TrancheName;
				if (DealName.isEmpty()) continue;
				if (CLOMsFolder.exists(DealName + ".clom")) {
					QFile SingleClom(CLOMsFolder.absoluteFilePath(DealName + ".clom"));
					if (SingleClom.open(QIODevice::ReadOnly)) {
						QDataStream out(&SingleClom);
						qint32 TempInt;
						out.setVersion(QDataStream::Qt_5_3);
						out >> TempInt;
						if (TempInt >= MinimumSupportedVersion && TempInt <= ModelVersionNumber) {
								{QDate Junk;  out >> Junk; } //drop LiborUpdateDate
							bool BaseToCall;
							out >> BaseToCall; //drop UseForwardCurve
							out >> BaseToCall;
							Waterfall TargetDeal;
							out >> TargetDeal;
							if (BaseToCall) out >> TargetDeal;
							const Tranche* TargetTranche = TargetDeal.GetTranche(TrancheName);
							TrancheCashFlow ScaledFlows = TargetTranche->GetCashFlow().ScaledCashFlows(i.value()->GetSize());
							for (int j = 0; j < ScaledFlows.Count(); ++j) {
								ResultingFlow.AddFlow(ScaledFlows.GetDate(j), ScaledFlows.GetInterest(j), MtgCashFlow::MtgFlowType::InterestFlow);
								ResultingFlow.AddFlow(ScaledFlows.GetDate(j), ScaledFlows.GetPrincipal(j), MtgCashFlow::MtgFlowType::PrincipalFlow);
								double TempOut = ScaledFlows.GetAmountOutstanding(j);
								ResultingFlow.AddFlow(ScaledFlows.GetDate(j), TempOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
								ResultingFlow.AddFlow(ScaledFlows.GetDate(j), TempOut*TargetTranche->GetTotalCoupon(TargetTranche->GetCashFlow().GetDate(j)), MtgCashFlow::MtgFlowType::WACouponFlow);
							}
							CashFound = true;
						}
						SingleClom.close();
					}
				}
			}
			#ifndef NO_DATABASE
			if (!CashFound && DBAvailable) {
				ConfigIni.beginGroup("Database");
				QSqlQuery query(db);
				query.prepare("exec " + ConfigIni.value("CashFlowsStoredProc", "getCashFlows").toString() + " ?");
				ConfigIni.endGroup();
				query.setForwardOnly(true);
				query.bindValue(0, TrancheName);
				bool FirstTime = true;
				if (query.exec()) {
					TrancheCashFlow ScaledFlows;
					while (query.next()) {
						if (FirstTime) {
							ScaledFlows.SetInitialOutstanding(query.value(5).toDouble() + query.value(4).toDouble());
							ScaledFlows.SetStartingDeferredInterest(0.0);
							FirstTime = false;
						}
						ScaledFlows.AddFlow(query.value(0).toDate(), query.value(3).toDouble(), TrancheCashFlow::TrancheFlowType::InterestFlow);
						ScaledFlows.AddFlow(query.value(0).toDate(), query.value(4).toDouble(), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
						ScaledFlows.AddFlow(query.value(0).toDate(), query.value(1).toDouble(), static_cast<qint32>(MtgCashFlow::MtgFlowType::WACouponFlow));
					}
					if (!FirstTime) {
						ScaledFlows = ScaledFlows.ScaledCashFlows(i.value()->GetSize());
						for (int j = 0; j < ScaledFlows.Count(); ++j) {
							ResultingFlow.AddFlow(ScaledFlows.GetDate(j), ScaledFlows.GetInterest(j), MtgCashFlow::MtgFlowType::InterestFlow);
							ResultingFlow.AddFlow(ScaledFlows.GetDate(j), ScaledFlows.GetPrincipal(j), MtgCashFlow::MtgFlowType::PrincipalFlow);
							double TempOut = ScaledFlows.GetAmountOutstanding(j);
							ResultingFlow.AddFlow(ScaledFlows.GetDate(j), TempOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
							ResultingFlow.AddFlow(ScaledFlows.GetDate(j), TempOut*ScaledFlows.GetFlow(j, static_cast<qint32>(MtgCashFlow::MtgFlowType::WACouponFlow)), MtgCashFlow::MtgFlowType::WACouponFlow);
						}
					}
				}
				CashFound = !FirstTime;	
			}
			#endif
			if (CashFound) {
				BeesSent.append(i.key());
				BeeReturned(i.key(), ResultingFlow);
			}
		}
		#ifndef NO_DATABASE
		if (DBAvailable) db.close();
		#endif
	}*/


	BeesReturned = 0;
	BeesSent.clear();
	m_ContinueCalculation = true;
	int NumberOfThreads = QThread::idealThreadCount();
	if (m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	MtgCalculatorThread* CurrentThread;
	for (auto SingleLoan = Loans.constBegin(); SingleLoan != Loans.constEnd(); ++SingleLoan) {
		if (NumofSent >= NumberOfThreads) break;
		if (BeesSent.contains(SingleLoan.key())) continue;
		CurrentThread = AddThread(SingleLoan.key());
		CurrentThread->SetLoan(*(SingleLoan.value()));
		CurrentThread->SetCPR(m_CPRass);
		CurrentThread->SetCDR(m_CDRass);
		CurrentThread->SetLS(m_LSass);
		CurrentThread->SetRecoveryLag(m_RecoveryLag);
		CurrentThread->SetDelinquency(m_Delinquency);
		CurrentThread->SetDelinquencyLag(m_DelinquencyLag);
		CurrentThread->SetOverrideAssumptions(m_OverrideAssumptions);
		CurrentThread->SetStartDate(StartDate);
		CurrentThread->start();
		++NumofSent;
	}
	return true;
}
void MtgCalculator::BeeReturned(int Ident, const MtgCashFlow& a) {
	m_AggregatedRes+=a;
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::BeeReturned(Ident, a);
	if (!m_ContinueCalculation)return;
	MtgCalculatorThread* CurrentThread;
	for (auto SingleLoan = Loans.constBegin(); SingleLoan != Loans.constEnd(); ++SingleLoan) {
		if (BeesSent.contains(SingleLoan.key())) continue;
		CurrentThread = AddThread(SingleLoan.key());
		CurrentThread->SetLoan(*(SingleLoan.value()));
		CurrentThread->SetCPR(m_CPRass);
		CurrentThread->SetCDR(m_CDRass);
		CurrentThread->SetLS(m_LSass);
		CurrentThread->SetRecoveryLag(m_RecoveryLag);
		CurrentThread->SetDelinquency(m_Delinquency);
		CurrentThread->SetDelinquencyLag(m_DelinquencyLag);
		CurrentThread->SetOverrideAssumptions(m_OverrideAssumptions);
		CurrentThread->SetStartDate(StartDate);
		CurrentThread->start();
		return;
	}
}

void MtgCalculator::ClearLoans() {
	for (auto i = Loans.begin(); i != Loans.end(); i++) {
		delete i.value();
	}
	Loans.clear();
}
void MtgCalculator::Reset(){
	ClearLoans();
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::Reset();
}
QString MtgCalculator::ReadyToCalculate()const{
	QString Result;
	QString TempStr;
	if (StartDate.isNull()) Result += "Invalid Start Date\n";
	for (auto i=Loans.constBegin();i!=Loans.constEnd();i++){
		TempStr = i.value()->ReadyToCalculate();
		if(!TempStr.isEmpty()) Result+=TempStr+'\n';
	}
	if (BloombergVector(m_CPRass).IsEmpty(0.0,1.0)) Result += "CPR Vector: " + m_CPRass + '\n';
	if (BloombergVector(m_CDRass).IsEmpty(0.0, 1.0)) Result += "CDR Vector: " + m_CDRass + '\n';
	if (BloombergVector(m_LSass).IsEmpty()) Result += "LS Vector: " + m_LSass + '\n';
	if (IntegerVector(m_RecoveryLag).IsEmpty()) Result += "Recovery Lag Vector: " + m_RecoveryLag + '\n';
	if (BloombergVector(m_Delinquency).IsEmpty(0, 1.0)) Result += "Delinquency Vector Vector: " + m_Delinquency + '\n';
	if (IntegerVector(m_DelinquencyLag).IsEmpty()) Result += "Delinquency Lag Vector: " + m_DelinquencyLag + '\n';
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
	return Result;
}

QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows) {
	stream
		<< static_cast<qint32>(flows.Loans.size())
		<< flows.m_UseStoredCashFlows
		<< flows.m_CPRass
		<< flows.m_CDRass
		<< flows.m_LSass
		<< flows.m_RecoveryLag
		<< flows.m_Delinquency
		<< flows.m_DelinquencyLag
		<< flows.StartDate
		;
	for (auto i = flows.Loans.begin(); i != flows.Loans.end(); i++) {
		stream << i.key() << *(i.value());
	}
	return flows.SaveToStream(stream);
}
QDataStream& MtgCalculator::LoadOldVersion(QDataStream& stream) {
	Reset();
	qint32 tempInt, TempKey;
	stream >> tempInt;
	stream >> m_UseStoredCashFlows;
	stream >> m_CPRass;
	stream >> m_CDRass;
	stream >> m_LSass;
	stream >> m_RecoveryLag;
	stream >> m_Delinquency;
	stream >> m_DelinquencyLag;
	stream >> StartDate;
	Mortgage TmpMtg;
	for (qint32 i = 0; i < tempInt; i++) {
		stream >> TempKey;
		TmpMtg.SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> TmpMtg;
		AddLoan(TmpMtg, TempKey);
	}
	return TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::LoadOldVersion(stream);
}

void MtgCalculator::CompileReferenceRateValue(ForwardBaseRateTable& Values) {
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->CompileReferenceRateValue(Values);
}
void MtgCalculator::CompileReferenceRateValue(ConstantBaseRateTable& Values) {
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->CompileReferenceRateValue(Values);
}
#ifndef NO_DATABASE
void MtgCalculator::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) {
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->GetBaseRatesDatabase(Values, DownloadAll);
}
void MtgCalculator::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) {
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->GetBaseRatesDatabase(Values, DownloadAll);
}
#endif
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows) {
	return flows.LoadOldVersion(stream);
}
void MtgCalculator::SetLoans(const QHash<qint32, Mortgage*>& a) {
	ClearLoans();
	for (auto i = a.constBegin(); i != a.constEnd(); ++i) {
		Loans.insert(i.key(), new Mortgage(*(i.value())));
	}
}
void MtgCalculator::ClearResults() {
	m_AggregatedRes.Clear();
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::ClearResults();
}
