#include "WaterfallCalculator.h"
#include "WaterfallCalcThread.h"
WaterfallCalculator::WaterfallCalculator(QObject* parent/* =0 */)
	:QObject(parent)
	,BeesReturned(0)
	,BeesSent(0)
	,SequentialComputation(false)
{}
WaterfallCalculator::~WaterfallCalculator() { 
	WaterfallCalcThread* CurrentRunning;
	for (QHash<int, WaterfallCalcThread*>::iterator j = ThreadPool.begin(); j != ThreadPool.end(); j++) {
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
	for (QList<Waterfall*>::iterator i=Cascades.begin();i!=Cascades.end();i++){
		delete *i;
	}
	Cascades.clear();
}
void WaterfallCalculator::AddWaterfall(const Waterfall& a){
	Cascades.append(new Waterfall(a));
}
void WaterfallCalculator::StartCalculation(){
	BeesReturned=0;
	int NumberOfThreads=QThread::idealThreadCount();
	if(SequentialComputation || NumberOfThreads<1) NumberOfThreads=1;
	for(BeesSent=0;BeesSent<Cascades.size() && BeesSent<NumberOfThreads;BeesSent++){
		ThreadPool[BeesSent]= new WaterfallCalcThread(BeesSent, this);
		ThreadPool[BeesSent]->SetWaterfall(*(Cascades.at(BeesSent)));
		connect(ThreadPool[BeesSent], SIGNAL(Calculated(int, const Waterfall&)), this, SLOT(BeeReturned(int, const Waterfall&)));
		connect(ThreadPool[BeesSent], SIGNAL(Calculated(int, const Waterfall&)), ThreadPool[BeesSent], SLOT(stop()), Qt::QueuedConnection);
		ThreadPool[BeesSent]->start();
	}
}
void WaterfallCalculator::BeeReturned(int ID,const Waterfall& a){
	(Cascades[ID])->operator=(a);
	ThreadPool.erase(ThreadPool.find(ID));
	BeesReturned++;
	if(BeesReturned==Cascades.size()){
		emit Calculated();
		return;
	}
	if(BeesSent>=Cascades.size()) return;
	ThreadPool[BeesSent] = new WaterfallCalcThread(BeesSent, this);
	ThreadPool[BeesSent]->SetWaterfall(*(Cascades.at(BeesSent)));
	connect(ThreadPool[BeesSent], SIGNAL(Calculated(int, const Waterfall&)), this, SLOT(BeeReturned(int, const Waterfall&)));
	connect(ThreadPool[BeesSent], SIGNAL(Calculated(int, const Waterfall&)), ThreadPool[BeesSent], SLOT(stop()), Qt::QueuedConnection);
	ThreadPool[BeesSent]->start();
	BeesSent++;
}