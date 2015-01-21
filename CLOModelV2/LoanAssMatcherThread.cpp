#include "LoanAssMatcherThread.h"
#include <QFile>
#include <QFileInfo>
#include "BackwardCompatibilityInterface.h"
#include "Waterfall.h"
#include "MtgCalculator.h"
#include "CommonFunctions.h"
LoanAssMatcherThread::LoanAssMatcherThread(int Identity, QObject *parent /*= nullptr*/) 
	: TemplAsyncThread <LoanAssMatcherResult>(Identity, parent)
{}

void LoanAssMatcherThread::run() {
	Result.Clear();
	QFile ModelFile(m_ModelToScan);
	if (!ModelFile.open(QIODevice::ReadOnly)) return ReturnError();
	qint32 VersionChecker;
	QDataStream out(&ModelFile);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		ModelFile.close();
		return ReturnError();;
	}
	{QDate Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	QString NewModelName;
	{Waterfall Junk; Junk.SetLoadProtocolVersion(VersionChecker); out >> Junk; NewModelName = Junk.GetDealName(); }
	{Waterfall Junk; Junk.SetLoadProtocolVersion(VersionChecker); out >> Junk; }
	QHash<qint32, Mortgage*> LoanPool;
	MtgCalculator Junk;
	Junk.SetLoadProtocolVersion(VersionChecker);
	out >> Junk;
	ModelFile.close();
	LoanPool = Junk.GetLoans();

	for (auto i = LoanPool.constBegin(); i != LoanPool.constEnd(); ++i) {
		for (auto j = m_AvailableAssumptions->constBegin(); j != m_AvailableAssumptions->constEnd(); ++j) {
			//if (j.value()->GetScenarioName().compare(i.value()->GetProperty("Scenario"),Qt::CaseInsensitive)==0) continue;
			for (const QString& CurrProperty : LoansPropertiesToSearch) {
				if (j.value()->MatchPattern(i.value()->GetProperty(CurrProperty))) {
					Result.AddScenario(j.value()->GetScenarioName(), i.value()->GetProperty("Scenario"), i.value()->GetProperty("Facility"), i.value()->GetProperty("Issuer"), i.key());
					break;
				}
			}
		}
	}
	if (Result.ScenarioCount() > 0) {
		Result.SetFilePath(m_ModelToScan);
		Result.SetDealName(NewModelName.isEmpty() ? QFileInfo(ModelFile).completeBaseName() : NewModelName);
	}
	emit AnonimCalculated(Identifier);
}

void LoanAssMatcherThread::ReturnError() {
	emit ErrorCalculation(Identifier);
}
