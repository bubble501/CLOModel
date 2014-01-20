#ifndef MtgCalculator_h__
#define MtgCalculator_h__
#include <QDate>
#include <QObject>
#include <QList>
#include "MtgCashFlow.h"
class Mortgage;
class MtgCashFlow;
class MtgCalculator : public QObject{
	Q_OBJECT
public:
	MtgCalculator(QObject* parent=0);
	~MtgCalculator(){Reset();}
	void AddLoan(const Mortgage& a);
	void SetCPR(const QString& a);
	void SetCDR(const QString& a);
	void SetLS(const QString& a);
	void SetStartDate(const QDate& a){StartDate=a;}
	void StartCalculation();
	const QString& GetCPR() const {return CPRass;}
	const QString& GetCDR() const {return CDRass;}
	const QString& GetCLR() const {return LSass;}
	const QDate& GetStartDate()const {return StartDate;}
	const QList<Mortgage*>& GetLoans() const {return Loans;}
	const MtgCashFlow& GetResult()const{return Result;}
	void Reset();
	QString ReadyToCalculate()const;
	int Count() const{return Loans.size();}
private:
	QList<Mortgage*> Loans;
	QString CPRass;
	QString CDRass;
	QString LSass;
	QDate StartDate;
	MtgCashFlow Result;
	int BeesSent;
	int BeesReturned;
private slots:
	void BeeReturned(const MtgCashFlow& a);
signals:
	void Calculated();
	void BeeCalculated(int);
};
#endif // MtgCalculator_h__