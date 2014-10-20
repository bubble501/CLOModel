#include "ApplyFlowThread.h"
ApplyFlowThread::ApplyFlowThread(int ID, QObject* parent)
	:TemplAsyncThread<MtgCashFlow>(ID,parent)
{}
void ApplyFlowThread::run() {
	m_Result= BaseFlow->ApplyScenario(
		AssumptionsToApply.GetCPRscenario()
		, AssumptionsToApply.GetCDRscenario()
		, AssumptionsToApply.GetLSscenario()
	);
	emit AnonimCalculated(Identifier);
	exec();
}