#include "CommonFunctions.h"
#include <QDate>
#include <QRegExp>
#include <QStringList>
#include <QList>
#include <qmath.h>
#include <QDir>
#include "BloombergVector.h"

int MonthDiff(const QDate& FutureDte,const QDate& PresentDte){
	int Result;
	Result=(FutureDte.year()-PresentDte.year())*12;
	Result+=FutureDte.month()-PresentDte.month();
	return Result;
}
double RoundUp(double a){
	double Result=static_cast<double>(static_cast<int>(a));
	if(a>Result) Result+=1.0;
	return Result;
}
QString Commarize(double num,unsigned int precision){
	QString Result=QString::number(num,'f',precision);
	QString Commarized;
	bool HasDecimal=Result.contains('.');
	int currnecounter=0;
	for(int j=Result.size()-1;j>=0;j--){
		if(HasDecimal){
			Commarized.prepend(Result.at(j));
			HasDecimal=Result.at(j)!='.';
			continue;
		}
		if(currnecounter%3==0 && currnecounter>0) Commarized.prepend(',');
		currnecounter++;
		Commarized.prepend(Result.at(j));
	}
	return Commarized;
}

double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, int Daycount){
	if(Dte.size()!=Flws.size() || Dte.size()==0 || Dte.size()<2) return 0.0;
	double Result=Flws.at(0);
	double DiscountFactor=1.0;
	for(int i=1;i<Dte.size();i++){
		DiscountFactor*=qPow(1.0+Interest,static_cast<double>(Dte.at(i-1).daysTo(Dte.at(i)))/static_cast<double>(Daycount));
		Result+=Flws.at(i)/DiscountFactor;//qPow(1.0+Interest,static_cast<double>(Dte.at(0).daysTo(Dte.at(i)))/static_cast<double>(Daycount));
	}
	return Result;
}
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& Interest, int Daycount){
	if(Dte.size()!=Flws.size() || Dte.size()<2) return 0.0;
	BloombergVector AdjInterest(Interest);
	if(AdjInterest.GetAnchorDate().isNull()) AdjInterest.SetAnchorDate(Dte.at(1));
	double Result=Flws.at(0);
	double DiscountFactor=1.0;
	for(int i=1;i<Dte.size();i++){
		//DiscountFactor*=qPow(1.0+AdjInterest.GetValue(Dte.at(i)),static_cast<double>(Dte.at(i-1).daysTo(Dte.at(i)))/static_cast<double>(Daycount));
		DiscountFactor*=1.0+(AdjInterest.GetValue(Dte.at(i))*static_cast<double>(Dte.at(i-1).daysTo(Dte.at(i)))/static_cast<double>(Daycount));
		Result+=Flws.at(i)/DiscountFactor;
	}
	return Result;
}
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const QString& Interest, int Daycount){
	return CalculateNPV(Dte,Flws,BloombergVector(Interest),Daycount);
}
double CalculateNPVsimple(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, int Daycount){
	if(Dte.size()!=Flws.size() || Dte.size()==0) return 0.0;
	double Result=Flws.at(0);
	for(int i=1;i<Dte.size();i++){
		Result+=Flws.at(i)/(1.0+(Interest*static_cast<double>(Dte.at(0).daysTo(Dte.at(i)))/static_cast<double>(Daycount)));
	}
	return Result;
}
double CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, int Daycount, double Guess,int precision){
	if(Guess<=0 || Guess>10) Guess=0.05;
	unsigned int CurrentIterations=0;
	double Result=Guess;
	double PreviuousGuess=Guess+0.01;
	double CurrentGuess=Guess;
	double PreviousNPV=CalculateNPV(Dte,Flws,PreviuousGuess,Daycount);
	double CurrentNPV=CalculateNPV(Dte,Flws,Result,Daycount);
	unsigned int IterationWatch;
	for (IterationWatch = 0; qAbs(CurrentNPV)>qPow(10.0, -static_cast<double>(precision)) && IterationWatch<MaximumIRRIterations; IterationWatch++)
	//while(qAbs(CurrentNPV)>qPow(10.0,-static_cast<double>(precision)))
	{
		Result-=CurrentNPV*(Result-PreviuousGuess)/(CurrentNPV-PreviousNPV);
		//if(Result<0.0 || Result>10.0) return 0.0;
		PreviousNPV=CurrentNPV;
		PreviuousGuess=CurrentGuess;
		CurrentGuess=Result;
		CurrentNPV=CalculateNPV(Dte,Flws,Result,Daycount);
		if(++CurrentIterations>=MaximumIRRIterations) return 0.0;
	}
	if (IterationWatch == MaximumIRRIterations)
		return 0.0;
	return Result;
}
double CalculateIRRSimple(const QList<QDate>& Dte, const QList<double>& Flws, int Daycount, double Guess,int precision){
	if(Guess<=0 || Guess>10) Guess=0.05;
	double Result=Guess;
	double PreviuousGuess=Guess+0.01;
	double CurrentGuess=Guess;
	double PreviousNPV=CalculateNPVsimple(Dte,Flws,PreviuousGuess,Daycount);
	double CurrentNPV=CalculateNPVsimple(Dte,Flws,Result,Daycount);
	unsigned int IterationWatch;
	for (IterationWatch = 0; qAbs(CurrentNPV)>qPow(10.0, -static_cast<double>(precision)) && IterationWatch<MaximumIRRIterations; IterationWatch++)
	//while(qAbs(CurrentNPV)>qPow(10.0,-static_cast<double>(precision)))
	{
		Result-=CurrentNPV*(Result-PreviuousGuess)/(CurrentNPV-PreviousNPV);
		//if(Result<0.0 || Result>10.0) return 0.0;
		PreviousNPV=CurrentNPV;
		PreviuousGuess=CurrentGuess;
		CurrentGuess=Result;
		CurrentNPV=CalculateNPVsimple(Dte,Flws,Result,Daycount);
	}
	if (IterationWatch == MaximumIRRIterations)
		return 0.0;
	return Result;
}
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate,int Daycount, double Guess,int precision){
	double Yld = CalculateIRR(Dte,Flws,Daycount,Guess,precision);
	double Freq=1.0;
	if(Dte.size()>2){
		Freq=(1.0/((static_cast<double>(Dte.at(1).daysTo(Dte.last())))/(static_cast<double>(Dte.size())-2.0)/365.0));
		if(Freq-static_cast<double>(static_cast<int>(Freq))>=0.5) Freq=RoundUp(Freq);
		else Freq=static_cast<double>(static_cast<int>(Freq));
	}
	double AdjYeld= (qPow(1.0+Yld,1.0/Freq)-1.0)*Freq;
	return (AdjYeld-BaseRate)*10000.0;
}
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& BaseRate,int Daycount, double Guess,int precision){
	if(Guess<=0 || Guess>10) Guess=0.05;
	double Result=Guess;
	double PreviuousGuess=Guess+0.01;
	double CurrentGuess=Guess;
	double PreviousNPV=CalculateNPV(Dte,Flws,BaseRate+PreviuousGuess,Daycount);
	double CurrentNPV=CalculateNPV(Dte,Flws,BaseRate+Result,Daycount);
	unsigned int IterationWatch;
	for (IterationWatch = 0; qAbs(CurrentNPV)>qPow(10.0, -static_cast<double>(precision)) && IterationWatch<MaximumIRRIterations; IterationWatch++)
	//while(qAbs(CurrentNPV)>qPow(10.0,-static_cast<double>(precision)))
	{
		Result-=CurrentNPV*(Result-PreviuousGuess)/(CurrentNPV-PreviousNPV);
		//if(Result<0.0 || Result>10.0) return 0.0;
		PreviousNPV=CurrentNPV;
		PreviuousGuess=CurrentGuess;
		CurrentGuess=Result;
		CurrentNPV=CalculateNPV(Dte,Flws,BaseRate+Result,Daycount);
	}
	if (IterationWatch == MaximumIRRIterations) 
		return 0.0;
	return Result*10000.0;
}
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const QString& BaseRate,int Daycount, double Guess,int precision){
	return CalculateDM(Dte,Flws,BloombergVector(BaseRate),Daycount,Guess,precision);
}
double CalculateDMSimple(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate,int Daycount, double Guess,int precision){
	return (CalculateIRRSimple(Dte,Flws,Daycount,Guess,precision)-BaseRate)*10000.0;
}
bool removeDir(const QString & dirName)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName)) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = removeDir(info.absoluteFilePath());
			}
			else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = dir.rmdir(dirName);
	}
	return result;
}