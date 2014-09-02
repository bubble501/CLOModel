#ifndef MtgCalculator_h__
#define MtgCalculator_h__
#include <QDate>
#include <QObject>
#include <QList>
#include "MtgCashFlow.h"
#include "CommonFunctions.h"
#include "BaseRateTable.h"
class Mortgage;
class MtgCashFlow;
class MtgCalculatorThread;
class MtgCalculator : public QObject, public BackwardInterface{
	Q_OBJECT
public:
	MtgCalculator(QObject* parent=0);
	~MtgCalculator();
	void AddLoan(const Mortgage& a);
	void SetStartDate(const QDate& a){StartDate=a;}
	bool StartCalculation();
	const QString& GetCPRass() const { return m_CPRass; }
	void SetCPRass(const QString& val) { m_CPRass = val; }
	const QString& GetCDRass() const { return m_CDRass; }
	void SetCDRass(const QString& val) { m_CDRass = val; }
	const QString& GetLSass() const { return m_LSass; }
	void SetLSass(const QString& val) { m_LSass = val; }
	const QString& GetRecoveryLag() const { return m_RecoveryLag; }
	void SetRecoveryLag(const QString& val) { m_RecoveryLag = val; }
	const QString& GetDelinquency() const { return m_Delinquency; }
	void SetDelinquency(const QString& val) { m_Delinquency = val; }
	const QString& GetDelinquencyLag() const { return m_DelinquencyLag; }
	void SetDelinquencyLag(const QString& val) { m_DelinquencyLag = val; }
	void CompileReferenceRateValue(ForwardBaseRateTable& Values)const;
	void CompileReferenceRateValue(ConstantBaseRateTable& Values)const;
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll=false)const;
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll=false)const;
#endif

	const QDate& GetStartDate()const {return StartDate;}
	const QList<Mortgage*>& GetLoans() const {return Loans;}
	QList<Mortgage*>& GetLoans() {return Loans;}
	const MtgCashFlow& GetResult()const{return Result;}
	void Reset();
	QString ReadyToCalculate()const;
	int Count() const{return Loans.size();}
	void SetSequentialComputation(bool a){SequentialComputation=a;}
	bool GetSequentialComputation()const {return SequentialComputation;}
private:
	QHash<int, MtgCalculatorThread*> ThreadPool;
	QList<Mortgage*> Loans;
	QString m_CPRass;
	QString m_CDRass;
	QString m_LSass;
	QString m_RecoveryLag;
	QString m_Delinquency;
	QString m_DelinquencyLag;
	
	QDate StartDate;
	MtgCashFlow Result;
	int BeesSent;
	int BeesReturned;
	bool SequentialComputation;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
private slots:
	void BeeReturned(int,const MtgCashFlow& a);
signals:
	void Calculated();
	void BeeCalculated(int);
	friend QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows);
	friend QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows);
};
QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows);
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows);
#endif // MtgCalculator_h__