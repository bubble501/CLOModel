#include "MtgCalculatorThread.h"
#include "CommonFunctions.h"
MtgCalculatorThread::MtgCalculatorThread(QObject* parent)
	:QThread(parent)
	,CPRass("0")
	,CDRass("0")
	,LSass("0")
{
	connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}
void MtgCalculatorThread::run(){
	Loan.CalculateCashFlows(CPRass,CDRass,LSass,StartDate);
	emit Calculated(Loan.GetCashFlow());
	exec();
}
void MtgCalculatorThread::SetCPR(const BloombergVector& a){
	CPRass=a;
}
void MtgCalculatorThread::SetCDR(const BloombergVector& a){
	CDRass=a;
}
void MtgCalculatorThread::SetLS(const BloombergVector& a){
	LSass=a;
}