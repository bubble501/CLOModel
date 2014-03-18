#ifndef MtgCalculatorThread_h__
#define MtgCalculatorThread_h__
#include "BloombergVector.h"
#include <QThread>
#include "Mortgage.h"
class MtgCalculatorThread : public QThread{
	Q_OBJECT
private:
	Mortgage Loan;
	BloombergVector CPRass;
	BloombergVector CDRass;
	BloombergVector LSass;
	QDate StartDate;
public:
	MtgCalculatorThread(QObject* parent=0);
	void SetLoan(const Mortgage& a){Loan=a;}
	const Mortgage& Getloan() const{return Loan;}
	const MtgCashFlow& GetCashFlow()const{return Loan.GetCashFlow();}
	void SetCPR(const BloombergVector& a);
	void SetCDR(const BloombergVector& a);
	void SetLS(const BloombergVector& a);
	void SetStartDate(const QDate& a){StartDate=a;}
	const BloombergVector& GetCPR() const {return CPRass;}
	const BloombergVector& GetCDR() const {return CDRass;}
	const BloombergVector& GetCLR() const {return LSass;}
	const QDate& GetStartDate()const {return StartDate;}
signals:
	void Calculated(const MtgCashFlow&);
public slots:
	void run();
	void stop(){exit(0);}
};
#endif // MtgCalculatorThread_h__