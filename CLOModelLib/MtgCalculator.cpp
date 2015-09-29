#include "MtgCalculator.h"
#include "Private/MtgCalculator_p.h"
#include "Private/InternalItems.h"
#include "Mortgage.h"
#include <QDir>
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
#include "BaseRateTable.h"
#include <QTemporaryFile>
DEFINE_PUBLIC_QOBJECT_COMMONS(MtgCalculator)
MtgCalculator::~MtgCalculator()
{

}
MtgCalculatorPrivate::~MtgCalculatorPrivate()
{

}
MtgCalculatorPrivate::MtgCalculatorPrivate(MtgCalculator *q)
    :AbstrAsyncCalculatorPrivate(q)
    , m_dataDir(QDir::tempPath() + '/')
    , m_UseStoredCashFlows(false)
    , m_DownloadScenario(false)
{}
MtgCalculator::MtgCalculator(MtgCalculatorPrivate *d, QObject* parent)
    :TemplAsyncCalculator <MtgCalculatorThread, MtgCashFlow>(d,parent)
{
}

QString MtgCalculatorPrivate::writeTempFile(const Mortgage& val) const
{
    QTemporaryFile destFile(m_dataDir.path() + '/');
    destFile.setAutoRemove(false);
    if (destFile.open()) {
        QDataStream out(&destFile);
        out.setVersion(StreamVersionUsed);
        out << val;
        destFile.close();
        return destFile.fileName();
    }
    PrintToTempFile("PermissionError", "Could not write temporary file to save Mortgage");
    return QString();
}
void MtgCalculatorPrivate::clearTempDir()
{
    const auto fileList = QDir(m_dataDir.path()).entryInfoList();
    for (auto i = fileList.constBegin(); i != fileList.constEnd(); ++i)
        QFile::remove(i->absoluteFilePath());
}
void MtgCalculatorPrivate::removeTempFile(const QString& path) const
{
    QFile::remove(path);
}
Mortgage MtgCalculatorPrivate::readTempFile(const QString& path) const
{
    Mortgage result;
    QFile sourceFile(path);
    if (sourceFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&sourceFile);
        in.setVersion(StreamVersionUsed);
        in >> result;
        sourceFile.close();
    }
    else
        PrintToTempFile("PermissionError", "Could not read temporary file to load Mortgage");
    return result;
}

void MtgCalculator::SetLoan(const Mortgage& a, qint32 Index)
{
    Q_D(MtgCalculator);
	auto FoundLn = d->m_LoansPath.find(Index);
    if (FoundLn == d->m_LoansPath.end()) {
        RETURN_WHEN_RUNNING(true,)
        d->m_LoansPath.insert(Index,d->writeTempFile(a));
	}
    else {
        d->removeTempFile(FoundLn.value());
        FoundLn.value() = d->writeTempFile(a);
    }

}

Mortgage MtgCalculator::getLoan(qint32 Index) const
{
    Q_D(const MtgCalculator);
    return d->readTempFile(d->m_LoansPath.value(Index));
}

