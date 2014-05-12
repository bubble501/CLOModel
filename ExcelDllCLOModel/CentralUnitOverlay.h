#ifndef CentralUnitOverlay_h__
#define CentralUnitOverlay_h__
#include "CentralUnitAPI.h"
#include <vcclr.h>
using namespace System;
namespace ManagedCLO {
	ref class ManCentralUnit;
	class CentralUnitOverlay : public CentralUnitAPI
	{
		Q_OBJECT
	public:
		CentralUnitOverlay(ManCentralUnit^ ManVer,QObject* parent=0);
		void CompileBaseRates(Collections::Generic::Dictionary<String^,double>^ Values);
	private:
		gcroot<ManCentralUnit^> m_ManagedVersion;
	private slots:
		void ProgressUpdateCallBack(double prog);
		void CalculationFinishedCallBack();
	};
}
#endif // CentralUnitOverlay_h__