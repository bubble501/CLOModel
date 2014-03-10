#include "ManUtilities.h"
#include <QList>
#ifdef _DEBUG
double ABSUtilities::IRRold(List<DateTime>^ Dte, List<double>^ Flws, int Daycount, double Guess,int precision){
	QList<QDate> NativeDate;
	for each(const DateTime% a in Dte) NativeDate.append(DateTime2QDate(a));
	QList<double> NativeFlws;
	for each(double a in Flws) NativeFlws.append(a);
	return CalculateIRR(NativeDate,NativeFlws,Daycount,Guess,precision);
}

double ABSUtilities::NPVold(List<DateTime>^ Dte, List<double>^ Flws, double Interest, int Daycount){
	QList<QDate> NativeDate;
	for each(const DateTime% a in Dte) NativeDate.append(DateTime2QDate(a));
	QList<double> NativeFlws;
	for each(double a in Flws) NativeFlws.append(a);
	return CalculateNPV(NativeDate,NativeFlws,Interest,Daycount);
}
double ABSUtilities::DiscountMarginold(List<DateTime>^ Dte, List<double>^ Flws, double BaseRate,int Daycount, double Guess,int precision){
	QList<QDate> NativeDate;
	for each(const DateTime% a in Dte) NativeDate.append(DateTime2QDate(a));
	QList<double> NativeFlws;
	for each(double a in Flws) NativeFlws.append(a);
	return CalculateDM(NativeDate,NativeFlws,BaseRate,Daycount,Guess,precision);
}
#endif


double ABSUtilities::NPV(List<DateTime>^ Dte, List<double>^ Flws, double Interest, int Daycount){
	if(Dte->Count!=Flws->Count) throw gcnew System::Exception("The number of dates is not equal to the number of flows");
	double Result=Flws[0];
	for(int i=1;i<Dte->Count;i++){
		Result+=Flws[i]/System::Math::Pow(1.0+Interest,(Dte[i].ToOADate()-Dte[0].ToOADate())/static_cast<double>(Daycount));
	}
	return Result;
}
double ABSUtilities::IRR(List<DateTime>^ Dte, List<double>^ Flws, int Daycount, double Guess,int precision){
	if(Guess<=0 || Guess>10) Guess=0.05;
	unsigned int CurrentIterations=0;
	double Result=Guess;
	double PreviuousGuess=Guess+0.01;
	double CurrentGuess=Guess;
	double PreviousNPV=NPV(Dte,Flws,PreviuousGuess,Daycount);
	double CurrentNPV=NPV(Dte,Flws,Result,Daycount);
	while(System::Math::Abs(CurrentNPV)>System::Math::Pow(10.0,-static_cast<double>(precision))){
		Result-=CurrentNPV*(Result-PreviuousGuess)/(CurrentNPV-PreviousNPV);
		PreviousNPV=CurrentNPV;
		PreviuousGuess=CurrentGuess;
		CurrentGuess=Result;
		CurrentNPV=NPV(Dte,Flws,Result,Daycount);
		if(++CurrentIterations>=MaximumIRRIterations) return 0.0;
	}
	return Result;
}
double ABSUtilities::DiscountMargin(List<DateTime>^ Dte, List<double>^ Flws, double BaseRate,int Daycount, double Guess,int precision){
	double Yld = IRR(Dte,Flws,Daycount,Guess,precision);
	double Freq=1.0;
	if(Dte->Count>2){
		Freq=1.0/((Dte[Dte->Count -1].ToOADate()-Dte[1].ToOADate())/(static_cast<double>(Dte->Count)-2.0)/365.0);
		if(Freq-static_cast<double>(static_cast<int>(Freq))>=0.5) Freq=System::Math::Ceiling(Freq);
		else Freq=static_cast<double>(static_cast<int>(Freq));
	}
	double AdjYeld= (System::Math::Pow(1.0+Yld,1.0/Freq)-1.0)*Freq;
	return (AdjYeld-BaseRate)*10000.0;
}