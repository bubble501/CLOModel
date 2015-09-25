#include "MtgCalculatorThread.h"
#include "Private/MtgCalculatorThread_p.h"
#include "CommonFunctions.h"
MtgCalculatorThreadPrivate::~MtgCalculatorThreadPrivate(){}
MtgCalculatorThreadPrivate::MtgCalculatorThreadPrivate(MtgCalculatorThread *q)
    :AbstrAsyncThreadPrivate(q)
{}
MtgCalculatorThread::MtgCalculatorThread(int Identity, QObject *parent)
    : MtgCalculatorThread(new MtgCalculatorThreadPrivate(this), Identity, parent)
{}
MtgCalculatorThread::MtgCalculatorThread(MtgCalculatorThreadPrivate* d, int ID, QObject* parent)
    : TemplAsyncThread<MtgCashFlow>(d, ID, parent)
{
    d->CPRass = "0";
    d->CDRass = "0";
    d->LSass = "0";
    d->RecoveryLag = "0";
    d->Delinquency = "0";
    d->DelinquencyLag = "0";
    d->m_OverrideAssumptions = false;
    d->m_DownloadScenario = false;
}

void MtgCalculatorThread::SetLoan(const Mortgage& a)
{
    Q_D(MtgCalculatorThread);
    d->Loan = a;
}

void MtgCalculatorThread::SetLoanProperty(const QString& pn, const QString& pv)
{
    Q_D(MtgCalculatorThread);
    d->Loan.SetProperty(pn, pv);
}

const Mortgage& MtgCalculatorThread::Getloan() const
{
    Q_D(const MtgCalculatorThread);
    return d->Loan;
}

const MtgCashFlow& MtgCalculatorThread::GetCashFlow() const
{
    Q_D(const MtgCalculatorThread);
    return d->Loan.GetCashFlow();
}

void MtgCalculatorThread::EmitFromAnonim()
{
    Q_D( MtgCalculatorThread);
    emit Calculated(d->Identifier, d->Loan.GetCashFlow());
}

void MtgCalculatorThread::run()
{
    Q_D( MtgCalculatorThread);
    if (d->Loan.CalculateCashFlows(d->StartDate, d->CPRass, d->CDRass, d->LSass, d->RecoveryLag, d->Delinquency, d->DelinquencyLag, d->m_OverrideAssumptions, d->m_DownloadScenario))
        emit AnonimCalculated(d->Identifier);
	else
        emit ErrorCalculation(d->Identifier);
	exec();
}
void MtgCalculatorThread::SetCPR(const QString& a) { 
    Q_D(MtgCalculatorThread); 
    d->CPRass = a; 
}
void MtgCalculatorThread::SetCDR(const QString& a)
{
    Q_D(MtgCalculatorThread);
    d->CDRass = a; 
}
void MtgCalculatorThread::SetLS(const QString& a)
{
    Q_D(MtgCalculatorThread);
    d->LSass = a; 
}
void MtgCalculatorThread::SetRecoveryLag(const QString& a)
{
    Q_D(MtgCalculatorThread);
    d->RecoveryLag = a; 
}
void MtgCalculatorThread::SetDelinquency(const QString& a)
{
    Q_D(MtgCalculatorThread);
    d->Delinquency = a;
}
void MtgCalculatorThread::SetDelinquencyLag(const QString& a)
{
    Q_D(MtgCalculatorThread);
    d->DelinquencyLag = a; 
}
void MtgCalculatorThread::SetStartDate(const QDate& a)
{
    Q_D( MtgCalculatorThread);
    d->StartDate = a;
}

const QString& MtgCalculatorThread::GetCPR() const
{
    Q_D(const MtgCalculatorThread);
    return d->CPRass;
}

const QString& MtgCalculatorThread::GetCDR() const
{
    Q_D(const MtgCalculatorThread);
    return d->CDRass;
}

const QString& MtgCalculatorThread::GetCLR() const
{
    Q_D(const MtgCalculatorThread);
    return d->LSass;
}

const QString& MtgCalculatorThread::GetRecoveryLag() const
{
    Q_D(const MtgCalculatorThread);
    return d->RecoveryLag;
}

const QString& MtgCalculatorThread::GetDelinquency() const
{
    Q_D(const MtgCalculatorThread);
    return d->Delinquency;
}

const QString& MtgCalculatorThread::GetDelinquencyLag() const
{
    Q_D(const MtgCalculatorThread);
    return d->DelinquencyLag;
}

const QDate& MtgCalculatorThread::GetStartDate() const
{
    Q_D(const MtgCalculatorThread);
    return d->StartDate;
}

void MtgCalculatorThread::SetOverrideAssumptions(bool a)
{
    Q_D( MtgCalculatorThread);
    d->m_OverrideAssumptions = a;
}

bool MtgCalculatorThread::GetOverrideAssumptions() const
{
    Q_D(const MtgCalculatorThread);
    return d->m_OverrideAssumptions;
}

bool MtgCalculatorThread::GetDownloadScenario() const
{
    Q_D(const MtgCalculatorThread);
    return d->m_DownloadScenario;
}

void MtgCalculatorThread::SetDownloadScenario(bool val)
{
    Q_D( MtgCalculatorThread);
    d->m_DownloadScenario = val;
}
