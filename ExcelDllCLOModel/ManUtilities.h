#ifndef Utilities_h__
#define Utilities_h__
#include "Mortgage.h"
#include "CommonFunctions.h"
#include "ManagedCommons.h"
#include "ManBloombergVector.h"
using namespace ManagedCLO;
using namespace System::Collections::Generic;
/*!
\brief Static utilities functions
\details This class contains a number of static utilities functions developed for the CLO model but that can be of general usage
*/
ref class ABSUtilities
{
public:
	/*!
	\brief Calculates the payment for a loan based on constant payments and a constant interest rate.
	\arg Inter The interest rate for the loan in scalar form (5%=0.05)
	\arg Periods The total number of payments for the loan
	\arg PresentValue Is the present value, or the total amount that a series of future payments is worth now; also known as the principal.
	\warning Inter must be referring to the same time interval between two payments.<br/>To convert an annual rate \f$ r \f$ for a loan that pays interest \f$ p \f$ times a year apply the formula Inter\f$= \left((1+r)^\frac{p}{12} \right)-1 \f$
	*/
	static double pmt(double Inter, int Periods, double PresentValue){return Mortgage::pmt(Inter,Periods,PresentValue);}

	/*!
	\brief Formats a number with commas to separate thousands and precision deciaml places
	*/
	static String^ FormatNumber(double num,unsigned int precision){return QString2String(Commarize(num,precision));}
	/*!
	\brief Calculates the IRR of a series of cash flows
	\arg Dte The list of dates at which there is a flow
	\arg Flws The list of cash flows
	\arg Daycount The daycount convention, put 360 for Actual/360
	\arg guess The starting guess for the IRR (usually 0.05)
	\arg precision The order of magnitude of the largest error. 0 will make it precise to the unity, 2 will make it precise to the cent (the higher the precision the slower the function). Usually 3.
	\details This function calculates the internal rate of return of a series of cash flows happening on specific dates.<br/>When Flws contains more than negative number then there can be multiple roots and changing Guess may change the result of the function.
	*/
	static double IRR(List<DateTime>^ Dte, List<double>^ Flws, int Daycount, double Guess,int precision);
	/*!
	\brief Calculates the NPV of a series of cash flows
	\arg Dte The list of dates at which there is a flow
	\arg Flws The list of cash flows
	\arg Interest The annualized discount rate
	\arg Daycount The daycount convention, put 360 for Actual/360
	\details This function calculates the net present value of a series of cash flows happening on specific dates discounted using a specified annualized interest rate.
	\sa NPV(List<DateTime>^,List<double>^,ManBloombergVector^,int)
	*/
	static double NPV(List<DateTime>^ Dte, List<double>^ Flws, double Interest, int Daycount);
	/*!
	\brief Calculates the NPV of a series of cash flows
	\arg Dte The list of dates at which there is a flow
	\arg Flws The list of cash flows
	\arg Interest The vector representing the value of the discount rate in time
	\arg Daycount The daycount convention, put 360 for Actual/360
	\details This function calculates the net present value of a series of cash flows happening on specific dates discounted using a specified annualized interest rate.
	\sa NPV(List<DateTime>^,List<double>^,double,int)
	*/
	static double NPV(List<DateTime>^ Dte, List<double>^ Flws, ManBloombergVector^ Interest, int Daycount);
	/*!
	\brief Calculates the NPV of a series of cash flows
	\arg Dte The list of dates at which there is a flow
	\arg Flws The list of cash flows
	\arg Interest The string representation of the vector describing the value of the discount rate in time
	\arg Daycount The daycount convention, put 360 for Actual/360
	\details This function calculates the net present value of a series of cash flows happening on specific dates discounted using a specified annualized interest rate.
	\sa NPV(List<DateTime>^,List<double>^,double,int)
	*/
	static double NPV(List<DateTime>^ Dte, List<double>^ Flws, String^ Interest, int Daycount){return NPV(Dte,Flws,gcnew ManBloombergVector(Interest),Daycount);}
	/*!
	\brief Calculates the DM of a series of cash flows
	\arg Dte The list of dates at which there is a flow
	\arg Flws The list of cash flows
	\arg BaseRate The base rate over which to calculate the margin
	\arg Daycount The daycount convention, put 360 for Actual/360
	\arg guess The starting guess for the IRR (usually 0.05)
	\arg precision The order of magnitude of the largest error. 0 will make it precise to the unity, 2 will make it precise to the cent (the higher the precision the slower the function). Usually 3.
	\details This function calculates the discount margin over a base rate of a series of cash flows happening on specific dates.<br/>When Flws contains more than negative number then there can be multiple roots and changing Guess may change the result of the function.
	\sa DiscountMargin(List<DateTime>^,List<double>^,ManBloombergVector^,int,double,int)
	*/
	static double DiscountMargin(List<DateTime>^ Dte, List<double>^ Flws, double BaseRate,int Daycount, double Guess,int precision);
	/*!
	\brief Calculates the DM of a series of cash flows
	\arg Dte The list of dates at which there is a flow
	\arg Flws The list of cash flows
	\arg BaseRate The vector representing the base rate over which to calculate the margin
	\arg Daycount The daycount convention, put 360 for Actual/360
	\arg guess The starting guess for the IRR (usually 0.05)
	\arg precision The order of magnitude of the largest error. 0 will make it precise to the unity, 2 will make it precise to the cent (the higher the precision the slower the function). Usually 3.
	\details This function calculates the discount margin over a base rate of a series of cash flows happening on specific dates.<br/>When Flws contains more than negative number then there can be multiple roots and changing Guess may change the result of the function.
	\sa DiscountMargin(List<DateTime>^,List<double>^,double,int,double,int)
	*/
	static double DiscountMargin(List<DateTime>^ Dte, List<double>^ Flws, ManBloombergVector^ BaseRate,int Daycount, double Guess,int precision);
	/*!
	\brief Calculates the DM of a series of cash flows
	\arg Dte The list of dates at which there is a flow
	\arg Flws The list of cash flows
	\arg BaseRate The string representation of the vector describing the base rate over which to calculate the margin
	\arg Daycount The daycount convention, put 360 for Actual/360
	\arg guess The starting guess for the IRR (usually 0.05)
	\arg precision The order of magnitude of the largest error. 0 will make it precise to the unity, 2 will make it precise to the cent (the higher the precision the slower the function). Usually 3.
	\details This function calculates the discount margin over a base rate of a series of cash flows happening on specific dates.<br/>When Flws contains more than negative number then there can be multiple roots and changing Guess may change the result of the function.
	\sa DiscountMargin(List<DateTime>^,List<double>^,double,int,double,int)
	*/
	static double DiscountMargin(List<DateTime>^ Dte, List<double>^ Flws, String^ BaseRate,int Daycount, double Guess,int precision){return DiscountMargin(Dte,Flws,gcnew ManBloombergVector(BaseRate),Daycount,Guess,precision);}
#ifdef _DEBUG
	static double NPVold(List<DateTime>^ Dte, List<double>^ Flws, double Interest, int Daycount);
	static double IRRold(List<DateTime>^ Dte, List<double>^ Flws, int Daycount, double Guess,int precision);
	static double DiscountMarginold(List<DateTime>^ Dte, List<double>^ Flws, double BaseRate,int Daycount, double Guess,int precision);
#endif
};
#endif // Utilities_h__

