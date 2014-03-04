#include "WaterfallCalculator.h"
#include "WaterfallCalcThread.h"
WaterfallCalculator::WaterfallCalculator(QObject* parent/* =0 */)
	:QObject(parent)
	,BeesReturned(0)
	,BeesSent(0)
	,SequentialComputation(false)
{}
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
		WaterfallCalcThread* Bee=new WaterfallCalcThread(BeesSent,this);
		Bee->SetWaterfall(*(Cascades.at(BeesSent)));
		connect(Bee,SIGNAL(Calculated(int,const Waterfall&)),this,SLOT(BeeReturned(int,const Waterfall&)));
		connect(Bee,SIGNAL(Calculated(int,const Waterfall&)),Bee,SLOT(stop()),Qt::QueuedConnection);
		Bee->start();
	}
}
void WaterfallCalculator::BeeReturned(int ID,const Waterfall& a){
	(Cascades[ID])->operator=(a);
	BeesReturned++;
	if(BeesReturned==Cascades.size()){
		emit Calculated();
		return;
	}
	if(BeesSent>=Cascades.size()) return;
	WaterfallCalcThread* Bee=new WaterfallCalcThread(BeesSent,this);
	Bee->SetWaterfall(*(Cascades.at(BeesSent)));
	connect(Bee,SIGNAL(Calculated(int,const Waterfall&)),this,SLOT(BeeReturned(int,const Waterfall&)));
	connect(Bee,SIGNAL(Calculated(int,const Waterfall&)),Bee,SLOT(stop()),Qt::QueuedConnection);
	Bee->start();
	BeesSent++;
}