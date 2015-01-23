#ifndef ReinvestmentTest_h__
#define ReinvestmentTest_h__
#include <QDate>
#include <QString>
#include "Mortgage.h"
#include "BloombergVector.h"
#include "IntegerVector.h"
class ReinvestmentTest : public BackwardInterface{
public:
	ReinvestmentTest();
	ReinvestmentTest(const ReinvestmentTest& a);
	ReinvestmentTest& operator=(const ReinvestmentTest& a);
	const GenericCashFlow& GetReinvested() const { return m_Reinvested; }
	const QDate& GetReinvestmentPeriod() const {return ReinvestmentPeriod;}
	const Mortgage& GetReinvestmentBond() const {return ReinvestmentBond;}
	const BloombergVector& GetCPRAssumption() const { return CPRAssumption; }
	const BloombergVector& GetCDRAssumption() const { return CDRAssumption; }
	const BloombergVector& GetLSAssumption() const { return LSAssumption; }
	const BloombergVector& GetWALAssumption() const { return WALAssumption; }
	const IntegerVector& GetReinvDelay()const { return m_ReinvestmentDelay; }
	const BloombergVector& GetReinvPrice()const { return ReinvestmentPrice; }
	void ClearFlows() { m_Reinvested.Clear(); ReinvestmentBond.ResetFlows(); }
	double GetQueuedCash(QDate StartDate=QDate()) const;
	bool ReinvestQueueueEmpty() const { return ReinvestQueue.isEmpty(); }
	void ResetReinvestQueueue() { ReinvestQueue.clear(); }
	void SetupReinvBond(
		const QString& IntrVec
		, const QString& CPRVec
		, const QString& CDRVec
		, const QString& LSVec
		, const QString& WALval
		, const QString& PayFreq = "1"
		, const QString& AnnuityVec = "I"
		, const QString& ReinvPric = "100"
		, const QString& ReinvDel = "0"
		, const QString& ReinvSpr = "1"
		, const QString& FloatingBase = "ZERO"
		, const QString& RecoveryLag = "0"
		, const QString& Delinquency = "0"
		, const QString& DelinquencyLag = "0"
	);
	void QueueReinvestments(double Amount, const QDate& CurrentDate);
	const MtgCashFlow& ProcessQueue(const QDate& CurrentDate, int Period, const QDate& MaxMaturity=QDate());
	void SetMissingAnchors(const QDate& a);
	void SetReinvestementPeriod(const QDate& ReinvPer);
	void CalculateBondCashFlows(double Size, QDate StartDate, int Period, const QDate& MaxMaturity = QDate());
	const MtgCashFlow& GetBondCashFlow() const;
	void SetCPR(const QString& a);
	void SetCDR(const QString& a);
	void SetLS(const QString& a);
	void SetReinvestmentDelay(const QString& a);
	void SetRecoveryLag(const QString& a);
	void SetDelinquency(const QString& a);
	void SetDelinquencyLag(const QString& a);
	const IntegerVector&  GetDelinquencyLag() const { return m_DelinquencyLag; }
	const IntegerVector&  GetRecoveryLag() const { return m_RecoveryLag; }
	const BloombergVector& GetDelinquency() const { return m_Delinquency; }
	IntegerVector GetReinvestmentSpreadOverTime() const { return m_ReinvestmentSpreadOverTime.GetVector(); }
	void SetReinvestmentSpreadOverTime(const QString& val) { m_ReinvestmentSpreadOverTime = val; }
	void CompileReferenceRateValue(ForwardBaseRateTable& Values) { ReinvestmentBond.CompileReferenceRateValue(Values); }
	void CompileReferenceRateValue(ConstantBaseRateTable& Values) { ReinvestmentBond.CompileReferenceRateValue(Values); }
	void RemoveBondFlow(const QDate& a);
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false) { ReinvestmentBond.GetBaseRatesDatabase(Values, DownloadAll); }
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false) { ReinvestmentBond.GetBaseRatesDatabase(Values, DownloadAll); }
#endif
	friend QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
	friend QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
protected:
	GenericCashFlow m_Reinvested;
	QDate ReinvestmentPeriod;
	Mortgage ReinvestmentBond;
	BloombergVector CPRAssumption;
	BloombergVector CDRAssumption;
	BloombergVector LSAssumption;
	BloombergVector WALAssumption;
	BloombergVector ReinvestmentPrice;
	IntegerVector m_ReinvestmentDelay;
	IntegerVector m_RecoveryLag;
	BloombergVector m_Delinquency;
	IntegerVector m_DelinquencyLag;
	IntegerVector m_ReinvestmentSpreadOverTime;
	QHash<QDate,double> ReinvestQueue;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;

};
QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
#endif // ReinvestmentTest_h__
