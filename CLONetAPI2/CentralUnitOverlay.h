#ifndef CentralUnitOverlay_h__
#define CentralUnitOverlay_h__
#include "CentralUnitAPI.h"
#include "ManBaseRateTable.h"
#include <vcclr.h>
using namespace System;
namespace ManagedCLO {
	ref class ManCentralUnit;
	class CentralUnitOverlay : public CentralUnitAPI
	{
		Q_OBJECT
	public:
		CentralUnitOverlay(ManCentralUnit^ ManVer,QObject* parent=0);
		void CompileBaseRates(ManConstBaseRateTable^ Values);
		void CompileBaseRates(ManForwBaseRateTable^ Values);
#ifndef NO_DATABASE
		void GetBaseRatesDatabase(ManConstBaseRateTable^ Values, bool DownloadAll);
		void GetBaseRatesDatabase(ManForwBaseRateTable^ Values, bool DownloadAll);
#endif
	private:
		gcroot<ManCentralUnit^> m_ManagedVersion;
	private slots:
		void ProgressUpdateCallBack(double prog);
		void CalculationFinishedCallBack();
	};
}
#endif // CentralUnitOverlay_h__