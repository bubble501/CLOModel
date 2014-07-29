#include "CentralUnitOverlay.h"
#include "ManCentralUnit.h"
#include <QHash>
#include "ManBloombergVector.h"
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
void CentralUnitOverlay::CompileBaseRates(ManConstBaseRateTable^ Values) {
	ConstantBaseRateTable FillValues = *(Values->Unmanaged);
	CentralUnitAPI::CompileBaseRates(FillValues);
	Values = gcnew ManConstBaseRateTable(FillValues);
}
void CentralUnitOverlay::CompileBaseRates(ManForwBaseRateTable^ Values) {
	ForwardBaseRateTable FillValues = *(Values->Unmanaged);
	CentralUnitAPI::CompileBaseRates(FillValues);
	Values = gcnew ManForwBaseRateTable(FillValues);
}
#ifndef NO_DATABASE
void CentralUnitOverlay::GetBaseRatesDatabase(ManConstBaseRateTable^ Values, bool DownloadAll) {
	ConstantBaseRateTable FillValues = *(Values->Unmanaged);
	CentralUnitAPI::GetBaseRatesDatabase(FillValues, DownloadAll);
	Values = gcnew ManConstBaseRateTable(FillValues);
}
void CentralUnitOverlay::GetBaseRatesDatabase(ManForwBaseRateTable^ Values, bool DownloadAll) {
	ForwardBaseRateTable FillValues = *(Values->Unmanaged);
	CentralUnitAPI::GetBaseRatesDatabase(FillValues, DownloadAll);
	Values = gcnew ManForwBaseRateTable(FillValues);
}
#endif