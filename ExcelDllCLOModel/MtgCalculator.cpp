#include "MtgCalculator.h"
#include "MtgCalculatorThread.h"
#include "Mortgage.h"
MtgCalculator::MtgCalculator(QObject* parent)
	:QObject(parent)
	,SequentialComputation(false)
{

}
void MtgCalculator::SetCPR(const QString& a){
	CPRass=a;
}
void MtgCalculator::SetCDR(const QString& a){
	CDRass=a;
}
void MtgCalculator::SetLS(const QString& a){
	LSass=a;
}
void MtgCalculator::AddLoan(const Mortgage& a){
	Loans.append(new Mortgage(a));
}
void MtgCalculator::StartCalculation(){
	BeesReturned=0;
	int NumberOfThreads=QThread::idealThreadCount();
	if(SequentialComputation || NumberOfThreads<1) NumberOfThreads=1;
	for(BeesSent=0;BeesSent<Loans.size() && BeesSent<NumberOfThreads;BeesSent++){
		MtgCalculatorThread* Bee=new MtgCalculatorThread(this);
		Bee->SetLoan(*(Loans.at(BeesSent)));
		Bee->SetCPR(CPRass);
		Bee->SetCDR(CDRass);
		Bee->SetLS(LSass);
		Bee->SetStartDate(StartDate);
		connect(Bee,SIGNAL(Calculated(const MtgCashFlow&)),this,SLOT(BeeReturned(const MtgCashFlow&)));
		connect(Bee,SIGNAL(Calculated(const MtgCashFlow&)),Bee,SLOT(stop()),Qt::QueuedConnection);
		Bee->start();
	}
}
void MtgCalculator::BeeReturned(const MtgCashFlow& a){
	Result+=a;
	BeesReturned++;
	emit BeeCalculated(BeesReturned);
	if(BeesReturned==Loans.size()){
		emit Calculated();
		return;
	}
	if(BeesSent>=Loans.size()) return;
	MtgCalculatorThread* Bee=new MtgCalculatorThread(this);
	Bee->SetLoan(*(Loans.at(BeesSent++)));
	Bee->SetCPR(CPRass);
	Bee->SetCDR(CDRass);
	Bee->SetLS(LSass);
	Bee->SetStartDate(StartDate);
	connect(Bee,SIGNAL(Calculated(const MtgCashFlow&)),this,SLOT(BeeReturned(const MtgCashFlow&)));
	connect(Bee,SIGNAL(Calculated(const MtgCashFlow&)),Bee,SLOT(stop()),Qt::QueuedConnection);
	Bee->start();
}
void MtgCalculator::Reset(){
	for (QList<Mortgage*>::iterator i=Loans.begin();i!=Loans.end();i++){
		delete *i;
	}
	Loans.clear();
}
QString MtgCalculator::ReadyToCalculate()const{
	QString Result;
	QString TempStr;
	for (QList<Mortgage*>::const_iterator i=Loans.begin();i!=Loans.end();i++){
		TempStr=(*i)->ReadyToCalculate();
		if(!TempStr.isEmpty()) Result+=TempStr+'\n';
	}
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
	return Result;
}

QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows){
	stream 
		<< flows.Loans.size()
		<< flows.CPRass
		<< flows.CDRass
		<< flows.LSass
		<< flows.StartDate
	 ;
	for(QList<Mortgage*>::const_iterator i=flows.Loans.begin();i!=flows.Loans.end();i++){
		stream << **i;
	}
	return stream;
}
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows){
	int tempInt;
	stream 
		>> tempInt
		>> flows.CPRass
		>> flows.CDRass
		>> flows.LSass
		>> flows.StartDate
		;
	Mortgage TmpMtg;
	for(int i=0;i<tempInt;i++){
		stream >> TmpMtg;
	}
	flows.AddLoan(TmpMtg);
	return stream;
}
