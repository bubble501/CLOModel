#ifndef ExcelOutput_h__
#define ExcelOutput_h__

#include <QString>
#include <QColor>
#include "ExcelCommons.h"
class Tranche;
class MtgCashFlow;
class Waterfall;
class StressTest;
class QDate;
namespace ExcelOutput{
/*!
\brief Sends the information of the tranche cash flows to excel
\param source The tranche whose cash flows should be printed
\param DestinationAddress The address of the cell the cash flows should be printed to
\param BackgrndCol The background color of the cells that will contain the cash flows
\param PrintDates Determines if the dates of the cash flows should be printed
\param PrintOutstanding Determines if the tranche outstanding through time should be printed 
\param PrintInterest Determines if the interest flows should be printed 
\param PrintPrincipal Determines if the principal flows should be printed 
\param PrintTotalFlow Determines if the the total (interest+principal) flow should be printed
\param PrintDeferred Determines if the stock of cumulative deferred interests through time should be printed 
\param PrintOCtest Determines if the OC test results through time should be printed 
\param PrintICtest Determines if the IC test results through time should be printed 
\return S_OK if the output was successful or <a href=http://blogs.msdn.com/b/eldar/archive/2007/04/03/a-lot-of-hresult-codes.aspx target="_blank">an error code</a>
\details This functions sends the informations regarding a tranche cash flows to excel

It will call the VBA function PrintTranche passing it the following arguments:
-# A Variant containing a matrix with rows going from 0 to how many flows the tranche has and columns going from 1 to the number of boolean arguments that are true. Each matrix cell will contain the relevant value. The columns contents will follow the same order as the boolean arguments of this function
-# A String containing the address of the cell where the the tranche cash flows should be printed
-# A String containing the name of the tranche
-# A String containing the color background information in the format: RED,GREEN,BLUE in base 255
-# A Boolean equal to the PrintDates argument
-# A Boolean equal to the PrintOutstanding argument
-# A Boolean equal to the PrintInterest argument
-# A Boolean equal to the PrintPrincipal argument
-# A Boolean equal to the PrintTotalFlow argument
-# A Boolean equal to the PrintDeferred argument
-# A Boolean equal to the PrintOCtest argument
-# A Boolean equal to the PrintICtest argument
-# A Double representing the OC test limit for the tranche
-# A Double representing the IC test limit for the tranche
 */
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
		bool PrintICtest  = true,
		bool PrintPDL = false
	);
/*!
\brief Sends the information of the loans cash flows to excel to be charted
\param source The loans cash flows that should be printed
\param DestinationSheet The sheet name where the chart is
\param DestinationIndex The index of the chart object in the DestinationSheet sheet where the results should be plotted to
\param PrintDates Determines if the dates of the cash flows should be printed
\param PrintOutstanding Determines if the tranche outstanding through time should be printed 
\param PrintInterest Determines if the interest flows should be printed 
\param PrintScheduled Determines if the scheduled principal flows should be printed 
\param PrintPrepay Determines if the prepayments flows should be printed
\param PrintTotalPrincipal Determines if the total principal (scheduled+prepayments) flows should be printed 
\param PrintLoss Determines if the losses on principal through time should be printed
\return S_OK if the output was successful or <a href=http://blogs.msdn.com/b/eldar/archive/2007/04/03/a-lot-of-hresult-codes.aspx target="_blank">an error code</a>
\details This functions sends the informations regarding a tranche cash flows to excel to be plotted in a chart

It will call the VBA function PlotMortgagesFlows passing it the following arguments:
-# A String containing DestinationSheet
-# A Long containing DestinationIndex
-# A Variant containing the dates that will be used as X values in the chart
-# A Variant containing the total loans amount outstanding through time. It will be printed as an area chart
-# A Variant containing the loans interest cash flows. It will be printed as a bar chart
-# A Variant containing the loans scheduled principal cash flows. It will be printed as a bar chart
-# A Variant containing the loans prepayments cash flows. It will be printed as a bar chart
-# A Variant containing the loans total principal (scheduled+prepayments) cash flows. It will be printed as a bar chart
-# A Variant containing the loans loss on principal for the period through time. It will be printed as a bar chart

If a boolean parameter is set to false the relevant array will contain empty values
 */
	HRESULT PlotMortgagesFlows(
		const MtgCashFlow& source,
		const QString& DestinationSheet,
		int DestinationIndex,
		bool PrintDates= true, 
		bool PrintOutstanding= true, 
		bool PrintInterest= true, 
		bool PrintScheduled= true, 
		bool PrintPrepay= true, 
		bool PrintTotalPrincipal= false,
		bool PrintLoss= true
	);
