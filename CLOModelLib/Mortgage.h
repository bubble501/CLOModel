#ifndef Mortgage_h__
#define Mortgage_h__
#include "MtgCashFlow.h"
#include "IntegerVector.h"
#include "BloombergVector.h"
class LoanAssumption;
class MortgagePrivate;
class MtgCashFlow;
class BaseRateVector;
class ForwardBaseRateTable;
class ConstantBaseRateTable;
class Mortgage;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const Mortgage& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, Mortgage& flows);
class CLOMODELLIB_EXPORT Mortgage : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(Mortgage)
    DECLARE_PUBLIC_COMMONS_COPY(Mortgage)
protected:
	void FillDiscountOutstanding();
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	double GetInterest(const QDate& a, const QDate& StartAccrue, const QDate& EndAccrue, DayCountConvention DayCnt);
	double GetInterest(int a, const QDate& StartAccrue, const QDate& EndAccrue, DayCountConvention DayCnt);
	double GetInterest(const QDate& a);
	double GetInterest(int a);
	QString GetProperty(qint32 PropIndex, bool PropValue) const;
public:
	static double pmt(double Interest, int Periods, double PresentValue);
	const IntegerVector& GetPaymentFreq() const;
	void SetPaymentFreq(const QString& a);
	const BaseRateVector& GetFloatingRateBase() const;
	const BloombergVector& GetFloatingRateValue() const;
	bool GetUseForwardCurve() const;
	void CompileReferenceRateValue(ForwardBaseRateTable& Values);
	void CompileReferenceRateValue(ConstantBaseRateTable& Values);
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false);
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false);
	void SetCashFlows(const MtgCashFlow& a);
	//void SetFloatingRateBase(const BaseRateVector& a) { m_FloatRateBase = a; }
	void SetScenario(const LoanAssumption& value, bool OverrideCurrent
		#ifdef Assumptions_ExcelOverDB
			=false
		#else
			=true
		#endif
		);
	void SetFloatingRateBase(const QString& a);
	void setFixedRate();
	const QDate& GetMaturityDate() const;
	void SetMaturityDate(const QDate& a);
	const MtgCashFlow& GetCashFlow() const;
	MtgCashFlow& GetCashFlow();
	QString GetAnnuity() const;
	void SetAnnuity(const QString& a);
	QString GetInterest() const;
	void SetInterest(const QString& a);
	double GetSize() const;
	void SetSize(double a);
	bool CalculateCashFlows(const QDate& StartDate, const QString& CPRVecs, const QString& CDRVecs, const QString& LossVecs, const QString& RecoveryLag = "0", const QString& Delinquency = "0", const QString& DelinquencyLag = "0", bool OverrideProperties = false, bool DownloadScenario = true);
	bool CalculateCashFlows(const QDate& StartDate, BloombergVector CPRVec, BloombergVector CDRVec, BloombergVector LossVec, IntegerVector  RecoveryLag = IntegerVector("0"), BloombergVector Delinquency = BloombergVector("0"), IntegerVector DelinquencyLag = IntegerVector("0"), bool OverrideProperties = false, bool DownloadScenario = true);
	void AddCashFlow(const QDate& Dte, double Amt, MtgCashFlow::MtgFlowType FlowTpe);
	void AddCashFlow(const MtgCashFlow& a);
	QString ReadyToCalculate() const;
	void ResetFlows();
	void SetProperty(const QString& PropName, const QString& Value);
	void RemoveProperty(const QString& PropName);
	void RemoveProperty(qint32 PropName);
	QString GetProperty(const QString& PropName) const;
	QString GetPropertyValue(qint32 PropIndex) const;
	QString GetPropertyName(qint32 PropIndex) const;
	bool HasProperty(const QString& PropName) const;
	qint32 GetNumProperties()const;
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Mortgage& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Mortgage& flows);
};

#endif // Mortgage_h__