#ifndef StressTestOverlay_h__
#define StressTestOverlay_h__
#include "StressTest.h"
#include <vcclr.h>
using namespace System;
namespace ManagedCLO {
	ref class ManStressTest;
	class StressTestOverlay : public StressTest
	{
		Q_OBJECT
	public:
		StressTestOverlay(ManStressTest^ ManVer,QObject* parent=0);
	private:
		gcroot<ManStressTest^> m_ManagedVersion;
	private slots:
		void AllFinishCallback();
		void ProgressStatusCallback(double prog);
	};
}
#endif // StressTestOverlay_h__

