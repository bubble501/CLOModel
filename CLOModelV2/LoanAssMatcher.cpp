#include "LoanAssMatcher.h"
#include <QDir>
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#endif
LoanAssMatcher::LoanAssMatcher(QObject *parent)
	: TemplAsyncCalculator<LoanAssMatcherThread, LoanAssMatcherResult>(parent)
{
	connect(this, &LoanAssMatcher::Calculated, this, &LoanAssMatcher::ClearFilesInFolder);
	connect(this, &LoanAssMatcher::Stopped, this, &LoanAssMatcher::ClearFilesInFolder);
}
void LoanAssMatcher::GetAssumptionsDatabase() {
	RETURN_WHEN_RUNNING(true, )
#ifndef NO_DATABASE
	QHash<QString, LoanAssumption*> NewAssumptions;
	m_AvailableAssumptions.clear();
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
				LoanAssumption* CurrentAss(nullptr);
				for (auto i = NewAssumptions.begin(); i != NewAssumptions.end(); ++i) {
					if (i.key().compare(CurrentScenario, Qt::CaseInsensitive) == 0) {
						CurrentAss = i.value();
						break;
					}
				}
				if (!CurrentAss) {
					CurrentAss = new LoanAssumption(CurrentScenario);
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
		m_AvailableAssumptions.insert(i.key(), QSharedPointer<LoanAssumption>(i.value()));
	}
}

void LoanAssMatcher::AddAssumption(const QString& key,const LoanAssumption& a) {
	RETURN_WHEN_RUNNING(true, )
	m_AvailableAssumptions.remove(key);
	m_AvailableAssumptions.insert(key, QSharedPointer<LoanAssumption>(new LoanAssumption(a)));
}

void LoanAssMatcher::RemoveAssumption(const QString& key) {
	RETURN_WHEN_RUNNING(true, )
	m_AvailableAssumptions.remove(key);
}

void LoanAssMatcher::Reset() {
	RETURN_WHEN_RUNNING(true, )
	m_AvailableAssumptions.clear();
	m_FolderToScan.clear();
	m_FilesInFolder.clear();
	TemplAsyncCalculator<LoanAssMatcherThread, LoanAssMatcherResult>::Reset();
}

QString LoanAssMatcher::ReadyToCalculate() const {
	RETURN_WHEN_RUNNING(true, "Calculator Already Running\n")
	QString Result;
	if (NumBees()==0) Result += tr("No models found in the specified folder\n");
	if (m_AvailableAssumptions.isEmpty()) Result += tr("No assumptions available\n");
	return Result;
}

int LoanAssMatcher::NumBees() const {
	if (!m_FilesInFolder.isEmpty()) return m_FilesInFolder.size();
	QDir Source(m_FolderToScan);
	if (!Source.exists()) return 0;
	Source.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	Source.setNameFilters(QStringList() << "*.clom");
	return Source.entryInfoList().size();
}

bool LoanAssMatcher::StartCalculation() {
	RETURN_WHEN_RUNNING(true, false)
	if (!ReadyToCalculate().isEmpty()) return false;
	QDir Source(m_FolderToScan);
	Source.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	Source.setNameFilters(QStringList() << "*.clom");
	m_FilesInFolder= Source.entryInfoList();
	BeesReturned = 0;
	BeesSent.clear();
	m_ContinueCalculation = true;
	int NumberOfThreads = QThread::idealThreadCount();
	if (m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	LoanAssMatcherThread* CurrentThread;
	for (int i = 0; i < m_FilesInFolder.size() && i<NumberOfThreads; ++i) {
		if (BeesSent.contains(i)) continue;
		CurrentThread = AddThread(i);
		CurrentThread->SetAvailableAssumptions(m_AvailableAssumptions);
		CurrentThread->SetModelToScan(m_FilesInFolder.at(i).absoluteFilePath());
		CurrentThread->start();
	}
	return true;
}

void LoanAssMatcher::BeeReturned(int Ident, const LoanAssMatcherResult& a) {
	RETURN_WHEN_RUNNING(false, )
	TemplAsyncCalculator<LoanAssMatcherThread, LoanAssMatcherResult>::BeeReturned(Ident, a);
	for (int i = 0; i < m_FilesInFolder.size(); ++i) {
		if (BeesSent.contains(i)) continue;
		LoanAssMatcherThread* CurrentThread = AddThread(i);
		CurrentThread->SetAvailableAssumptions(m_AvailableAssumptions);
		CurrentThread->SetModelToScan(m_FilesInFolder.at(i).absoluteFilePath());
		CurrentThread->start();
		return;
	}
}

void LoanAssMatcher::SetFolderToScan(const QString& val) {
	RETURN_WHEN_RUNNING(true, )
	m_FolderToScan = val;
	m_FilesInFolder.clear();
}

QDataStream& LoanAssMatcher::LoadOldVersion(QDataStream& stream) {
	RETURN_WHEN_RUNNING(true, stream)
	qint32 TempSize;
	QString TempString; 
	LoanAssumption TempAss("");
	stream >> m_FolderToScan >> TempSize;
	for (int i = 0; i < TempSize; ++i) {
		stream >> TempString >> TempAss;
		m_AvailableAssumptions.insert(TempString, QSharedPointer<LoanAssumption>(new LoanAssumption(TempAss)));
	}
	ResetProtocolVersion();
	return stream;
}

QDataStream& operator<<(QDataStream & stream, const LoanAssMatcher& flows) {
	if (flows.m_ContinueCalculation) return stream;
	stream << flows.m_FolderToScan << qint32(flows.m_AvailableAssumptions.size());
	for (auto i = flows.m_AvailableAssumptions.constBegin(); i != flows.m_AvailableAssumptions.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	return stream;
}

QDataStream& operator>>(QDataStream & stream, LoanAssMatcher& flows) {
	return flows.LoadOldVersion(stream);
}
