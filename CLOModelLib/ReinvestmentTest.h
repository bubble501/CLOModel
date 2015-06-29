#ifndef ReinvestmentTest_h__
#define ReinvestmentTest_h__
#include "BackwardCompatibilityInterface.h"
#include "IntegerVector.h"
#include <QDate>
class ReinvestmentTestPrivate;
class GenericCashFlow;
class QDate;
class BloombergVector;
class Mortgage;
class MtgCashFlow;
class ConstantBaseRateTable;
class ForwardBaseRateTable;
class CLOMODELLIB_EXPORT ReinvestmentTest : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(ReinvestmentTest)
    DECLARE_PUBLIC_COMMONS_COPY(ReinvestmentTest)
public:
	const GenericCashFlow& GetReinvested() const;
	const QDate& GetReinvestmentPeriod() const;
	const Mortgage& GetReinvestmentBond() const;
	const BloombergVector& GetCPRAssumption() const;
	const BloombergVector& GetCDRAssumption() const;
	const BloombergVector& GetLSAssumption() const;
	const BloombergVector& GetWALAssumption() const;
	const IntegerVector& GetReinvDelay()const;
	const BloombergVector& GetReinvPrice()const;
	void ClearFlows();
	double GetQueuedCash(QDate StartDate) const;
	bool ReinvestQueueueEmpty() const;
	void ResetReinvestQueueue();
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
	const IntegerVector&  GetDelinquencyLag() const;
	const IntegerVector&  GetRecoveryLag() const;
	const BloombergVector& GetDelinquency() const;
	IntegerVector GetReinvestmentSpreadOverTime() const;
	void SetReinvestmentSpreadOverTime(const QString& val);
	void CompileReferenceRateValue(ForwardBaseRateTable& Values);
	void CompileReferenceRateValue(ConstantBaseRateTable& Values);
	void RemoveBondFlow(const QDate& a);
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false);
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false);
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;

};
CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
#endif // ReinvestmentTest_h__
