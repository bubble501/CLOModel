#ifndef Mortgage_h__
#define Mortgage_h__
#include "MtgCashFlow.h"
#include <QDate>
#include <QString>
#include "BloombergVector.h"
#include "RepaymentVector.h"
#include "IntegerVector.h"
#include "BaseRateVect.h"
#include "BaseRateTable.h"
#include "DayCountVect.h"
class Mortgage : public BackwardInterface{
private:
	QString m_Properties;
	QDate m_MaturityDate;
	RepaymentVector m_AnnuityVect;
	double m_Size;
	BloombergVector m_InterestVect;
	MtgCashFlow m_CashFlows;
	BaseRateVector m_FloatRateBase;
	BloombergVector m_FloatingRateBaseValue;
	IntegerVector m_PaymentFreq;
	bool m_UseForwardCurve;
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
	Mortgage();
	Mortgage(const Mortgage& a);
	const Mortgage& operator=(const Mortgage& a);
	const IntegerVector& GetPaymentFreq() const {return m_PaymentFreq;}
	void SetPaymentFreq(const QString& a) {m_PaymentFreq=a;}
	const BaseRateVector& GetFloatingRateBase() const { return m_FloatRateBase; }
	const BloombergVector& GetFloatingRateValue() const { return m_FloatingRateBaseValue; }
	bool GetUseForwardCurve() const { return m_UseForwardCurve; }
	void CompileReferenceRateValue(ForwardBaseRateTable& Values) { m_FloatingRateBaseValue = m_FloatRateBase.CompileReferenceRateValue(Values); m_UseForwardCurve = true; }
	void CompileReferenceRateValue(ConstantBaseRateTable& Values) { m_FloatingRateBaseValue = m_FloatRateBase.CompileReferenceRateValue(Values); m_UseForwardCurve = false; }
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false) { m_FloatingRateBaseValue = m_FloatRateBase.GetBaseRatesDatabase(Values, DownloadAll); m_UseForwardCurve = false; }
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false) { m_FloatingRateBaseValue = m_FloatRateBase.GetBaseRatesDatabase(Values, DownloadAll); m_UseForwardCurve = true; }
#endif

	//void SetFloatingRateBase(const BaseRateVector& a) { m_FloatRateBase = a; }
	void SetFloatingRateBase(const QString& a) { m_FloatRateBase = BaseRateVector(a); }
	void setFixedRate() { m_FloatingRateBaseValue = "0"; m_FloatRateBase = "ZERO"; }
	const QDate& GetMaturityDate() const {return m_MaturityDate;}
	void SetMaturityDate(const QDate& a) {m_MaturityDate=a;}
	const MtgCashFlow& GetCashFlow() const{return m_CashFlows;}
	MtgCashFlow& GetCashFlow(){return m_CashFlows;}
	QString GetAnnuity() const {return m_AnnuityVect.GetVector();}
	void SetAnnuity(const QString& a);
	QString GetInterest() const {return m_InterestVect.GetVector();}
	void SetInterest(const QString& a);
	double GetSize() const{return m_Size;}
	void SetSize(double a){if(a>=0) m_Size=a;}
	bool CalculateCashFlows(const QDate& StartDate,const QString& CPRVecs, const QString& CDRVecs, const QString& LossVecs, const QString& RecoveryLag = "0", const QString& Delinquency = "0", const QString& DelinquencyLag = "0");
	bool CalculateCashFlows(const QDate& StartDate, BloombergVector CPRVec, BloombergVector CDRVec, BloombergVector LossVec, IntegerVector  RecoveryLag = IntegerVector("0"), BloombergVector Delinquency = BloombergVector("0"), IntegerVector DelinquencyLag = IntegerVector("0"), bool OverrideProperties=false);
	void AddCashFlow(const QDate& Dte, double Amt, MtgCashFlow::MtgFlowType FlowTpe) { m_CashFlows.AddFlow(Dte, Amt, FlowTpe); }
	void AddCashFlow(const MtgCashFlow& a) { m_CashFlows.AddFlow(a); }
	QString ReadyToCalculate() const;
	void ResetFlows() { m_CashFlows.Clear(); }
	void SetProperty(const QString& PropName, const QString& Value);
	void RemoveProperty(const QString& PropName);
	void RemoveProperty(qint32 PropName);
	QString GetProperty(const QString& PropName) const;
	QString GetPropertyValue(qint32 PropIndex) const { return GetProperty(PropIndex, true); }
	QString GetPropertyName(qint32 PropIndex) const { return GetProperty(PropIndex, false); }
	bool HasProperty(const QString& PropName) const;
	qint32 GetNumProperties()const;
	friend QDataStream& operator<<(QDataStream & stream, const Mortgage& flows);
	friend QDataStream& operator>>(QDataStream & stream, Mortgage& flows);
};
QDataStream& operator<<(QDataStream & stream, const Mortgage& flows);
QDataStream& operator>>(QDataStream & stream, Mortgage& flows);
#endif // Mortgage_h__