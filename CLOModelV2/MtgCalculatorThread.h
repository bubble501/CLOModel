#ifndef MtgCalculatorThread_h__
#define MtgCalculatorThread_h__
#include "TemplAsyncThread.h"
#include "Mortgage.h"
class MtgCalculatorThread : public TemplAsyncThread <MtgCashFlow>{
	Q_OBJECT
protected:
	Mortgage Loan;
	QString CPRass;
	QString CDRass;
	QString LSass;
	QString RecoveryLag;
	QString Delinquency;
	QString DelinquencyLag;
	QDate StartDate;
	bool m_OverrideAssumptions;
	bool m_DownloadScenario;
public:
	MtgCalculatorThread(int Identity,QObject* parent=nullptr);
	void SetLoan(const Mortgage& a){Loan=a;}
	void SetLoanProperty(const QString& pn, const QString& pv) { Loan.SetProperty(pn, pv); }
	const Mortgage& Getloan() const{return Loan;}
	const MtgCashFlow& GetCashFlow()const{return Loan.GetCashFlow();}
	void SetCPR(const QString& a);
	void SetCDR(const QString& a);
	void SetLS(const QString& a);
	void SetRecoveryLag(const QString& a);
	void SetDelinquency(const QString& a);
	void SetDelinquencyLag(const QString& a);
	void SetStartDate(const QDate& a){StartDate=a;}
	const QString& GetCPR() const { return CPRass; }
	const QString& GetCDR() const { return CDRass; }
	const QString& GetCLR() const { return LSass; }
	const QString& GetRecoveryLag() const { return RecoveryLag; }
	const QString& GetDelinquency() const { return Delinquency; }
	const QString& GetDelinquencyLag() const { return DelinquencyLag; }
	const QDate& GetStartDate()const {return StartDate;}
	void SetOverrideAssumptions(bool a) { m_OverrideAssumptions = a; }
	bool GetOverrideAssumptions()const { return m_OverrideAssumptions; }
	bool GetDownloadScenario() const { return m_DownloadScenario; }
	void SetDownloadScenario(bool val) { m_DownloadScenario = val; }
signals:
	void Calculated(int,const MtgCashFlow&);
protected slots:
	virtual void EmitFromAnonim()override {
		emit Calculated(Identifier, Loan.GetCashFlow());
	}
public slots:
	virtual void run() override;
};
#endif // MtgCalculatorThread_h__