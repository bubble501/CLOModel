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
	const QDate& GetReinvestmentPeriod() const {return ReinvestmentPeriod;}
	const Mortgage& GetReinvestmentBond() const {return ReinvestmentBond;}
	QString GetCPRAssumption() const {return CPRAssumption.GetVector();}
	QString GetCDRAssumption() const {return CDRAssumption.GetVector();}
	QString GetLSAssumption() const {return LSAssumption.GetVector();}
	QString GetWALAssumption() const {return WALAssumption.GetVector();}
	QString GetReinvDelay()const { return m_ReinvestmentDelay.GetVector(); }
	QString GetReinvPrice()const { return ReinvestmentPrice.GetVector(); }
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
	void SetMissingAnchors(const QDate& a);
	void SetReinvestementPeriod(const QDate& ReinvPer);
	void CalculateBondCashFlows(double Size, QDate StartDate, int Period=0);
	const MtgCashFlow& GetBondCashFlow() const;
	void SetCPR(const QString& a);
	void SetCDR(const QString& a);
	void SetLS(const QString& a);
	void SetReinvestmentDelay(const QString& a);
	void SetRecoveryLag(const QString& a);
	void SetDelinquency(const QString& a);
	void SetDelinquencyLag(const QString& a);
	QString GetDelinquencyLag() const { return m_DelinquencyLag.GetVector(); }
	QString GetRecoveryLag() const { return m_RecoveryLag.GetVector(); }
	QString GetDelinquency() const { return m_Delinquency.GetVector(); }
	QString GetReinvestmentSpreadOverTime() const { return m_ReinvestmentSpreadOverTime.GetVector(); }
	void SetReinvestmentSpreadOverTime(const QString& val) { m_ReinvestmentSpreadOverTime = val; }
	void CompileReferenceRateValue(ForwardBaseRateTable& Values) { ReinvestmentBond.CompileReferenceRateValue(Values); }
	void CompileReferenceRateValue(ConstantBaseRateTable& Values) { ReinvestmentBond.CompileReferenceRateValue(Values); }
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false) { ReinvestmentBond.GetBaseRatesDatabase(Values, DownloadAll); }
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false) { ReinvestmentBond.GetBaseRatesDatabase(Values, DownloadAll); }
#endif
	friend QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
	friend QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
protected:
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
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;

};
QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
#endif // ReinvestmentTest_h__
