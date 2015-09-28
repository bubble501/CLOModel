#ifndef MtgCalculatorThread_h__
#define MtgCalculatorThread_h__
#include "TemplAsyncThread.h"
class MtgCashFlow;
class Mortgage;
class MtgCalculatorThreadPrivate;
class CLOMODELLIB_EXPORT MtgCalculatorThread : public TemplAsyncThread <MtgCashFlow>
{
	Q_OBJECT
protected:
    Q_DECLARE_PRIVATE(MtgCalculatorThread)
    MtgCalculatorThread(MtgCalculatorThreadPrivate* d, int ID, QObject* parent = nullptr);
public:
    virtual ~MtgCalculatorThread();
	MtgCalculatorThread(int Identity,QObject* parent=nullptr);
	void SetLoan(const Mortgage& a);
	void SetLoanProperty(const QString& pn, const QString& pv);
	const Mortgage& Getloan() const;
	const MtgCashFlow& GetCashFlow()const;
	void SetCPR(const QString& a);
	void SetCDR(const QString& a);
	void SetLS(const QString& a);
	void SetRecoveryLag(const QString& a);
	void SetDelinquency(const QString& a);
	void SetDelinquencyLag(const QString& a);
	void SetStartDate(const QDate& a);
	const QString& GetCPR() const;
	const QString& GetCDR() const;
	const QString& GetCLR() const;
	const QString& GetRecoveryLag() const;
	const QString& GetDelinquency() const;
	const QString& GetDelinquencyLag() const;
	const QDate& GetStartDate()const;
	void SetOverrideAssumptions(bool a);
	bool GetOverrideAssumptions()const;
	bool GetDownloadScenario() const;
	void SetDownloadScenario(bool val);
signals:
	void Calculated(int,const MtgCashFlow&);
protected slots:
	virtual void EmitFromAnonim()override;
public slots:
	virtual void run() override;
};
#endif // MtgCalculatorThread_h__