bool MtgCalculator::StartCalculation()
{
    Q_D(MtgCalculator);
	RETURN_WHEN_RUNNING(true, false)
	if (!ReadyToCalculate().isEmpty()) return false;
	{//Check if all base rates are valid
		bool CheckAgain = false;
		ConstantBaseRateTable TempTable;
        for (auto i = d->m_LoansPath.constBegin(); i != d->m_LoansPath.constEnd() && !CheckAgain; ++i) {
			if (d->readTempFile(i.value()).GetFloatingRateValue().IsEmpty()) {
				CompileReferenceRateValue(TempTable);
				CheckAgain = true;
			}
		}
        for (auto i = d->m_LoansPath.constBegin(); CheckAgain && i != d->m_LoansPath.constEnd(); ++i) {
            if (d->readTempFile(i.value()).GetFloatingRateValue().IsEmpty()) return false;
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
						out.setVersion(StreamVersionUsed);
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


    d->BeesReturned = 0;
    d->BeesSent.clear();
    d->m_ContinueCalculation = true;
	int NumberOfThreads = availableThreads();
    if (d->m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	MtgCalculatorThread* CurrentThread;
    for (auto SingleLoan = d->m_LoansPath.constBegin(); SingleLoan != d->m_LoansPath.constEnd(); ++SingleLoan) {
		if (NumofSent >= NumberOfThreads) break;
        if (d->BeesSent.contains(SingleLoan.key())) continue;
		CurrentThread = AddThread(SingleLoan.key());
		CurrentThread->SetLoan(d->readTempFile(SingleLoan.value()));
        if (d->TempProperties.contains(SingleLoan.key())) {
            const auto CurrProps = d->TempProperties.value(SingleLoan.key());
			for (auto j = CurrProps->constBegin(); j != CurrProps->constEnd(); ++j) {
				CurrentThread->SetLoanProperty(j.key(), j.value());
			}
		}
        CurrentThread->SetCPR(d->m_CPRass);
        CurrentThread->SetCDR(d->m_CDRass);
        CurrentThread->SetLS(d->m_LSass);
        CurrentThread->SetRecoveryLag(d->m_RecoveryLag);
        CurrentThread->SetDelinquency(d->m_Delinquency);
        CurrentThread->SetDelinquencyLag(d->m_DelinquencyLag);
        CurrentThread->SetOverrideAssumptions(d->m_OverrideAssumptions);
        CurrentThread->SetStartDate(d->StartDate);
        CurrentThread->SetDownloadScenario(d->m_DownloadScenario);
		CurrentThread->start();
		++NumofSent;
	}
	return true;
}
void MtgCalculator::BeeReturned(int Ident, const MtgCashFlow& a) {
    Q_D(MtgCalculator);
	RETURN_WHEN_RUNNING(false, )
    d->m_AggregatedRes += a;

    Q_ASSERT(d->m_LoansPath.contains(Ident));
    {
        auto tempLoan =d->readTempFile(d->m_LoansPath.value(Ident));
        tempLoan.SetCashFlows(a);
        SetLoan(tempLoan, Ident);
    }
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::BeeReturned(Ident, a);
    auto& tempRes = getResultVoid();
    auto i = tempRes.find(Ident);
    if (i != tempRes.end())
        i.value().reset();
    if (!d->m_ContinueCalculation)return;
	MtgCalculatorThread* CurrentThread;
    for (auto SingleLoan = d->m_LoansPath.constBegin(); SingleLoan != d->m_LoansPath.constEnd(); ++SingleLoan) {
        if (d->BeesSent.contains(SingleLoan.key())) continue;
		CurrentThread = AddThread(SingleLoan.key());
		CurrentThread->SetLoan(d->readTempFile(SingleLoan.value()));
        if (d->TempProperties.contains(SingleLoan.key())) {
            const auto CurrProps = d->TempProperties.value(SingleLoan.key());
			for (auto j = CurrProps->constBegin(); j != CurrProps->constEnd(); ++j) {
				CurrentThread->SetLoanProperty(j.key(), j.value());
			}
		}
        CurrentThread->SetCPR(d->m_CPRass);
        CurrentThread->SetCDR(d->m_CDRass);
        CurrentThread->SetLS(d->m_LSass);
        CurrentThread->SetRecoveryLag(d->m_RecoveryLag);
        CurrentThread->SetDelinquency(d->m_Delinquency);
        CurrentThread->SetDelinquencyLag(d->m_DelinquencyLag);
        CurrentThread->SetOverrideAssumptions(d->m_OverrideAssumptions);
        CurrentThread->SetStartDate(d->StartDate);
        CurrentThread->SetDownloadScenario(d->m_DownloadScenario);
		CurrentThread->start();
		return;
	}
}

void MtgCalculator::ClearLoans() {
    Q_D(MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
    d->m_LoansPath.clear();
    d->clearTempDir();
}
void MtgCalculator::Reset(){
	ClearLoans();
	ClearTempProperties();
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::Reset();
}
QString MtgCalculator::ReadyToCalculate()const{
    Q_D(const MtgCalculator);
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n" )
	QString Result;
	QString TempStr;
    if (d->StartDate.isNull()) Result += "Invalid Start Date\n";
    for (auto i = d->m_LoansPath.constBegin(); i != d->m_LoansPath.constEnd(); i++) {
		TempStr = d->readTempFile(i.value()).ReadyToCalculate();
		if(!TempStr.isEmpty()) Result+=TempStr+'\n';
	}
    if (BloombergVector(d->m_CPRass).IsEmpty(0.0, 1.0)) Result += "CPR Vector: " + d->m_CPRass + '\n';
    if (BloombergVector(d->m_CDRass).IsEmpty(0.0, 1.0)) Result += "CDR Vector: " + d->m_CDRass + '\n';
    if (BloombergVector(d->m_LSass).IsEmpty()) Result += "LS Vector: " + d->m_LSass + '\n';
    if (IntegerVector(d->m_RecoveryLag).IsEmpty()) Result += "Recovery Lag Vector: " + d->m_RecoveryLag + '\n';
    if (BloombergVector(d->m_Delinquency).IsEmpty(0, 1.0)) Result += "Delinquency Vector Vector: " + d->m_Delinquency + '\n';
    if (IntegerVector(d->m_DelinquencyLag).IsEmpty()) Result += "Delinquency Lag Vector: " + d->m_DelinquencyLag + '\n';
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
	return Result;
}

QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows) {
	if (flows.d_func()->m_ContinueCalculation) return stream;
	stream
        << static_cast<qint32>(flows.d_func()->m_LoansPath.size())
        << flows.d_func()->m_UseStoredCashFlows
        << flows.d_func()->m_CPRass
        << flows.d_func()->m_CDRass
        << flows.d_func()->m_LSass
        << flows.d_func()->m_RecoveryLag
        << flows.d_func()->m_Delinquency
        << flows.d_func()->m_DelinquencyLag
        << flows.d_func()->StartDate
		;
    for (auto i = flows.d_func()->m_LoansPath.constBegin(); i != flows.d_func()->m_LoansPath.constEnd(); i++) {
        stream << i.key() << flows.d_func()->readTempFile(i.value());
	}
	return flows.SaveToStream(stream);
}
QDataStream& MtgCalculator::LoadOldVersion(QDataStream& stream) {
    Q_D(MtgCalculator);
	RETURN_WHEN_RUNNING(true, stream)
	Reset();
	qint32 tempInt, TempKey;
	stream >> tempInt;
	stream >> d->m_UseStoredCashFlows;
    stream >> d->m_CPRass;
    stream >> d->m_CDRass;
    stream >> d->m_LSass;
    stream >> d->m_RecoveryLag;
    stream >> d->m_Delinquency;
    stream >> d->m_DelinquencyLag;
    stream >> d->StartDate;
	Mortgage TmpMtg;
    while (tempInt--) {
		stream >> TempKey;
		TmpMtg.SetLoadProtocolVersion(loadProtocolVersion());
		stream >> TmpMtg;
		SetLoan(TmpMtg, TempKey);
	}
	return TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::LoadOldVersion(stream);
}

const QDate& MtgCalculator::GetStartDate() const
{
    Q_D(const MtgCalculator);
    return d->StartDate;
}

bool MtgCalculator::GetUseStoredCashFlows() const
{
    Q_D(const MtgCalculator);
    return d->m_UseStoredCashFlows;
}

void MtgCalculator::SetUseStoredCashFlows(bool val)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, ) 
        d->m_UseStoredCashFlows = val;
}

const QString& MtgCalculator::GetCPRass() const
{
    Q_D(const MtgCalculator);
    return d->m_CPRass;
}

void MtgCalculator::SetCPRass(const QString& val)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, ) 
        d->m_CPRass = val;
}

