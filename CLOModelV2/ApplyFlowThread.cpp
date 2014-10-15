#include "ApplyFlowThread.h"
ApplyFlowThread::ApplyFlowThread(int ID, QObject* parent)
	:QThread(parent)
	, Identifier(ID) {
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}
void ApplyFlowThread::run() {
	emit Calculated(Identifier, BaseFlow->ApplyScenario(
		AssumptionsToApply.GetCPRscenario()
		, AssumptionsToApply.GetCDRscenario()
		, AssumptionsToApply.GetLSscenario()
	));
	exec();
}