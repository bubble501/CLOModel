#include "MtgCalculatorThread.h"
#include "CommonFunctions.h"
MtgCalculatorThread::MtgCalculatorThread(int Identity,QObject* parent)
	:QThread(parent)
	,CPRass("0")
	,CDRass("0")
	,LSass("0")
	, RecoveryLag("0")
	, Delinquency("0")
	, DelinquencyLag("0")
	, m_ID(Identity)
	, m_OverrideAssumptions(false)
{
	connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}
void MtgCalculatorThread::run(){
	if(Loan.CalculateCashFlows(StartDate, CPRass, CDRass, LSass, RecoveryLag, Delinquency, DelinquencyLag,m_OverrideAssumptions))
		emit Calculated(m_ID,Loan.GetCashFlow());
	else
		emit ErrorCalculation(m_ID);
	exec();
}
void MtgCalculatorThread::SetCPR(const QString& a) {
	CPRass=a;
}
void MtgCalculatorThread::SetCDR(const QString& a) {
	CDRass=a;
}
void MtgCalculatorThread::SetLS(const QString& a) {
	LSass=a;
}
void MtgCalculatorThread::SetRecoveryLag(const QString& a) { RecoveryLag = a; }
void MtgCalculatorThread::SetDelinquency(const QString& a) {  Delinquency = a; }
void MtgCalculatorThread::SetDelinquencyLag(const QString& a) { DelinquencyLag = a; }