/*!
\brief Sends the information of the loans cash flows to excel
\param source The loans cash flows that should be printed
\param DestinationAddress The address of the cell the cash flows should be printed to
\param PrintCounter Determines if a counter of the flows should be printed
\param PrintDates Determines if the dates of the cash flows should be printed
\param PrintOutstanding Determines if the tranche outstanding through time should be printed 
\param PrintInterest Determines if the interest flows should be printed 
\param PrintScheduled Determines if the scheduled principal flows should be printed 
\param PrintPrepay Determines if the prepayments flows should be printed
\param PrintAccruedInterest Determines if the stock of cumulative accrued interests through time should be printed 
\param PrintTotalPrincipal Determines if the total principal (scheduled+prepayments) flows should be printed 
\param PrintLoss Determines if the losses on principal through time should be printed
\param PrintLossOnInterest Determines if the losses on accrued interest through time should be printed 
\return S_OK if the output was successful or <a href=http://blogs.msdn.com/b/eldar/archive/2007/04/03/a-lot-of-hresult-codes.aspx target="_blank">an error code</a>
\details This functions sends the informations regarding a tranche cash flows to excel

It will call the VBA function PrintMortgagesRepLines passing it the following arguments:
-# A Variant containing a matrix with rows going from 0 to how many flows source has and columns going from 1 to the number of boolean arguments that are true. Each matrix cell will contain the relevant value. The columns contents will follow the same order as the boolean arguments of this function
-# A String containing the address of the cell where the the tranche cash flows should be printed
-# A Boolean equal to the PrintCounter argument
-# A Boolean equal to the PrintDates argument
-# A Boolean equal to the PrintOutstanding argument
-# A Boolean equal to the PrintInterest argument
-# A Boolean equal to the PrintScheduled argument
-# A Boolean equal to the PrintPrepay argument
-# A Boolean equal to the PrintAccruedInterest argument
-# A Boolean equal to the PrintTotalPrincipal argument
-# A Boolean equal to the PrintLoss argument
-# A Boolean equal to the PrintLossOnInterest argument
 */
	HRESULT PrintMortgagesRepLines(
		const MtgCashFlow& source,
		const QString& DestinationAddress,
		bool PrintCounter  = false, 
		bool PrintDates  = true, 
		bool PrintOutstanding  = true, 
		bool PrintInterest  = true, 
		bool PrintScheduled  = true, 
		bool PrintPrepay  = true, 
		bool PrintAccruedInterest  = true, 
		bool PrintTotalPrincipal  = false,
		bool PrintDefaults=true,
		bool PrintLoss  = true,
		bool PrintLossOnInterest  = true,
		bool PrintDelinquencies = true,
		bool PrintWAL = true
	);
	HRESULT PrintMergedCell(const QString& msg, const QString& TargetCell, int RowDim=1, int ColDim=1 ,const QColor& FillColor=Qt::white);
	HRESULT PrintColumn(const QString& Title, const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat="0.00",const QColor& FillColor=Qt::white);
	HRESULT PrintDataColumn(const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat="0.00");
	HRESULT PlotTranchesDynamic(const Waterfall& source,const QString& DestinationSheet,int DestinationIndex,const QDate& CallDate);
	HRESULT PlotOCTest(const Waterfall& source,const QString& DestinationSheet,int DestinationIndex,const QDate& CallDate);
	HRESULT PlotICTest(const Waterfall& source,const QString& DestinationSheet,int DestinationIndex,const QDate& CallDate);
	HRESULT PlotAnnualExcess(const Waterfall& source,const QString& DestinationSheet,int DestinationIndex,const QDate& CallDate);
	HRESULT PlotCostFunding(const Waterfall& source,const QString& DestinationSheet,int DestinationIndex,const QDate& CallDate);
	HRESULT PlotCallToEquity(const Waterfall& source,const QString& DestinationSheet,int DestinationIndex,const QDate& CallDate);
	HRESULT PlotCPRLS(const Waterfall& source,const QString& DestinationSheet,int DestinationIndex);
	HRESULT PlotEquityReturn(const Waterfall& source,const QString& DestinationSheet,int DestinationIndex);
	HRESULT PrintWaterfallStep(const QString& DestinationSheet, const QString& DestinationAddress, const QString& StepStructure);
}
#endif // ExcelOutput_h__


/*!
\file ExcelOutput.h
\author Luca Beldi
\date November 2013
\brief Excel Output Functions
\details File Containing the definition of the function that take care of outputting to excel
*/
/*!
\namespace ExcelOutput
\brief Namespace containing excel output functions
\author Luca Beldi
\date November 2013
*/