const QString& MtgCalculator::GetCDRass() const
{
    Q_D(const MtgCalculator);
    return d->m_CDRass;
}

void MtgCalculator::SetCDRass(const QString& val)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, ) 
        d->m_CDRass = val;
}

const QString& MtgCalculator::GetLSass() const
{
    Q_D(const MtgCalculator);
    return d->m_LSass;
}

void MtgCalculator::SetLSass(const QString& val)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, ) 
        d->m_LSass = val;
}

const QString& MtgCalculator::GetRecoveryLag() const
{
    Q_D(const MtgCalculator);
    return d->m_RecoveryLag;
}

void MtgCalculator::SetRecoveryLag(const QString& val)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, ) d->m_RecoveryLag = val;
}

const QString& MtgCalculator::GetDelinquency() const
{
    Q_D(const MtgCalculator);
    return d->m_Delinquency;
}

void MtgCalculator::SetDelinquency(const QString& val)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, )d->m_Delinquency = val;
}

const QString& MtgCalculator::GetDelinquencyLag() const
{
    Q_D(const MtgCalculator);
    return d->m_DelinquencyLag;
}

void MtgCalculator::SetDelinquencyLag(const QString& val)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, ) d->m_DelinquencyLag = val;
}

void MtgCalculator::CompileReferenceRateValue(ForwardBaseRateTable& Values)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, )
        d->CompileReferenceRateValueTemplate(Values);
}
void MtgCalculator::CompileReferenceRateValue(ConstantBaseRateTable& Values)
{
    Q_D(MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
        d->CompileReferenceRateValueTemplate(Values);
}
#ifndef NO_DATABASE
void MtgCalculator::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll)
{
    Q_D(MtgCalculator);
    RETURN_WHEN_RUNNING(true, )
        d->GetBaseRatesDatabaseTemplate(Values, DownloadAll);
}
void MtgCalculator::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll)
{
    Q_D(MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
        d->GetBaseRatesDatabaseTemplate(Values, DownloadAll);
}
#endif
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows) {
	return flows.LoadOldVersion(stream);
}

