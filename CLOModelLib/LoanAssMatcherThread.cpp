#include "LoanAssMatcherThread.h"
#include "Private/LoanAssMatcherThread_p.h"
#include <QFile>
#include <QFileInfo>
#include "BackwardCompatibilityInterface.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include "Waterfall.h"
#include "MtgCalculator.h"
#include "CommonFunctions.h"
#include "Private/InternalItems.h"
#include "Mortgage.h"
LoanAssMatcherThread::~LoanAssMatcherThread() {}
LoanAssMatcherThreadPrivate::~LoanAssMatcherThreadPrivate() {}
LoanAssMatcherThreadPrivate::LoanAssMatcherThreadPrivate(LoanAssMatcherThread *q)
    :AbstrAsyncThreadPrivate(q)
    , m_AvailableAssumptions(nullptr)
{}
LoanAssMatcherThread::LoanAssMatcherThread(int Identity, QObject *parent /*= nullptr*/) 
    : LoanAssMatcherThread(new LoanAssMatcherThreadPrivate(this), Identity, parent)
{}
LoanAssMatcherThread::LoanAssMatcherThread(LoanAssMatcherThreadPrivate* d, int ID, QObject* parent)
    : TemplAsyncThread<LoanAssMatcherResult>(d, ID, parent)
{}
const QString& LoanAssMatcherThread::GetModelToScan() const
{
    Q_D(const LoanAssMatcherThread);
    return d->m_ModelToScan;
}

void LoanAssMatcherThread::SetModelToScan(const QString& val)
{
    Q_D( LoanAssMatcherThread);
    d->m_ModelToScan = val;
}

void LoanAssMatcherThread::SetAvailableAssumptions(const QHash<QString, std::shared_ptr<LoanAssumption> >& a)
{
    Q_D( LoanAssMatcherThread);
    d->m_AvailableAssumptions = &a;
}

void LoanAssMatcherThread::EmitFromAnonim()
{
    Q_D( LoanAssMatcherThread);
    emit Calculated(d->Identifier, d->Result);
}

void LoanAssMatcherThread::run()
{
    Q_D( LoanAssMatcherThread);
    if (!d->m_AvailableAssumptions) {
        emit ErrorCalculation(d->Identifier);
        return;
    }
    d->Result.Clear();
    QFile ModelFile(d->m_ModelToScan);
	if (!ModelFile.open(QIODevice::ReadOnly)) return ReturnError();
	qint32 VersionChecker;
	QDataStream out(&ModelFile);
	out.setVersion(StreamVersionUsed);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		ModelFile.close();
		return ReturnError();;
	}
	{QDate Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	QSet<QString> NewModelName;
	{Waterfall Junk; Junk.SetLoadProtocolVersion(VersionChecker); out >> Junk; NewModelName = Junk.GetDealName(); }
	{Waterfall Junk; Junk.SetLoadProtocolVersion(VersionChecker); out >> Junk; }
	//QHash<qint32, std::shared_ptr<Mortgage> > LoanPool;
    MtgCalculator MtgCalc; MtgCalc.SetLoadProtocolVersion(VersionChecker); out >> MtgCalc;
	ModelFile.close();
    const auto& LoanPoolKeys = MtgCalc.GetResultKeys();

	for (auto i = LoanPoolKeys.constBegin(); i != LoanPoolKeys.constEnd(); ++i) {
        const auto tempLoan = MtgCalc.getLoan(*i);
        for (auto j = d->m_AvailableAssumptions->constBegin(); j != d->m_AvailableAssumptions->constEnd(); ++j) {
			for (const QString& CurrProperty : LoansPropertiesToSearch) {
                if (j.value()->MatchPattern(tempLoan.GetProperty(CurrProperty))) {
                    d->Result.AddScenario(j.value()->GetScenarioName(), tempLoan.GetProperty("Scenario"), tempLoan.GetProperty("Facility"), tempLoan.GetProperty("Issuer"), *i);
					break;
				}
			}
		}
	}
    if (d->Result.ScenarioCount() > 0) {
        d->Result.SetFilePath(d->m_ModelToScan);
        d->Result.SetDealName(NewModelName.isEmpty() ? QFileInfo(ModelFile).completeBaseName() : *NewModelName.begin());
	}
    emit AnonimCalculated(d->Identifier);
}

void LoanAssMatcherThread::ReturnError()
{
    Q_D( LoanAssMatcherThread);
    emit ErrorCalculation(d->Identifier);
}
