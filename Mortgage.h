#ifndef Mortgage_h__
#define Mortgage_h__

#include "MtgCashFlow.h"
#include <QDate>
#include <QString>
class Mortgage {
private:
	QDate m_MaturityDate;
	QString m_AnnuityVect;
	double m_Size;
	int m_LossMultiplier;
	int m_PrepayMultiplier;
	QString m_InterestVect;
	MtgCashFlow m_CashFlows;
	double m_FloatingRateBase;
	int m_PaymentFreq;
	double pmt(double Interest, int Periods, double PresentValue) const;
public:
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
	int GetLossMultiplier() const {return m_LossMultiplier;}
	void SetLossMultiplier(int a) {if(a>=0) m_LossMultiplier=a;}
	int GetPrepayMultiplier() const {return m_PrepayMultiplier;}
	void SetPrepayMultiplier(int a) {if(a>=0) m_PrepayMultiplier=a;}
	const MtgCashFlow& GetCashFlow() const{return m_CashFlows;}
	const QString& GetAnnuity() const {return m_AnnuityVect;}
	void SetAnnuity(const QString& a);
	const QString& GetInterest() const {return m_InterestVect;}
	void SetInterest(const QString& a);
	double GetSize() const{return m_Size;}
	void SetSize(double a){if(a>=0) m_Size=a;}
	void CalculateCashFlows(const QString& CPRVec,const QString& CDRVec,const QString& LossVec,const QDate& StartDate);
	QString ReadyToCalculate() const;
	friend QDataStream& operator<<(QDataStream & stream, const Mortgage& flows);
	friend QDataStream& operator>>(QDataStream & stream, Mortgage& flows);
};
QDataStream& operator<<(QDataStream & stream, const Mortgage& flows);
QDataStream& operator>>(QDataStream & stream, Mortgage& flows);
#endif // Mortgage_h__