/*
QHash<qint32, std::shared_ptr<Mortgage> >& MtgCalculator::GetLoans()
{
    Q_D( MtgCalculator);
    return d->Loans;
}

const QHash<qint32, std::shared_ptr<Mortgage> >& MtgCalculator::GetLoans() const
{
    Q_D(const MtgCalculator);
    return d->Loans;
}

void MtgCalculator::SetLoans(const QHash<qint32, Mortgage*>& a)
{
    Q_D( MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
	ClearLoans();
	for (auto i = a.constBegin(); i != a.constEnd(); ++i) {
        d->Loans.insert(i.key(), std::make_shared<Mortgage>(*(i.value())));
	}
}*/

void MtgCalculator::SetStartDate(const QDate& a)
{
    Q_D( MtgCalculator);
    d->StartDate = a;
}

void MtgCalculator::ClearResults()
{
    Q_D( MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
        d->m_AggregatedRes.Clear();
    for (auto i = d->m_LoansPath.constBegin(); i != d->m_LoansPath.constEnd(); ++i) {
        auto tempMtg = d->readTempFile(i.value());
        tempMtg.ResetFlows();
        SetLoan(tempMtg, i.key());
    }
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::ClearResults();
}

QList<qint32> MtgCalculator::GetResultKeys() const
{
    Q_D(const MtgCalculator);
    return d->m_LoansPath.keys();
}

const std::shared_ptr<MtgCashFlow> MtgCalculator::GetResult(qint32 key) const
{
    Q_D(const MtgCalculator);
    if (d->m_LoansPath.contains(key))
        return std::make_shared<MtgCashFlow>(d->readTempFile(d->m_LoansPath.value(key)).GetCashFlow());
    return std::shared_ptr<MtgCashFlow>(nullptr);
}

int MtgCalculator::NumBees() const
{
    Q_D(const MtgCalculator);
    return d->m_LoansPath.size();
}

bool MtgCalculator::hasLoan(qint32 Index)
{
    Q_D(const MtgCalculator);
    return d->m_LoansPath.contains(Index);
}

#ifndef NO_DATABASE
void MtgCalculator::DownloadScenarios() {
    Q_D( MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
	ClearTempProperties();
	QHash<QString, LoanAssumption> AssumptionCache;
    for (auto i = d->m_LoansPath.constBegin(); i != d->m_LoansPath.constEnd(); ++i) {
        const auto tempLoan = d->readTempFile(i.value());
        if (tempLoan.HasProperty("Scenario") && tempLoan.HasProperty("Mezzanine")) {
            if (!AssumptionCache.contains(tempLoan.GetProperty("Scenario"))) {
				LoanAssumption DownloadedAssumption(tempLoan.GetProperty("Scenario"));
				Db_Mutex.lock();
				QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
				if (!db.isValid()) {
					if (!db.isValid()) {
						db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
						db.setDatabaseName(
							"Driver={" + GetFromConfig("Database", "Driver")
							+ "}; "
							+ GetFromConfig("Database", "DataSource")
							);
						
					}
				}
				bool DbOpen = db.isOpen();
				if (!DbOpen) DbOpen = db.open();
				if (DbOpen) {
					QSqlQuery LoanAssQuerry(db);
					LoanAssQuerry.setForwardOnly(true);
					LoanAssQuerry.prepare("{CALL " + GetFromConfig("Database", "GetLoanAssumptionStoredProc") + "}");
					LoanAssQuerry.bindValue(":scenarioName", tempLoan.GetProperty("Scenario"));
					LoanAssQuerry.bindValue(":isSenior", true);
					if (LoanAssQuerry.exec()) {
						if (LoanAssQuerry.next()) {
							auto DbgRecord = LoanAssQuerry.record();
							int FieldCount = 0;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorLastUpdate(DbgRecord.value(FieldCount).toDate()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorMaturityExtension(QString::number(DbgRecord.value(FieldCount).toInt())); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorInitialHaircut(QString::number(DbgRecord.value(FieldCount).toDouble(),'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorPrepaymentFee(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorDayCount(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorCPR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorCDR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorLS(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorRecoveryLag ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorDelinquency (DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorDelinquencyLag (DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorPrice(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorHaircut ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorPrepayMultiplier ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetSeniorLossMultiplier ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
						}
					}
					LoanAssQuerry.bindValue(":isSenior", false);
					if (LoanAssQuerry.exec()) {
						if (LoanAssQuerry.next()) {
							auto DbgRecord = LoanAssQuerry.record();
							int FieldCount = 0;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzLastUpdate(DbgRecord.value(FieldCount).toDate()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzMaturityExtension(QString::number(DbgRecord.value(FieldCount).toInt())); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzInitialHaircut(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzPrepaymentFee(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzDayCount(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzCPR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzCDR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzLS(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzRecoveryLag(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzDelinquency(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzDelinquencyLag(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzPrice(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzHaircut(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzPrepayMultiplier(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption.SetMezzLossMultiplier(DbgRecord.value(FieldCount).toString()); ++FieldCount;
						}
					}
				}
				Db_Mutex.unlock();
				AssumptionCache.insert(DownloadedAssumption.GetScenarioName(), DownloadedAssumption);
			}
			const bool OverrideCurrent=
				#ifdef Assumptions_ExcelOverDB
					false
				#else
					true
				#endif
			;
            Q_ASSERT(AssumptionCache.contains(tempLoan.GetProperty("Scenario")));
			if (tempLoan.GetProperty("Mezzanine").compare("Yes", Qt::CaseInsensitive) == 0) {
				const auto CurrentAss = AssumptionCache.value(tempLoan.GetProperty("Scenario"));
				if ((OverrideCurrent || !tempLoan.HasProperty("MaturityExtension"))
					&&!CurrentAss.GetRawMezzMaturityExtension().isEmpty()) 
                    AddTempProperty(i.key(),"MaturityExtension", CurrentAss.GetMezzMaturityExtension());
				if ((OverrideCurrent || !tempLoan.HasProperty("StartingHaircut"))
					&& !CurrentAss.GetRawMezzInitialHaircut().isEmpty())  
                    AddTempProperty(i.key(),"StartingHaircut", CurrentAss.GetMezzInitialHaircut());
				if ((OverrideCurrent || !tempLoan.HasProperty("PrepaymentFee"))
					&& !CurrentAss.GetRawMezzPrepaymentFee().isEmpty())
                    AddTempProperty(i.key(),"PrepaymentFee", CurrentAss.GetMezzPrepaymentFee());
				if ((OverrideCurrent || !tempLoan.HasProperty("DayCount"))
					&&!CurrentAss.GetRawMezzDayCount().isEmpty()) 
                    AddTempProperty(i.key(),"DayCount", CurrentAss.GetMezzDayCount());
				if ((OverrideCurrent || !tempLoan.HasProperty("Haircut"))
					&& !CurrentAss.GetRawMezzHaircut().isEmpty()) 
                    AddTempProperty(i.key(),"Haircut", CurrentAss.GetMezzHaircut());
				if ((OverrideCurrent || !tempLoan.HasProperty("PrepayMultiplier"))
					&& !CurrentAss.GetRawMezzPrepayMultiplier().isEmpty())
                    AddTempProperty(i.key(),"PrepayMultiplier", CurrentAss.GetMezzPrepayMultiplier());
				if ((OverrideCurrent || !tempLoan.HasProperty("LossMultiplier"))
					&& !CurrentAss.GetRawMezzLossMultiplier().isEmpty()) 
                    AddTempProperty(i.key(),"LossMultiplier", CurrentAss.GetMezzLossMultiplier());
				if ((OverrideCurrent || !tempLoan.HasProperty("CPR"))
					&& !CurrentAss.GetRawMezzCPR().isEmpty())
                    AddTempProperty(i.key(),"CPR", CurrentAss.GetMezzCPR());
				if ((OverrideCurrent || !tempLoan.HasProperty("CDR"))
					&& !CurrentAss.GetRawMezzCDR().isEmpty()) 
                    AddTempProperty(i.key(),"CDR", CurrentAss.GetMezzCDR());
				if ((OverrideCurrent || !tempLoan.HasProperty("LS"))
					&& !CurrentAss.GetRawMezzLS().isEmpty()) 
                    AddTempProperty(i.key(),"LS", CurrentAss.GetMezzLS());
				if ((OverrideCurrent || !tempLoan.HasProperty("RecoveryLag"))
					&& !CurrentAss.GetRawMezzRecoveryLag().isEmpty())
                    AddTempProperty(i.key(),"RecoveryLag", CurrentAss.GetMezzRecoveryLag());
				if ((OverrideCurrent || !tempLoan.HasProperty("Delinquency"))
					&& !CurrentAss.GetRawMezzDelinquency().isEmpty())  
                    AddTempProperty(i.key(),"Delinquency", CurrentAss.GetMezzDelinquency());
				if ((OverrideCurrent || !tempLoan.HasProperty("DelinquencyLag"))
					&& !CurrentAss.GetRawMezzDelinquencyLag().isEmpty())  
                    AddTempProperty(i.key(),"DelinquencyLag", CurrentAss.GetMezzDelinquencyLag());
				if ((OverrideCurrent || !tempLoan.HasProperty("Price"))
					&& !CurrentAss.GetRawMezzPrice().isEmpty())  
                    AddTempProperty(i.key(),"Price", CurrentAss.GetMezzPrice());
			}
			else {
				auto CurrentAss = AssumptionCache.value(tempLoan.GetProperty("Scenario"));
				if ((OverrideCurrent || !tempLoan.HasProperty("MaturityExtension"))
					&& !CurrentAss.GetRawSeniorMaturityExtension().isEmpty()) 
                    AddTempProperty(i.key(),"MaturityExtension", CurrentAss.GetSeniorMaturityExtension());
				if ((OverrideCurrent || !tempLoan.HasProperty("StartingHaircut"))
					&& !CurrentAss.GetRawSeniorInitialHaircut().isEmpty())  
                    AddTempProperty(i.key(),"StartingHaircut", CurrentAss.GetSeniorInitialHaircut());
				if ((OverrideCurrent || !tempLoan.HasProperty("PrepaymentFee"))
					&& !CurrentAss.GetRawSeniorPrepaymentFee().isEmpty())
                    AddTempProperty(i.key(),"PrepaymentFee", CurrentAss.GetSeniorPrepaymentFee());
				if ((OverrideCurrent || !tempLoan.HasProperty("DayCount"))
					&& !CurrentAss.GetRawSeniorDayCount().isEmpty()) 
                    AddTempProperty(i.key(),"DayCount", CurrentAss.GetSeniorDayCount());
				if ((OverrideCurrent || !tempLoan.HasProperty("Haircut"))
					&& !CurrentAss.GetRawSeniorHaircut().isEmpty()) 
                    AddTempProperty(i.key(),"Haircut", CurrentAss.GetSeniorHaircut());
				if ((OverrideCurrent || !tempLoan.HasProperty("PrepayMultiplier"))
					&& !CurrentAss.GetRawSeniorPrepayMultiplier().isEmpty())
                    AddTempProperty(i.key(),"PrepayMultiplier", CurrentAss.GetSeniorPrepayMultiplier());
				if ((OverrideCurrent || !tempLoan.HasProperty("LossMultiplier"))
					&& !CurrentAss.GetRawSeniorLossMultiplier().isEmpty()) 
                    AddTempProperty(i.key(),"LossMultiplier", CurrentAss.GetSeniorLossMultiplier());
				if ((OverrideCurrent || !tempLoan.HasProperty("CPR"))
					&& !CurrentAss.GetRawSeniorCPR().isEmpty()) 
                    AddTempProperty(i.key(),"CPR", CurrentAss.GetSeniorCPR());
				if ((OverrideCurrent || !tempLoan.HasProperty("CDR"))
					&& !CurrentAss.GetRawSeniorCDR().isEmpty())
                    AddTempProperty(i.key(),"CDR", CurrentAss.GetSeniorCDR());
				if ((OverrideCurrent || !tempLoan.HasProperty("LS"))
					&& !CurrentAss.GetRawSeniorLS().isEmpty()) 
                    AddTempProperty(i.key(),"LS", CurrentAss.GetSeniorLS());
				if ((OverrideCurrent || !tempLoan.HasProperty("RecoveryLag"))
					&& !CurrentAss.GetRawSeniorRecoveryLag().isEmpty()) 
                    AddTempProperty(i.key(),"RecoveryLag", CurrentAss.GetSeniorRecoveryLag());
				if ((OverrideCurrent || !tempLoan.HasProperty("Delinquency"))
					&& !CurrentAss.GetRawSeniorDelinquency().isEmpty())  
                    AddTempProperty(i.key(),"Delinquency", CurrentAss.GetSeniorDelinquency());
				if ((OverrideCurrent || !tempLoan.HasProperty("DelinquencyLag"))
					&& !CurrentAss.GetRawSeniorDelinquencyLag().isEmpty())  
                    AddTempProperty(i.key(),"DelinquencyLag", CurrentAss.GetSeniorDelinquencyLag());
				if ((OverrideCurrent || !tempLoan.HasProperty("Price"))
					&& !CurrentAss.GetRawSeniorPrice().isEmpty())  
                    AddTempProperty(i.key(),"Price", CurrentAss.GetSeniorPrice());
			}
		}
	}
}

void MtgCalculator::GuessLoanScenarios(bool OverrideAss) {
    Q_D( MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
	QHash<QString, std::shared_ptr< LoanAssumption> > AvailableAssumptions;
	Db_Mutex.lock();
	{
		QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
		if (!db.isValid()) {
			if (!db.isValid()) {
				db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
				db.setDatabaseName(
					"Driver={" + GetFromConfig("Database", "Driver")
					+ "}; "
					+ GetFromConfig("Database", "DataSource")
					);
				
			}
		}
		bool DbOpen = db.isOpen();
		if (!DbOpen) DbOpen = db.open();
		if (DbOpen) {
			QSqlQuery LoanAssQuerry(db);
			LoanAssQuerry.setForwardOnly(true);
			LoanAssQuerry.prepare("{CALL " + GetFromConfig("Database", "GetAllLoanAssumptionsStoredProc") + "}");
			if (LoanAssQuerry.exec()) {
				while (LoanAssQuerry.next()) {
					auto DbgRecord = LoanAssQuerry.record();
					if (AvailableAssumptions.contains(DbgRecord.value(0).toString())) continue;
					auto CurrAss = AvailableAssumptions.insert(DbgRecord.value(0).toString(), std::make_shared<LoanAssumption>(DbgRecord.value(0).toString()));
					(*CurrAss)->SetAliases(DbgRecord.value(1).toString());
				}
			}
		}
	}
	Db_Mutex.unlock();
    for (auto SingleLoan = d->m_LoansPath.constBegin(); SingleLoan != d->m_LoansPath.constEnd(); ++SingleLoan)
    {
		for (auto i = AvailableAssumptions.constBegin(); i != AvailableAssumptions.constEnd(); ++i) {
			for (const QString& CurrProperty : LoansPropertiesToSearch) {
                auto tempLoan = d->readTempFile(SingleLoan.value());
                if (i.value()->MatchPattern(tempLoan.GetProperty(CurrProperty))) {
                    if (!tempLoan.HasProperty("Scenario") || OverrideAss) {
                        tempLoan.SetProperty("Scenario", i.key());
                        SetLoan(tempLoan, SingleLoan.key());
                    }
					break;
				}
			}
		}
	}
}



void MtgCalculator::SetOverrideAssumptions(bool a)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, )
        d->m_OverrideAssumptions = a;
}

bool MtgCalculator::GetOverrideAssumptions() const
{
    Q_D(const MtgCalculator);
    return 
        d->m_OverrideAssumptions;
}

MtgCashFlow MtgCalculator::GetAggregatedResults() const
{
    Q_D(const MtgCalculator);
    RETURN_WHEN_RUNNING(true, MtgCashFlow())
        return d->m_AggregatedRes;
}

bool MtgCalculator::GetDownloadScenario() const
{
    Q_D(const MtgCalculator);
    return 
        d->m_DownloadScenario;
}

void MtgCalculator::SetDownloadScenario(bool val)
{
    Q_D( MtgCalculator);
    RETURN_WHEN_RUNNING(true, ) 
        d->m_DownloadScenario = val;
}

void MtgCalculator::RemoveResult(qint32 Key)
{
    Q_D(MtgCalculator);
    auto loanIter = d->m_LoansPath.find(Key);
    if (loanIter != d->m_LoansPath.end()) {
        d->removeTempFile(loanIter.value());
        d->m_LoansPath.erase(loanIter);
    }
}

#endif
void MtgCalculator::ClearTempProperties()
{
    Q_D( MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
    d->TempProperties.clear();
}

void MtgCalculator::AddTempProperty(qint32 LoanID, const QString& PropertyName, const QString& PropertyValue)
{
    Q_D( MtgCalculator);
	RETURN_WHEN_RUNNING(true, )
        if (!d->m_LoansPath.contains(LoanID) || PropertyName.isEmpty() || PropertyValue.isEmpty())return;
    auto iter = d->TempProperties.find(LoanID);
    if (iter == d->TempProperties.end()) iter = d->TempProperties.insert(LoanID, std::make_shared<QHash<QString, QString>>());
	iter.value()->operator[](PropertyName) = PropertyValue;
}

bool MtgCalculator::uploadGeographyToDatabase(const QString& DealName, QDate startDate, QDate endDate) const
{
#ifndef NO_DATABASE
    if (DealName.isEmpty() || startDate.isNull() || endDate.isNull())
        return false;
    if (startDate > endDate)
        std::swap(startDate, endDate);
    const auto geogBreak = GetGeographicBreakdown();
    if (geogBreak.isEmpty()) {
        DEBG_LOG("uploadGeographyToDatabase() No geographic information found");
        return false;
    }
    {
        QMutexLocker dbLocker(&Db_Mutex);
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
            {
                //Check deal exist in the database
                QSqlQuery CheckBondExistQuery(db);
                CheckBondExistQuery.setForwardOnly(true);
                CheckBondExistQuery.prepare("{CALL " + GetFromConfig("Database", "GetDealInfoStoredProc") + "}");
                CheckBondExistQuery.bindValue(":dealName", DealName);
                if (!CheckBondExistQuery.exec()) {
                    DEBG_LOG("uploadGeographyToDatabase() Failed to run GetBondDetailsStoredProc");
                    return false;
                }
                if (!CheckBondExistQuery.next()) {
                    DEBG_LOG("uploadGeographyToDatabase() Deal not found in Database");
                    return false;
                }
            }
            db.transaction();
            for (auto i = geogBreak.constBegin(); i != geogBreak.constEnd(); ++i) {
                QSqlQuery UploadGeogrQuery(db);
                UploadGeogrQuery.setForwardOnly(true);
                UploadGeogrQuery.prepare("{CALL " + GetFromConfig("Database", "SetGeographyStoredProc") + "}");
                UploadGeogrQuery.bindValue(":dealName", DealName);
                UploadGeogrQuery.bindValue(":filedName", "Geography" + i.key());
                UploadGeogrQuery.bindValue(":fieldDisplayName", i.key());
                UploadGeogrQuery.bindValue(":startDate", startDate.toString(Qt::ISODate));
                UploadGeogrQuery.bindValue(":endDate", endDate.toString(Qt::ISODate));
                UploadGeogrQuery.bindValue(":Value", i.value());
                if (!UploadGeogrQuery.exec()){
                    DEBG_LOG(QString("uploadGeographyToDatabase() Unable to upload data to Database. Country: %1, Value: %2").arg(i.key()).arg(i.value()));
                    db.rollback();
                    return false;
                }
            }
            db.commit();
            return true;
        }
        DEBG_LOG("uploadGeographyToDatabase() Unable to open DB");
    }
#endif
    return false;
}

QHash<QString, double> MtgCalculator::GetGeographicBreakdown() const
{
    Q_D(const MtgCalculator);
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
    if (d->m_LoansPath.isEmpty()) return Result;
	QString CurrentGuess;
	simstring::reader dbr;
    const std::string DbRootFolder(GetFromConfig("Folder","CountriesDBFolder").toStdString());
	if (DbRootFolder.empty()) 
        return Result;
	if (!QFile::exists(QString::fromStdString(DbRootFolder + "CountriesDB\\ISO3166-1.ssdb"))) 
        BuildDBCountries(QString::fromStdString(DbRootFolder + "CountriesDB\\ISO3166-1.ssdb"));
	if (!dbr.open(DbRootFolder + "CountriesDB\\ISO3166-1.ssdb")) 
        return Result;
    for (auto i = d->m_LoansPath.constBegin(); i != d->m_LoansPath.constEnd(); ++i) {
        const auto tempLoan = d->readTempFile(i.value());
        if (tempLoan.GetSize()<0.01) continue;
        SumOut += tempLoan.GetSize();
        CurrentGuess = tempLoan.GetProperty("Country");
        if (!InsertUnknown(CurrentGuess, tempLoan.GetSize())) {
			CurrentGuess = GetGeography(CurrentGuess, dbr);
            CurrentGuess = GetCountryISOCode(CurrentGuess);
            if (!InsertUnknown(CurrentGuess, tempLoan.GetSize())) {
                auto Curres = Result.find(CurrentGuess);
                if (Curres == Result.end()) 
                    Result.insert(CurrentGuess, tempLoan.GetSize());
                else 
                    Curres.value() += tempLoan.GetSize();
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

QString MtgCalculator::GetGeography(const QString& guess, simstring::reader& dbr) const
{
	double simThreshold=0.6;
	std::vector<std::wstring> simMatches;
    QString Result;
	do {
		simMatches.clear();
		dbr.retrieve(guess.toLower().toStdWString(), simstring::cosine, simThreshold, std::back_inserter(simMatches));
        if (!simMatches.empty()) Result = QString::fromStdWString(simMatches.front());
		simThreshold += 0.05;
	} while (simMatches.size() > 1 && simThreshold<1.0);
    return Result;
}

QString MtgCalculator::GetCountryISOCode(QString name) const
{
	name = name.trimmed().toLower();
    if (name.isEmpty()) return QString();
	QFile InputFile(":/DataSources/ISO3166-1.xml");
	InputFile.open(QIODevice::ReadOnly);
	bool NameMatched = false;
	QString CurrentCode;
	bool CountryFound = false;
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
				if (!CountryFound) return QString();
                PossibleNames = xml.readElementText().split("#,#", QString::SkipEmptyParts);
                for (auto pni = PossibleNames.constBegin(); !NameMatched && pni != PossibleNames.constEnd(); ++pni) {
                    if (name.compare(*pni, Qt::CaseInsensitive) == 0)
                        NameMatched = true;
                }
			}
			else if (xml.name() == "Alpha-2") {
				if (!CountryFound) return QString();
                CurrentCode = xml.readElementText();
			}
		}
		else if (xml.isEndElement()) {
			if (xml.name() == "Country") {
				if (!CountryFound) return QString();
                if (NameMatched && !CurrentCode.isEmpty()) return CurrentCode.trimmed().toUpper();
				CountryFound = false;
				NameMatched = false;
				CurrentCode.clear();
			}
		}
	}
	return QString();
}
