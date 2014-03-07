#include "ManUtilities.h"
List<double>^ ABSUtilities::UnpackBloombergVect(String^ Vect, int PaymFreq, bool AdjustFreq){
	QList<double> NativeRes=UnpackVect(String2QString(Vect),PaymFreq,AdjustFreq);
	List<double>^ Result=gcnew List<double>(NativeRes.size());
	for(int i=0;i<NativeRes.size();i++)Result[i]=NativeRes.at(i);
	return Result;
}
double ABSUtilities::IRR(List<DateTime>^ Dte, List<double>^ Flws, int Daycount, double Guess,int precision){
	QList<QDate> NativeDate;
	for each(const DateTime% a in Dte) NativeDate.append(DateTime2QDate(a));
	QList<double> NativeFlws;
	for each(double a in Flws) NativeFlws.append(a);
	return CalculateIRR(NativeDate,NativeFlws,Daycount,Guess,precision);
}
double ABSUtilities::NPV(List<DateTime>^ Dte, List<double>^ Flws, double Interest, int Daycount){
	QList<QDate> NativeDate;
	for each(const DateTime% a in Dte) NativeDate.append(DateTime2QDate(a));
	QList<double> NativeFlws;
	for each(double a in Flws) NativeFlws.append(a);
	return CalculateNPV(NativeDate,NativeFlws,Interest,Daycount);
}
double ABSUtilities::DiscountMargin(List<DateTime>^ Dte, List<double>^ Flws, double BaseRate,int Daycount, double Guess,int precision){
	QList<QDate> NativeDate;
	for each(const DateTime% a in Dte) NativeDate.append(DateTime2QDate(a));
	QList<double> NativeFlws;
	for each(double a in Flws) NativeFlws.append(a);
	return CalculateDM(NativeDate,NativeFlws,BaseRate,Daycount,Guess,precision);
}