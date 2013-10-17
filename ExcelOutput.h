#ifndef ExcelOutput_h__
#define ExcelOutput_h__

#include <QString>
#include <QColor>
#include "ExcelCommons.h"
class Tranche;
class MtgCashFlow;
class StressTest;
namespace ExcelOutput{
	
	HRESULT PrintTrancheFlow(
		const Tranche& source,
		const QString& DestinationAddress,
		const QColor& BackgrndCol,
		bool PrintDates  = true, 
		bool PrintOutstanding  = true, 
		bool PrintInterest  = true, 
		bool PrintPrincipal  = true, 
		bool PrintTotalFlow  = false, 
		bool PrintDeferred  = true, 
		bool PrintOCtest  = true, 
		bool PrintICtest  = true
	);
	HRESULT PrintMortgagesRepLines(
		const MtgCashFlow& source,
		const QString& DestinationAddress,
		bool PrintCounter  = false, 
		bool PrintDates  = true, 
		bool PrintOutstanding  = true, 
		bool PrintInterest  = true, 
		bool PrintScheduled  = false, 
		bool PrintPrepay  = true, 
		bool PrintAccruedInterest  = true, 
		bool PrintTotalPrincipal  = false,
		bool PrintLoss  = true,
		bool PrintLossOnInterest  = true
	);
	HRESULT PrintStressTest(
		const StressTest& stresser,
		const QString& TrancheTarget,
		const QString& DestinationAddress,
		bool SetupConditionalFormatting  = false
	);
	HRESULT PrintMergedCell(const QString& msg, const QString& TargetCell, int RowDim=1, int ColDim=1 ,const QColor& FillColor=Qt::white);
	HRESULT PrintColumn(const QString& Title, const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat="0.00",const QColor& FillColor=Qt::white);
}
#endif // ExcelOutput_h__