#include "StressTestOverlay.h"
#include "ManStressTest.h"
using namespace ManagedCLO;
StressTestOverlay::StressTestOverlay(ManStressTest^ ManVer,QObject* parent)
	:StressTest(parent)
	,m_ManagedVersion(ManVer)
{
	connect(this,SIGNAL(AllFinished()),this,SLOT(AllFinishCallback()));
	connect(this,SIGNAL(ProgressStatus(double)),this,SLOT(ProgressStatusCallback(double)));
}
void StressTestOverlay::AllFinishCallback(){m_ManagedVersion->AllFinishCallback();}
void StressTestOverlay::ProgressStatusCallback(double prog){m_ManagedVersion->ProgressStatusCallback(prog);}