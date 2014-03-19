#ifndef Mortgage_h__
#define Mortgage_h__
#include "MtgCashFlow.h"
#include <QDate>
#include <QString>
#include "AnnuityVector.h"
#include "BloombergVector.h"
class Mortgage {
private:
	QDate m_MaturityDate;
	AnnuityVector m_AnnuityVect;
	double m_Size;
	BloombergVector m_LossMultiplier;
	BloombergVector m_PrepayMultiplier;
	BloombergVector m_InterestVect;
	MtgCashFlow m_CashFlows;
	double m_FloatingRateBase;
	int m_PaymentFreq;
public:
	static double pmt(double Interest, int Periods, double PresentValue);
	Mortgage();
	Mortgage(const Mortgage& a);
	const Mortgage& operator=(const Mortgage& a);
	int GetPaymentFreq() const {return m_PaymentFreq;}
	void SetPaymentFreq(int a) {if(a>=1) m_PaymentFreq=a;}
	double GetFloatingRateBase() const {return m_FloatingRateBase;}
	void SetFloatingRateBase(double a) {m_FloatingRateBase=a;}
	void setFixedRate(){m_FloatingRateBase=0.0;}
	const QDate& GetMaturityDate() const {return m_MaturityDate;}
	void SetMaturityDate(const QDate& a) {m_MaturityDate=a;}
	QString GetLossMultiplier() const {return m_LossMultiplier.GetVector();}
	void SetLossMultiplier(QString a) {m_LossMultiplier=a;}
	QString GetPrepayMultiplier() const {return m_PrepayMultiplier.GetVector();}
	void SetPrepayMultiplier(QString a) {m_PrepayMultiplier=a;}
	const MtgCashFlow& GetCashFlow() const{return m_CashFlows;}
	QString GetAnnuity() const {return m_AnnuityVect.GetVector();}
	void SetAnnuity(const QString& a);
	QString GetInterest() const {return m_InterestVect.GetVector();}
	void SetInterest(const QString& a);
	double GetSize() const{return m_Size;}
	void SetSize(double a){if(a>=0) m_Size=a;}
	void CalculateCashFlows(const QString& CPRVecs,const QString& CDRVecs,const QString& LossVecs,const QDate& StartDate);
	void CalculateCashFlows(const BloombergVector& CPRVecs,const BloombergVector& CDRVecs,const BloombergVector& LossVecs,const QDate& StartDate);
	QString ReadyToCalculate() const;
	void ResetFlows(){m_CashFlows.RemoveAllFlows();}
	friend QDataStream& operator<<(QDataStream & stream, const Mortgage& flows);
	friend QDataStream& operator>>(QDataStream & stream, Mortgage& flows);
};
QDataStream& operator<<(QDataStream & stream, const Mortgage& flows);
QDataStream& operator>>(QDataStream & stream, Mortgage& flows);
#endif // Mortgage_h__