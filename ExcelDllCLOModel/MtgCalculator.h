#ifndef MtgCalculator_h__
#define MtgCalculator_h__
#include <QDate>
#include <QObject>
#include <QList>
#include "MtgCashFlow.h"
#include "CommonFunctions.h"
#include "BloombergVector.h"
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
	QString GetCPR() const {return CPRass.GetVector();}
	QString GetCDR() const {return CDRass.GetVector();}
	QString GetCLR() const {return LSass.GetVector();}
	const QDate& GetStartDate()const {return StartDate;}
	const QList<Mortgage*>& GetLoans() const {return Loans;}
	const MtgCashFlow& GetResult()const{return Result;}
	void Reset();
	QString ReadyToCalculate()const;
	int Count() const{return Loans.size();}
	void SetSequentialComputation(bool a){SequentialComputation=a;}
	bool GetSequentialComputation()const {return SequentialComputation;}
private:
	QList<Mortgage*> Loans;
	BloombergVector CPRass;
	BloombergVector CDRass;
	BloombergVector LSass;
	QDate StartDate;
	MtgCashFlow Result;
	int BeesSent;
	int BeesReturned;
	bool SequentialComputation;
private slots:
	void BeeReturned(const MtgCashFlow& a);
signals:
	void Calculated();
	void BeeCalculated(int);
	friend QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows);
	friend QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows);
};
QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows);
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows);
#endif // MtgCalculator_h__