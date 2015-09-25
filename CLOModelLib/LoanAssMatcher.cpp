#include "LoanAssMatcher.h"
#include "Private/LoanAssMatcher_p.h"
#include "Private/InternalItems.h"
#include <QDir>
#include <QDate>
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#endif
DEFINE_PUBLIC_QOBJECT_COMMONS(LoanAssMatcher)
LoanAssMatcherPrivate::~LoanAssMatcherPrivate(){}
LoanAssMatcher::~LoanAssMatcher(){}
LoanAssMatcherPrivate::LoanAssMatcherPrivate(LoanAssMatcher *q)
:AbstrAsyncCalculatorPrivate(q)
{}
LoanAssMatcher::LoanAssMatcher(LoanAssMatcherPrivate *d, QObject* parent)
    : TemplAsyncCalculator <LoanAssMatcherThread, LoanAssMatcherResult>(d, parent)
{
    connect(this, &LoanAssMatcher::Calculated, this, &LoanAssMatcher::ClearFilesInFolder);
    connect(this, &LoanAssMatcher::Stopped, this, &LoanAssMatcher::ClearFilesInFolder);
}

void LoanAssMatcher::GetAssumptionsDatabase() {
    RETURN_WHEN_RUNNING(true, )
    Q_D(LoanAssMatcher);
    QHash<QString, std::shared_ptr<LoanAssumption> > NewAssumptions;
#ifndef NO_DATABASE
	d->m_AvailableAssumptions.clear();
	Db_Mutex.lock();
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
		QSqlQuery LoanAssQuery(db);
		LoanAssQuery.setForwardOnly(true);
		LoanAssQuery.prepare("{CALL " + GetFromConfig("Database", "GetAllLoanAssumptionStoredProc") + "}");
		if (LoanAssQuery.exec()) {
			bool CurrentSenior;
			int FieldCount;
			while (LoanAssQuery.next()) {
				FieldCount = 0;
				auto LoanAssRecord = LoanAssQuery.record();
				QString CurrentScenario = LoanAssRecord.value(FieldCount).toString(); ++FieldCount;
				std::shared_ptr<LoanAssumption> CurrentAss(nullptr);
				for (auto i = NewAssumptions.begin(); i != NewAssumptions.end(); ++i) {
					if (i.key().compare(CurrentScenario, Qt::CaseInsensitive) == 0) {
						CurrentAss = i.value();
						break;
					}
				}
				if (!CurrentAss) {
					CurrentAss = std::make_shared< LoanAssumption>(CurrentScenario);
					if (!LoanAssRecord.isNull(FieldCount)) CurrentAss->SetAliases(LoanAssRecord.value(FieldCount).toString());
					NewAssumptions.insert(CurrentAss->GetScenarioName(), CurrentAss);
				}++FieldCount;
				CurrentSenior = LoanAssRecord.value(FieldCount).toBool(); ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorMaturityExtension(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzMaturityExtension(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorInitialHaircut(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzInitialHaircut(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrepaymentFee(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzPrepaymentFee(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDayCount(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzDayCount(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorCPR(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzCPR(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorCDR(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzCDR(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLS(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzLS(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorRecoveryLag(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzRecoveryLag(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDelinquency(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzDelinquency(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorDelinquencyLag(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzDelinquencyLag(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrice(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzPrice(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorHaircut(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzHaircut(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorPrepayMultiplier(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzPrepayMultiplier(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLossMultiplier(LoanAssRecord.value(FieldCount).toString());
					else CurrentAss->SetMezzLossMultiplier(LoanAssRecord.value(FieldCount).toString());
				} ++FieldCount;
				if (!LoanAssRecord.isNull(FieldCount)) {
					if (CurrentSenior) CurrentAss->SetSeniorLastUpdate(LoanAssRecord.value(FieldCount).toDate());
					else CurrentAss->SetMezzLastUpdate(LoanAssRecord.value(FieldCount).toDate());
				} ++FieldCount;

			}
		}
	}
	Db_Mutex.unlock();
#endif
	for (auto i = NewAssumptions.begin(); i != NewAssumptions.end(); ++i) {
        d->m_AvailableAssumptions.insert(i.key(), i.value());
	}
}

void LoanAssMatcher::AddAssumption(const QString& key, const LoanAssumption& a)
{
    Q_D(LoanAssMatcher);
	RETURN_WHEN_RUNNING(true, )
        d->m_AvailableAssumptions.remove(key);
    d->m_AvailableAssumptions.insert(key, std::make_shared< LoanAssumption>(a));
}

void LoanAssMatcher::RemoveAssumption(const QString& key)
{
    Q_D(LoanAssMatcher);
	RETURN_WHEN_RUNNING(true, )
        d->m_AvailableAssumptions.remove(key);
}

const QString& LoanAssMatcher::GetFolderToScan() const
{
    Q_D(const LoanAssMatcher);
    return d->m_FolderToScan;
}

void LoanAssMatcher::Reset()
{
    Q_D(LoanAssMatcher);
	RETURN_WHEN_RUNNING(true, )
        d->m_AvailableAssumptions.clear();
    d->m_FolderToScan.clear();
    d->m_FilesInFolder.clear();
	TemplAsyncCalculator<LoanAssMatcherThread, LoanAssMatcherResult>::Reset();
}

QString LoanAssMatcher::ReadyToCalculate() const
{
    Q_D(const LoanAssMatcher);
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n")
	QString Result;
	if (NumBees()==0) Result += tr("No models found in the specified folder\n");
    if (d->m_AvailableAssumptions.isEmpty()) Result += tr("No assumptions available\n");
	return Result;
}

int LoanAssMatcher::NumBees() const
{
    Q_D(const LoanAssMatcher);
    if (!d->m_FilesInFolder.isEmpty()) return d->m_FilesInFolder.size();
    QDir Source(d->m_FolderToScan);
	if (!Source.exists()) return 0;
	Source.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	Source.setNameFilters(QStringList() << "*.clom");
	return Source.entryInfoList().size();
}

bool LoanAssMatcher::StartCalculation()
{
    Q_D( LoanAssMatcher);
	RETURN_WHEN_RUNNING(true, false)
	if (!ReadyToCalculate().isEmpty()) return false;
    QDir Source(d->m_FolderToScan);
	Source.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	Source.setNameFilters(QStringList() << "*.clom");
    d->m_FilesInFolder = Source.entryInfoList();
    d->BeesReturned = 0;
    d->BeesSent.clear();
    d->m_ContinueCalculation = true;
	int NumberOfThreads = availableThreads();
    if (d->m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	LoanAssMatcherThread* CurrentThread;
    for (int i = 0; i < d->m_FilesInFolder.size() && i<NumberOfThreads; ++i) {
        if (d->BeesSent.contains(i)) continue;
		CurrentThread = AddThread(i);
        CurrentThread->SetAvailableAssumptions(d->m_AvailableAssumptions);
        CurrentThread->SetModelToScan(d->m_FilesInFolder.at(i).absoluteFilePath());
		CurrentThread->start();
	}
	return true;
}

void LoanAssMatcher::BeeReturned(int Ident, const LoanAssMatcherResult& a)
{
    Q_D( LoanAssMatcher);
	RETURN_WHEN_RUNNING(false, )
	TemplAsyncCalculator<LoanAssMatcherThread, LoanAssMatcherResult>::BeeReturned(Ident, a);
    for (int i = 0; i < d->m_FilesInFolder.size(); ++i) {
        if (d->BeesSent.contains(i)) continue;
		LoanAssMatcherThread* CurrentThread = AddThread(i);
        CurrentThread->SetAvailableAssumptions(d->m_AvailableAssumptions);
        CurrentThread->SetModelToScan(d->m_FilesInFolder.at(i).absoluteFilePath());
		CurrentThread->start();
		return;
	}
}

void LoanAssMatcher::SetFolderToScan(const QString& val)
{
    Q_D( LoanAssMatcher);
	RETURN_WHEN_RUNNING(true, )
        d->m_FolderToScan = val;
    d->m_FilesInFolder.clear();
}

QDataStream& LoanAssMatcher::LoadOldVersion(QDataStream& stream)
{
    Q_D( LoanAssMatcher);
	RETURN_WHEN_RUNNING(true, stream)
	qint32 TempSize;
	QString TempString; 
	LoanAssumption TempAss("");
    stream >> d->m_FolderToScan >> TempSize;
	for (int i = 0; i < TempSize; ++i) {
		stream >> TempString >> TempAss;
        d->m_AvailableAssumptions.insert(TempString,std::make_shared< LoanAssumption>(TempAss));
	}
	ResetProtocolVersion();
	return stream;
}

void LoanAssMatcher::ClearFilesInFolder()
{
    Q_D( LoanAssMatcher);
    RETURN_WHEN_RUNNING(true, ) 
        d->m_FilesInFolder.clear();
}

QDataStream& operator<<(QDataStream & stream, const LoanAssMatcher& flows)
{
	if (flows.d_func()->m_ContinueCalculation) return stream;
    stream << flows.d_func()->m_FolderToScan << qint32(flows.d_func()->m_AvailableAssumptions.size());
    for (auto i = flows.d_func()->m_AvailableAssumptions.constBegin(); i != flows.d_func()->m_AvailableAssumptions.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	return stream;
}

QDataStream& operator>>(QDataStream & stream, LoanAssMatcher& flows) {
	return flows.LoadOldVersion(stream);
}
