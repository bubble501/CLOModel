#include "MtgCalculator.h"
#include "Mortgage.h"
#include <QDir>
#include "Waterfall.h"
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#endif
#include <QProcessEnvironment>
#include "LoanAssumption.h"
#include <QCache>
#include "simstring.h"
#include <QXmlStreamReader>
MtgCalculator::MtgCalculator(QObject* parent)
	:TemplAsyncCalculator <MtgCalculatorThread, MtgCashFlow>(parent)
	,m_UseStoredCashFlows(true)
	, m_DownloadScenario(false)
{}
void MtgCalculator::AddLoan(const Mortgage& a, qint32 Index) {
	RETURN_WHEN_RUNNING(true, )
	auto FoundLn = Loans.find(Index);
	if (FoundLn != Loans.end()) {
		delete FoundLn.value();
		Loans.erase(FoundLn);
	}
	Loans.insert(Index, new Mortgage(a));
}
bool MtgCalculator::StartCalculation() {
	RETURN_WHEN_RUNNING(true, false)
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
		if (TempProperties.contains(SingleLoan.key())) {
			const auto CurrProps = TempProperties.value(SingleLoan.key());
			for (auto j = CurrProps->constBegin(); j != CurrProps->constEnd(); ++j) {
				CurrentThread->SetLoanProperty(j.key(), j.value());
			}
		}
		CurrentThread->SetCPR(m_CPRass);
		CurrentThread->SetCDR(m_CDRass);
		CurrentThread->SetLS(m_LSass);
		CurrentThread->SetRecoveryLag(m_RecoveryLag);
		CurrentThread->SetDelinquency(m_Delinquency);
		CurrentThread->SetDelinquencyLag(m_DelinquencyLag);
		CurrentThread->SetOverrideAssumptions(m_OverrideAssumptions);
		CurrentThread->SetStartDate(StartDate);
		CurrentThread->SetDownloadScenario(m_DownloadScenario);
		CurrentThread->start();
		++NumofSent;
	}
	return true;
}
void MtgCalculator::BeeReturned(int Ident, const MtgCashFlow& a) {
	RETURN_WHEN_RUNNING(false, )
	m_AggregatedRes+=a;
	if (Loans.contains(Ident)) Loans[Ident]->SetCashFlows(a);
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::BeeReturned(Ident, a);
	if (!m_ContinueCalculation)return;
	MtgCalculatorThread* CurrentThread;
	for (auto SingleLoan = Loans.constBegin(); SingleLoan != Loans.constEnd(); ++SingleLoan) {
		if (BeesSent.contains(SingleLoan.key())) continue;
		CurrentThread = AddThread(SingleLoan.key());
		CurrentThread->SetLoan(*(SingleLoan.value()));
		if (TempProperties.contains(SingleLoan.key())) {
			const auto CurrProps = TempProperties.value(SingleLoan.key());
			for (auto j = CurrProps->constBegin(); j != CurrProps->constEnd(); ++j) {
				CurrentThread->SetLoanProperty(j.key(), j.value());
			}
		}
		CurrentThread->SetCPR(m_CPRass);
		CurrentThread->SetCDR(m_CDRass);
		CurrentThread->SetLS(m_LSass);
		CurrentThread->SetRecoveryLag(m_RecoveryLag);
		CurrentThread->SetDelinquency(m_Delinquency);
		CurrentThread->SetDelinquencyLag(m_DelinquencyLag);
		CurrentThread->SetOverrideAssumptions(m_OverrideAssumptions);
		CurrentThread->SetStartDate(StartDate);
		CurrentThread->SetDownloadScenario(m_DownloadScenario);
		CurrentThread->start();
		return;
	}
}

