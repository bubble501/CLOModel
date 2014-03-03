#ifndef Mortgage_h__
#define Mortgage_h__
#include <MtgCashFlow.h>
#include <QDate>
#include <QString>
class Mortgage {
public:
	static double pmt(double Interest, int Periods, double PresentValue);
	Mortgage();
	Mortgage(const Mortgage& a);
	const Mortgage& operator=(const Mortgage& a);
	int GetPaymentFreq() const;
	void SetPaymentFreq(int a);
	double GetFloatingRateBase() const;
	void SetFloatingRateBase(double a);
	void setFixedRate();
	const QDate& GetMaturityDate() const;
	void SetMaturityDate(const QDate& a);
	int GetLossMultiplier() const;
	void SetLossMultiplier(int a);
	int GetPrepayMultiplier() const;
	void SetPrepayMultiplier(int a);
	const MtgCashFlow& GetCashFlow() const;
	const QString& GetAnnuity() const;
	void SetAnnuity(const QString& a);
	const QString& GetInterest() const;
	void SetInterest(const QString& a);
	double GetSize() const;
	void SetSize(double a);
	void CalculateCashFlows(const QString& CPRVec,const QString& CDRVec,const QString& LossVec,const QDate& StartDate);
	QString ReadyToCalculate() const;
	void ResetFlows();
};
#endif // Mortgage_h__