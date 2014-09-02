#include "MtgCalculator.h"
#include "MtgCalculatorThread.h"
#include "Mortgage.h"
MtgCalculator::MtgCalculator(QObject* parent)
	:QObject(parent)
	,SequentialComputation(false)
{

}
MtgCalculator::~MtgCalculator() { 
	MtgCalculatorThread* CurrentRunning;
	for (QHash<int, MtgCalculatorThread*>::iterator j = ThreadPool.begin(); j != ThreadPool.end(); j++) {
		CurrentRunning = *j;
		if (CurrentRunning) {
			if (CurrentRunning->isRunning()) {
				CurrentRunning->exit();
				CurrentRunning->wait();
			}
		}
	}
	Reset(); 
}
void MtgCalculator::AddLoan(const Mortgage& a){
	Loans.append(new Mortgage(a));
}
bool MtgCalculator::StartCalculation(){
	BeesReturned=0;
	{//Check if all base rates are valid
		bool CheckAgain = false;
		ConstantBaseRateTable TempTable;
		for (QList<Mortgage*>::const_iterator i = Loans.constBegin(); i != Loans.constEnd(); ++i) {
			if ((*i)->GetFloatingRateValue().IsEmpty()) {
				(*i)->CompileReferenceRateValue(TempTable);
				CheckAgain = true;
			}
		}
		for (QList<Mortgage*>::const_iterator i = Loans.constBegin(); CheckAgain && i != Loans.constEnd(); ++i) {
			if ((*i)->GetFloatingRateValue().IsEmpty()) return false;
		}
	}
	int NumberOfThreads=QThread::idealThreadCount();
	if(SequentialComputation || NumberOfThreads<1) NumberOfThreads=1;
	for(BeesSent=0;BeesSent<Loans.size() && BeesSent<NumberOfThreads;BeesSent++){
		ThreadPool[BeesSent] = new MtgCalculatorThread(BeesSent, this);
		ThreadPool[BeesSent]->SetLoan(*(Loans.at(BeesSent)));
		ThreadPool[BeesSent]->SetCPR(m_CPRass);
		ThreadPool[BeesSent]->SetCDR(m_CDRass);
		ThreadPool[BeesSent]->SetLS(m_LSass);
		ThreadPool[BeesSent]->SetRecoveryLag(m_RecoveryLag);
		ThreadPool[BeesSent]->SetDelinquency(m_Delinquency);
		ThreadPool[BeesSent]->SetDelinquencyLag(m_DelinquencyLag);
		ThreadPool[BeesSent]->SetStartDate(StartDate);
		connect(ThreadPool[BeesSent], SIGNAL(Calculated(int, const MtgCashFlow&)), this, SLOT(BeeReturned(int, const MtgCashFlow&)));
		connect(ThreadPool[BeesSent], SIGNAL(Calculated(int, const MtgCashFlow&)), ThreadPool[BeesSent], SLOT(stop()), Qt::QueuedConnection);
		ThreadPool[BeesSent]->start();
	}
	return true;
}
void MtgCalculator::BeeReturned(int Ident,const MtgCashFlow& a){
	Result+=a;
	BeesReturned++;
	ThreadPool.erase(ThreadPool.find(Ident));
	emit BeeCalculated(BeesReturned);
	if(BeesReturned==Loans.size()){
		emit Calculated();
		return;
	}
	if(BeesSent>=Loans.size()) return;
	ThreadPool[BeesSent] = new MtgCalculatorThread(BeesSent,this);
	ThreadPool[BeesSent]->SetLoan(*(Loans.at(BeesSent)));
	ThreadPool[BeesSent]->SetCPR(m_CPRass);
	ThreadPool[BeesSent]->SetCDR(m_CDRass);
	ThreadPool[BeesSent]->SetLS(m_LSass);
	ThreadPool[BeesSent]->SetRecoveryLag(m_RecoveryLag);
	ThreadPool[BeesSent]->SetDelinquency(m_Delinquency);
	ThreadPool[BeesSent]->SetDelinquencyLag(m_DelinquencyLag);
	ThreadPool[BeesSent]->SetStartDate(StartDate);
	connect(ThreadPool[BeesSent], SIGNAL(Calculated(int, const MtgCashFlow&)), this, SLOT(BeeReturned(int, const MtgCashFlow&)));
	connect(ThreadPool[BeesSent], SIGNAL(Calculated(int, const MtgCashFlow&)), ThreadPool[BeesSent], SLOT(stop()), Qt::QueuedConnection);
	ThreadPool[BeesSent]->start();
	BeesSent++;
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
	if (BloombergVector(m_CPRass).IsEmpty(0.0,1.0)) Result += "CPR Vector: " + m_CPRass + '\n';
	if (BloombergVector(m_CDRass).IsEmpty(0.0, 1.0)) Result += "CDR Vector: " + m_CDRass + '\n';
	if (BloombergVector(m_LSass).IsEmpty()) Result += "LS Vector: " + m_LSass + '\n';
	if (IntegerVector(m_RecoveryLag).IsEmpty()) Result += "Recovery Lag Vector: " + m_RecoveryLag + '\n';
	if (BloombergVector(m_Delinquency).IsEmpty(0, 1.0)) Result += "Delinquency Vector Vector: " + m_Delinquency + '\n';
	if (IntegerVector(m_DelinquencyLag).IsEmpty()) Result += "Delinquency Lag Vector: " + m_DelinquencyLag + '\n';
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
	return Result;
}

QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows){
	stream 
		<< static_cast<qint32>(flows.Loans.size())
		<< flows.m_CPRass
		<< flows.m_CDRass
		<< flows.m_LSass
		<< flows.m_RecoveryLag
		<< flows.m_Delinquency
		<< flows.m_DelinquencyLag
		<< flows.StartDate
	 ;
	for(QList<Mortgage*>::const_iterator i=flows.Loans.begin();i!=flows.Loans.end();i++){
		stream << **i;
	}
	return stream;
}
QDataStream& MtgCalculator::LoadOldVersion(QDataStream& stream) {
	qint32 tempInt;
	stream >> tempInt;
	stream >> m_CPRass;
	stream >> m_CDRass;
	stream >> m_LSass;
	stream >> m_RecoveryLag;
	stream >> m_Delinquency;
	stream >> m_DelinquencyLag;
	stream >> StartDate;
	Mortgage TmpMtg;
	for (int i = 0; i < tempInt; i++) {
		TmpMtg.SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> TmpMtg;
		AddLoan(TmpMtg);
	}
	ResetProtocolVersion();
	return stream;
}

void MtgCalculator::CompileReferenceRateValue(ForwardBaseRateTable& Values)const {
	for (QList<Mortgage*>::const_iterator i = Loans.begin(); i != Loans.end(); i++)
		(*i)->CompileReferenceRateValue(Values);
}
void MtgCalculator::CompileReferenceRateValue(ConstantBaseRateTable& Values)const {
	for (QList<Mortgage*>::const_iterator i = Loans.begin(); i != Loans.end(); i++)
		(*i)->CompileReferenceRateValue(Values);
}
#ifndef NO_DATABASE
void MtgCalculator::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll)const {
	for (QList<Mortgage*>::const_iterator i = Loans.begin(); i != Loans.end(); i++)
		(*i)->GetBaseRatesDatabase(Values, DownloadAll);
}
void MtgCalculator::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll)const {
	for (QList<Mortgage*>::const_iterator i = Loans.begin(); i != Loans.end(); i++)
		(*i)->GetBaseRatesDatabase(Values, DownloadAll);
}
#endif
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows) {
	return flows.LoadOldVersion(stream);
}
