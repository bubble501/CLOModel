#ifndef MtgCalculatorThread_h__
#define MtgCalculatorThread_h__

#include <QThread>
#include "Mortgage.h"
class MtgCalculatorThread : public QThread{
	Q_OBJECT
private:
	Mortgage Loan;
	QString CPRass;
	QString CDRass;
	QString LSass;
	QDate StartDate;
public:
	MtgCalculatorThread(QObject* parent=0);
	void SetLoan(const Mortgage& a){Loan=a;}
	const Mortgage& Getloan() const{return Loan;}
	const MtgCashFlow& GetCashFlow()const{return Loan.GetCashFlow();}
	void SetCPR(const QString& a);
	void SetCDR(const QString& a);
	void SetLS(const QString& a);
	void SetStartDate(const QDate& a){StartDate=a;}
	const QString& GetCPR() const {return CPRass;}
	const QString& GetCDR() const {return CDRass;}
	const QString& GetCLR() const {return LSass;}
	const QDate& GetStartDate()const {return StartDate;}
signals:
	void Calculated(const MtgCashFlow&);
public slots:
	void run();
	void stop(){exit(0);}
};
#endif // MtgCalculatorThread_h__