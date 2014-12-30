#ifndef MtgCalculator_h__
#define MtgCalculator_h__
#include <QDate>
#include "TemplAsyncCalculator.h"
#include <QList>
#include "MtgCashFlow.h"
#include "CommonFunctions.h"
#include "BaseRateTable.h"
#include "MtgCalculatorThread.h"
class Mortgage;
class MtgCashFlow;
class MtgCalculator : public TemplAsyncCalculator <MtgCalculatorThread,MtgCashFlow>{
	Q_OBJECT
public slots:
	virtual bool StartCalculation() override;
public:
	virtual QString ReadyToCalculate() const override;
	MtgCalculator(QObject* parent = 0);
	virtual ~MtgCalculator() { Reset(); }
	virtual void Reset() override;
	virtual void ClearResults() override;
	virtual int NumBees() const override { return Loans.size(); }
	void AddLoan(const Mortgage& a, qint32 Index);
	const QHash<qint32, Mortgage*>& GetLoans() const { return Loans; }
	QHash<qint32, Mortgage*>& GetLoans() { return Loans; }
	void SetLoans(const QHash<qint32, Mortgage*>& a);
	void SetStartDate(const QDate& a){StartDate=a;}
	const QDate& GetStartDate()const { return StartDate; }
	bool GetUseStoredCashFlows() const { return m_UseStoredCashFlows; }
	void SetUseStoredCashFlows(bool val) { m_UseStoredCashFlows = val; }
	const QString& GetCPRass() const { return m_CPRass; }
	void SetCPRass(const QString& val) { m_CPRass = val; }
	const QString& GetCDRass() const { return m_CDRass; }
	void SetCDRass(const QString& val) { m_CDRass = val; }
	const QString& GetLSass() const { return m_LSass; }
	void SetLSass(const QString& val) { m_LSass = val; }
	const QString& GetRecoveryLag() const { return m_RecoveryLag; }
	void SetRecoveryLag(const QString& val) { m_RecoveryLag = val; }
	const QString& GetDelinquency() const { return m_Delinquency; }
	void SetDelinquency(const QString& val) { m_Delinquency = val; }
	const QString& GetDelinquencyLag() const { return m_DelinquencyLag; }
	void SetDelinquencyLag(const QString& val) { m_DelinquencyLag = val; }
	void CompileReferenceRateValue(ForwardBaseRateTable& Values);
	void CompileReferenceRateValue(ConstantBaseRateTable& Values);
	void ClearLoans();
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll=false);
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll=false);
	void DownloadScenarios();
	void GuessLoanScenarios(bool OverrideAss);
#endif
	void SetOverrideAssumptions(bool a) { m_OverrideAssumptions = a; }
	bool GetOverrideAssumptions()const { return m_OverrideAssumptions; }
	const MtgCashFlow& GetAggregatedResults()const { return m_AggregatedRes; }
	bool GetDownloadScenario() const { return m_DownloadScenario; }
	void SetDownloadScenario(bool val) { m_DownloadScenario = val; }
private:
	QHash<qint32, Mortgage*> Loans;
	QString m_CPRass;
	QString m_CDRass;
	QString m_LSass;
	QString m_RecoveryLag;
	QString m_Delinquency;
	QString m_DelinquencyLag;
	bool m_OverrideAssumptions;
	bool m_DownloadScenario;
	bool m_UseStoredCashFlows;
	QDate StartDate;
	MtgCashFlow m_AggregatedRes;
protected:
	virtual void BeeReturned(int Ident, const MtgCashFlow& a) override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows);
	friend QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows);
};
QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows);
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows);
#endif // MtgCalculator_h__