void MtgCalculator::ClearLoans() {
	RETURN_WHEN_RUNNING(true, )
	for (auto i = Loans.begin(); i != Loans.end(); i++) {
		delete i.value();
	}
	Loans.clear();
}
void MtgCalculator::Reset(){
	RETURN_WHEN_RUNNING(true, )
	ClearLoans();
	ClearTempProperties();
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::Reset();
}
QString MtgCalculator::ReadyToCalculate()const{
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n" )
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
	if (flows.m_ContinueCalculation) return stream;
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
	RETURN_WHEN_RUNNING(true, stream)
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
	RETURN_WHEN_RUNNING(true, )
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->CompileReferenceRateValue(Values);
}
void MtgCalculator::CompileReferenceRateValue(ConstantBaseRateTable& Values) {
	RETURN_WHEN_RUNNING(true, )
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->CompileReferenceRateValue(Values);
}
#ifndef NO_DATABASE
void MtgCalculator::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) {
	RETURN_WHEN_RUNNING(true, )
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->GetBaseRatesDatabase(Values, DownloadAll);
}
void MtgCalculator::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) {
	RETURN_WHEN_RUNNING(true, )
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->GetBaseRatesDatabase(Values, DownloadAll);
}
#endif
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows) {
	return flows.LoadOldVersion(stream);
}
void MtgCalculator::SetLoans(const QHash<qint32, Mortgage*>& a) {
	RETURN_WHEN_RUNNING(true, )
	ClearLoans();
	for (auto i = a.constBegin(); i != a.constEnd(); ++i) {
		Loans.insert(i.key(), new Mortgage(*(i.value())));
	}
}
void MtgCalculator::ClearResults() {
	RETURN_WHEN_RUNNING(true, )
	m_AggregatedRes.Clear();
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::ClearResults();
}
#ifndef NO_DATABASE
void MtgCalculator::DownloadScenarios() {
	RETURN_WHEN_RUNNING(true, )
	ClearTempProperties();
	QCache<QString, LoanAssumption> AssumptionCache;
	for (auto i = Loans.constBegin(); i != Loans.constEnd(); ++i) { 
		if (i.value()->HasProperty("Scenario") && i.value()->HasProperty("Mezzanine")) {
			if (!AssumptionCache.contains(i.value()->GetProperty("Scenario"))) {
				LoanAssumption* DownloadedAssumption = new LoanAssumption(i.value()->GetProperty("Scenario"));
				Db_Mutex.lock();
				QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
				if (!db.isValid()) {
					if (!db.isValid()) {
						db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype", "QODBC"), "TwentyFourDB");
						db.setDatabaseName(
							"Driver={" + GetFromConfig("Database", "Driver", "SQL Server")
							+ "}; "
							+ GetFromConfig("Database", "DataSource", R"(Server=SYNSERVER2\SQLExpress; Initial Catalog = ABSDB; Integrated Security = SSPI; Trusted_Connection = Yes;)")
							);
						
					}
				}
				bool DbOpen = db.isOpen();
				if (!DbOpen) DbOpen = db.open();
				if (DbOpen) {
					QSqlQuery LoanAssQuerry(db);
					LoanAssQuerry.setForwardOnly(true);
					LoanAssQuerry.prepare("{CALL " + GetFromConfig("Database", "GetLoanAssumptionStoredProc", "getLoanAssumption(:scenarioName,:isSenior)") + "}");
					LoanAssQuerry.bindValue(":scenarioName", i.value()->GetProperty("Scenario"));
					LoanAssQuerry.bindValue(":isSenior", true);
					if (LoanAssQuerry.exec()) {
						if (LoanAssQuerry.next()) {
							auto DbgRecord = LoanAssQuerry.record();
							int FieldCount = 0;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorLastUpdate(DbgRecord.value(FieldCount).toDate()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorMaturityExtension(QString::number(DbgRecord.value(FieldCount).toInt())); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorInitialHaircut(QString::number(DbgRecord.value(FieldCount).toDouble(),'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorPrepaymentFee(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorDayCount(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorCPR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorCDR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorLS(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorRecoveryLag ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorDelinquency (DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorDelinquencyLag (DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorPrice(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorHaircut ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorPrepayMultiplier ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorLossMultiplier ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
						}
					}
					LoanAssQuerry.bindValue(":isSenior", false);
					if (LoanAssQuerry.exec()) {
						if (LoanAssQuerry.next()) {
							auto DbgRecord = LoanAssQuerry.record();
							int FieldCount = 0;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzLastUpdate(DbgRecord.value(FieldCount).toDate()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzMaturityExtension(QString::number(DbgRecord.value(FieldCount).toInt())); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzInitialHaircut(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzPrepaymentFee(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzDayCount(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzCPR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzCDR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzLS(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzRecoveryLag(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzDelinquency(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzDelinquencyLag(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzPrice(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzHaircut(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzPrepayMultiplier(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzLossMultiplier(DbgRecord.value(FieldCount).toString()); ++FieldCount;
						}
					}
				}
				Db_Mutex.unlock();
				AssumptionCache.insert(DownloadedAssumption->GetScenarioName(), DownloadedAssumption);
			}
			const bool OverrideCurrent=
				#ifdef Assumptions_ExcelOverDB
					false
				#else
					true
				#endif
			;
			if (i.value()->GetProperty("Mezzanine").compare("Yes", Qt::CaseInsensitive) == 0) {
				auto CurrentAss = AssumptionCache.object(i.value()->GetProperty("Scenario"));
				if ((OverrideCurrent || !i.value()->HasProperty("MaturityExtension"))
					&&!CurrentAss->GetRawMezzMaturityExtension().isEmpty()) AddTempProperty(i.key(),"MaturityExtension", CurrentAss->GetMezzMaturityExtension());
				if ((OverrideCurrent || !i.value()->HasProperty("StartingHaircut"))
					&& !CurrentAss->GetRawMezzInitialHaircut().isEmpty())  AddTempProperty(i.key(),"StartingHaircut", CurrentAss->GetMezzHaircut());
				if ((OverrideCurrent || !i.value()->HasProperty("PrepaymentFee"))
					&& !CurrentAss->GetRawMezzPrepaymentFee().isEmpty())AddTempProperty(i.key(),"PrepaymentFee", CurrentAss->GetMezzPrepaymentFee());
				if ((OverrideCurrent || !i.value()->HasProperty("DayCount"))
					&&!CurrentAss->GetRawMezzDayCount().isEmpty()) AddTempProperty(i.key(),"DayCount", CurrentAss->GetMezzDayCount());
				if ((OverrideCurrent || !i.value()->HasProperty("Haircut"))
					&& !CurrentAss->GetRawMezzHaircut().isEmpty()) AddTempProperty(i.key(),"Haircut", CurrentAss->GetMezzHaircut());
				if ((OverrideCurrent || !i.value()->HasProperty("PrepayMultiplier"))
					&& !CurrentAss->GetRawMezzPrepayMultiplier().isEmpty())AddTempProperty(i.key(),"PrepayMultiplier", CurrentAss->GetMezzPrepayMultiplier());
				if ((OverrideCurrent || !i.value()->HasProperty("LossMultiplier"))
					&& !CurrentAss->GetRawMezzLossMultiplier().isEmpty()) AddTempProperty(i.key(),"LossMultiplier", CurrentAss->GetMezzLossMultiplier());
				if ((OverrideCurrent || !i.value()->HasProperty("CPR"))
					&& !CurrentAss->GetRawMezzCPR().isEmpty()) AddTempProperty(i.key(),"CPR", CurrentAss->GetMezzCPR());
				if ((OverrideCurrent || !i.value()->HasProperty("CDR"))
					&& !CurrentAss->GetRawMezzCDR().isEmpty()) AddTempProperty(i.key(),"CDR", CurrentAss->GetMezzCDR());
				if ((OverrideCurrent || !i.value()->HasProperty("LS"))
					&& !CurrentAss->GetRawMezzLS().isEmpty()) AddTempProperty(i.key(),"LS", CurrentAss->GetMezzLS());
				if ((OverrideCurrent || !i.value()->HasProperty("RecoveryLag"))
					&& !CurrentAss->GetRawMezzRecoveryLag().isEmpty()) AddTempProperty(i.key(),"RecoveryLag", CurrentAss->GetMezzRecoveryLag());
				if ((OverrideCurrent || !i.value()->HasProperty("Delinquency"))
					&& !CurrentAss->GetRawMezzDelinquency().isEmpty())  AddTempProperty(i.key(),"Delinquency", CurrentAss->GetMezzDelinquency());
				if ((OverrideCurrent || !i.value()->HasProperty("DelinquencyLag"))
					&& !CurrentAss->GetRawMezzDelinquencyLag().isEmpty())  AddTempProperty(i.key(),"DelinquencyLag", CurrentAss->GetMezzDelinquencyLag());
				if ((OverrideCurrent || !i.value()->HasProperty("Price"))
					&& !CurrentAss->GetRawMezzPrice().isEmpty())  AddTempProperty(i.key(),"Price", CurrentAss->GetMezzPrice());
			}
			else {
				auto CurrentAss = AssumptionCache.object(i.value()->GetProperty("Scenario"));
				if ((OverrideCurrent || !i.value()->HasProperty("MaturityExtension"))
					&& !CurrentAss->GetRawSeniorMaturityExtension().isEmpty()) AddTempProperty(i.key(),"MaturityExtension", CurrentAss->GetSeniorMaturityExtension());
				if ((OverrideCurrent || !i.value()->HasProperty("StartingHaircut"))
					&& !CurrentAss->GetRawSeniorInitialHaircut().isEmpty())  AddTempProperty(i.key(),"StartingHaircut", CurrentAss->GetSeniorHaircut());
				if ((OverrideCurrent || !i.value()->HasProperty("PrepaymentFee"))
					&& !CurrentAss->GetRawSeniorPrepaymentFee().isEmpty())AddTempProperty(i.key(),"PrepaymentFee", CurrentAss->GetSeniorPrepaymentFee());
				if ((OverrideCurrent || !i.value()->HasProperty("DayCount"))
					&& !CurrentAss->GetRawSeniorDayCount().isEmpty()) AddTempProperty(i.key(),"DayCount", CurrentAss->GetSeniorDayCount());
				if ((OverrideCurrent || !i.value()->HasProperty("Haircut"))
					&& !CurrentAss->GetRawSeniorHaircut().isEmpty()) AddTempProperty(i.key(),"Haircut", CurrentAss->GetSeniorHaircut());
				if ((OverrideCurrent || !i.value()->HasProperty("PrepayMultiplier"))
					&& !CurrentAss->GetRawSeniorPrepayMultiplier().isEmpty())AddTempProperty(i.key(),"PrepayMultiplier", CurrentAss->GetSeniorPrepayMultiplier());
				if ((OverrideCurrent || !i.value()->HasProperty("LossMultiplier"))
					&& !CurrentAss->GetRawSeniorLossMultiplier().isEmpty()) AddTempProperty(i.key(),"LossMultiplier", CurrentAss->GetSeniorLossMultiplier());
				if ((OverrideCurrent || !i.value()->HasProperty("CPR"))
					&& !CurrentAss->GetRawSeniorCPR().isEmpty()) AddTempProperty(i.key(),"CPR", CurrentAss->GetSeniorCPR());
				if ((OverrideCurrent || !i.value()->HasProperty("CDR"))
					&& !CurrentAss->GetRawSeniorCDR().isEmpty()) AddTempProperty(i.key(),"CDR", CurrentAss->GetSeniorCDR());
				if ((OverrideCurrent || !i.value()->HasProperty("LS"))
					&& !CurrentAss->GetRawSeniorLS().isEmpty()) AddTempProperty(i.key(),"LS", CurrentAss->GetSeniorLS());
				if ((OverrideCurrent || !i.value()->HasProperty("RecoveryLag"))
					&& !CurrentAss->GetRawSeniorRecoveryLag().isEmpty()) AddTempProperty(i.key(),"RecoveryLag", CurrentAss->GetSeniorRecoveryLag());
				if ((OverrideCurrent || !i.value()->HasProperty("Delinquency"))
					&& !CurrentAss->GetRawSeniorDelinquency().isEmpty())  AddTempProperty(i.key(),"Delinquency", CurrentAss->GetSeniorDelinquency());
				if ((OverrideCurrent || !i.value()->HasProperty("DelinquencyLag"))
					&& !CurrentAss->GetRawSeniorDelinquencyLag().isEmpty())  AddTempProperty(i.key(),"DelinquencyLag", CurrentAss->GetSeniorDelinquencyLag());
				if ((OverrideCurrent || !i.value()->HasProperty("Price"))
					&& !CurrentAss->GetRawSeniorPrice().isEmpty())  AddTempProperty(i.key(),"Price", CurrentAss->GetSeniorPrice());
			}
		}
	}
}

void MtgCalculator::GuessLoanScenarios(bool OverrideAss) {
	RETURN_WHEN_RUNNING(true, )
	QHash<QString,LoanAssumption*> AvailableAssumptions;
	Db_Mutex.lock();
	{
		QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
		if (!db.isValid()) {
			if (!db.isValid()) {
				db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype", "QODBC"), "TwentyFourDB");
				db.setDatabaseName(
					"Driver={" + GetFromConfig("Database", "Driver", "SQL Server")
					+ "}; "
					+ GetFromConfig("Database", "DataSource", R"(Server=SYNSERVER2\SQLExpress; Initial Catalog = ABSDB; Integrated Security = SSPI; Trusted_Connection = Yes;)")
					);
				
			}
		}
		bool DbOpen = db.isOpen();
		if (!DbOpen) DbOpen = db.open();
		if (DbOpen) {
			QSqlQuery LoanAssQuerry(db);
			LoanAssQuerry.setForwardOnly(true);
			LoanAssQuerry.prepare("{CALL " + GetFromConfig("Database", "GetAllLoanAssumptionsStoredProc", "getAllLoanAssumptions") + "}");
			if (LoanAssQuerry.exec()) {
				while (LoanAssQuerry.next()) {
					auto DbgRecord = LoanAssQuerry.record();
					if (AvailableAssumptions.contains(DbgRecord.value(0).toString())) continue;
					auto CurrAss = AvailableAssumptions.insert(DbgRecord.value(0).toString(), new LoanAssumption(DbgRecord.value(0).toString()));
					(*CurrAss)->SetAliases(DbgRecord.value(1).toString());
				}
			}
		}
	}
	Db_Mutex.unlock();
	foreach(Mortgage* SingleLoan, Loans) {
		for (auto i = AvailableAssumptions.constBegin(); i != AvailableAssumptions.constEnd(); ++i) {
			for (const QString& CurrProperty : LoansPropertiesToSearch) {
				if (i.value()->MatchPattern(SingleLoan->GetProperty(CurrProperty))) {
					if (!SingleLoan->HasProperty("Scenario") || OverrideAss)
						SingleLoan->SetProperty("Scenario", i.key());
					break;
				}
			}
		}
	}
	for (auto i = AvailableAssumptions.begin(); i != AvailableAssumptions.end(); ++i) {
		delete i.value();
	}
}



#endif
void MtgCalculator::ClearTempProperties() {
	RETURN_WHEN_RUNNING(true, )
	for (auto i = TempProperties.begin(); i != TempProperties.end(); ++i)
		delete i.value();
	TempProperties.clear();
}

void MtgCalculator::AddTempProperty(qint32 LoanID, const QString& PropertyName, const QString& PropertyValue) {
	RETURN_WHEN_RUNNING(true, )
	if (!Loans.contains(LoanID) || PropertyName.isEmpty() || PropertyValue.isEmpty())return;
	auto iter=TempProperties.find(LoanID);
	if (iter == TempProperties.end()) iter=TempProperties.insert(LoanID, new QHash<QString, QString>());
	iter.value()->operator[](PropertyName) = PropertyValue;
}

QHash<QString, double> MtgCalculator::GetGeographicBreakdown() const {
	QHash<QString, double> Result;
	auto InsertUnknown([&Result](const QString& check, double Size) ->bool {
		if (check.isEmpty()) {
			auto Curres = Result.find("Unknown");
			if (Curres == Result.end()) Result.insert("Unknown", Size);
			else Curres.value() += Size;
			return true;
		}
		return false;
	});
	double SumOut=0.0;
	if (Loans.isEmpty()) return Result;
	QString CurrentGuess;
	simstring::reader dbr;
	const std::string DbRootFolder(qgetenv("CLO_MODEL_FOLDER").constData());
	if (DbRootFolder.empty()) return Result;
	if (!QFile::exists(QString::fromStdString(DbRootFolder + "CountriesDB\\ISO3166-1.ssdb"))) BuildDBCountries(QString::fromStdString(DbRootFolder + "CountriesDB\\ISO3166-1.ssdb"));
	if (!dbr.open(DbRootFolder + "CountriesDB\\ISO3166-1.ssdb")) return Result;
	for (auto i = Loans.constBegin(); i != Loans.constEnd(); ++i) {
		if ((*i)->GetSize()<0.01) continue;
		SumOut += (*i)->GetSize();
		CurrentGuess = (*i)->GetProperty("Country");
		if (!InsertUnknown(CurrentGuess, (*i)->GetSize())) {
			CurrentGuess = GetGeography(CurrentGuess, dbr);
			if (!InsertUnknown(CurrentGuess, (*i)->GetSize())) {
				CurrentGuess = GetCountryISOCode(CurrentGuess);
				if (!InsertUnknown(CurrentGuess, (*i)->GetSize())) {
					auto Curres = Result.find(CurrentGuess);
					if (Curres==Result.end()) Result.insert(CurrentGuess, (*i)->GetSize());
					else Curres.value() += (*i)->GetSize();
				}
			}
		}
	}
	dbr.close();
	if (SumOut <= 0.0 || Result.isEmpty()) return QHash<QString, double>();
	for (auto i = Result.begin(); i != Result.end(); ++i) {
		i.value() /=  SumOut;
	}
	return Result;
}

QString MtgCalculator::GetGeography(const QString& guess, simstring::reader& dbr) const{
	double simThreshold=0.6;
	std::vector<std::wstring> simMatches;
	do {
		simMatches.clear();
		dbr.retrieve(guess.toLower().toStdWString(), simstring::cosine, simThreshold, std::back_inserter(simMatches));
		simThreshold += 0.1;
	} while (simMatches.size() > 1 && simThreshold<1.0);
	if (simMatches.empty()) return QString();
	return QString::fromStdWString(simMatches.front());
}

QString MtgCalculator::GetCountryISOCode(QString name) const {
	name = name.trimmed().toLower();
	QFile InputFile(":/DataSources/ISO3166-1.xml");
	InputFile.open(QIODevice::ReadOnly);
	bool NameMatched = false;
	QString CurrentCode;
	bool CountryFound = false;
	bool NameFound = false;
	bool CodeFound = false;
	QXmlStreamReader xml(&InputFile);
	QStringList PossibleNames;
	while (!xml.atEnd() && !xml.hasError()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "Country") {
				if (CountryFound) return QString(); 
				CountryFound = true;
			}
			else if (xml.name() == "Name") {
				if (NameFound || CodeFound || !CountryFound) return QString();
				NameFound = true;
			}
			else if (xml.name() == "Alpha-2") {
				if (NameFound || CodeFound || !CountryFound) return QString();
				CodeFound = true;
			}
		}
		else if (xml.isEndElement()) {
			if (xml.name() == "Country") {
				if (!CountryFound || NameFound || CodeFound) return QString();
				CountryFound = false;
				NameMatched = false;
				CurrentCode.clear();
			}
			else if (xml.name() == "Name") {
				if (!NameFound) return QString();
				NameFound = false;
			}
			else if (xml.name() == "Alpha-2") {
				if (!CodeFound) return QString();
				CodeFound = false;
			}
		}
		else if (xml.isCharacters()) {
			if (NameFound) {
				PossibleNames = xml.text().toString().split("#,#", QString::SkipEmptyParts);
				for (auto pni = PossibleNames.constBegin(); !NameMatched && pni != PossibleNames.constEnd(); ++pni) {
					if (name.compare(*pni, Qt::CaseInsensitive) == 0) 
						NameMatched = true;
				}
			}
			else if (CodeFound) {
				CurrentCode = xml.text().toString();
			}
			if ((NameFound || CodeFound) && NameMatched && !CurrentCode.isEmpty()) {
				return CurrentCode.trimmed().toUpper();
			}
		}
	}
	return QString();
}
