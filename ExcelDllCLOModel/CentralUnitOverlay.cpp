#include "CentralUnitOverlay.h"
#include "ManCentralUnit.h"
using namespace ManagedCLO;
CentralUnitOverlay::CentralUnitOverlay(ManCentralUnit^ ManVer,QObject* parent)
	:CentralUnitAPI(parent)
	,m_ManagedVersion(ManVer)
{
	connect(this,SIGNAL(CalculationFinished()),this,SLOT(CalculationFinishedCallBack()));
	connect(this,SIGNAL(ProgressUpdate(double)),this,SLOT(ProgressUpdateCallBack(double)));
}
void CentralUnitOverlay::CalculationFinishedCallBack(){
	m_ManagedVersion->CalculationFinishedCallBack();
}
void CentralUnitOverlay::ProgressUpdateCallBack(double prog){
	m_ManagedVersion->ProgressUpdateCallBack(prog);
}