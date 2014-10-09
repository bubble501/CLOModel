#include "WaterfallCalculator.h"
#include "WaterfallCalcThread.h"
WaterfallCalculator::WaterfallCalculator(QObject* parent)
	:QObject(parent)
	,BeesReturned(0)
	,SequentialComputation(false)
	, KeepRunning(false)
{}
WaterfallCalculator::~WaterfallCalculator() { 
	WaterfallCalcThread* CurrentRunning;
	for (auto j = ThreadPool.begin(); j != ThreadPool.end(); j++) {
		CurrentRunning = *j;
		if (CurrentRunning) {
			if (CurrentRunning->isRunning()) {
				CurrentRunning->exit();
				CurrentRunning->wait();
			}
		}
	}
	ResetWaterfalls(); 
}
void WaterfallCalculator::ResetWaterfalls(){
	for (auto i=Cascades.begin();i!=Cascades.end();++i){
		delete *i;
	}
	Cascades.clear();
}
void WaterfallCalculator::AddWaterfall(const Waterfall& a, qint32 ID) {
	if (Cascades.contains(ID)) {
		delete Cascades[ID];
	}
	Cascades[ID]=new Waterfall(a);
}
void WaterfallCalculator::StartCalculation(){
	if (KeepRunning) return;
	KeepRunning = true;
	SentIDs.clear();
	int BeesSent = 0;
	BeesReturned = 0;
	int NumberOfThreads=QThread::idealThreadCount();
	if(SequentialComputation || NumberOfThreads<1) NumberOfThreads=1;
	for (auto i = Cascades.constBegin(); i != Cascades.constEnd() && BeesSent<NumberOfThreads; ++i ,++BeesSent) {
		ThreadPool[i.key()] = new WaterfallCalcThread(i.key(), this);
		ThreadPool[i.key()]->SetWaterfall(*(i.value()));
		connect(ThreadPool[i.key()], SIGNAL(Calculated(int, const Waterfall&)), this, SLOT(BeeReturned(int, const Waterfall&)));
		connect(ThreadPool[i.key()], SIGNAL(ErrorInCalculation(int)), this, SLOT(HandleErrorCalc(int)));
		connect(ThreadPool[i.key()], SIGNAL(ErrorInCalculation(int)), this, SIGNAL(ErrorInCalculation(int)));
		connect(ThreadPool[i.key()], SIGNAL(Calculated(int, const Waterfall&)), ThreadPool[i.key()], SLOT(stop()), Qt::QueuedConnection);
		SentIDs.insert(i.key());
		ThreadPool[i.key()]->start();
	}
}
void WaterfallCalculator::BeeReturned(int ID,const Waterfall& a){
	if (!KeepRunning) return;
	(Cascades[ID])->operator=(a);
	{
		QHash<qint32, WaterfallCalcThread*>::iterator CurrentThread = ThreadPool.find(ID);
		if (CurrentThread != ThreadPool.end()) {
			ThreadPool.erase(CurrentThread);
		}
	}
	emit CurrentProgress(100.0*static_cast<double>(++BeesReturned) / static_cast<double>(Cascades.size()));
	if (BeesReturned == Cascades.size()) {
		BeesReturned = 0; SentIDs.clear();
		emit Calculated();
		KeepRunning = false;
		return;
	}
	for (auto i = Cascades.constBegin(); i != Cascades.constEnd(); ++i) {
		if (SentIDs.contains(i.key())) continue;
		ThreadPool[i.key()] = new WaterfallCalcThread(i.key(), this);
		ThreadPool[i.key()]->SetWaterfall(*(i.value()));
		connect(ThreadPool[i.key()], SIGNAL(Calculated(int, const Waterfall&)), this, SLOT(BeeReturned(int, const Waterfall&)));
		connect(ThreadPool[i.key()], SIGNAL(ErrorInCalculation(int)), this, SLOT(HandleErrorCalc(int)));
		connect(ThreadPool[i.key()], SIGNAL(ErrorInCalculation(int)), this, SIGNAL(ErrorInCalculation(int)));
		connect(ThreadPool[i.key()], SIGNAL(Calculated(int, const Waterfall&)), ThreadPool[i.key()], SLOT(stop()), Qt::QueuedConnection);
		SentIDs.insert(i.key());
		ThreadPool[i.key()]->start();
		break;
	}
}

void WaterfallCalculator::HandleErrorCalc(int ID) {
	BeeReturned(ID, Waterfall());
}

void WaterfallCalculator::Stop() {
	KeepRunning = false;
	SentIDs.clear(); 
	BeesReturned = 0;
	for (auto i = ThreadPool.begin(); i != ThreadPool.end(); ++i) {
		disconnect(i.value());
	}
	ResetWaterfalls();
	for (auto i = ThreadPool.begin(); i != ThreadPool.end(); ++i) {
		i.value()->exit();
		i.value()->wait();
	}
}
