#include "CommonFunctions.h"
#include <QDate>
#include <QRegExp>
#include <QStringList>
#include <qmath.h>
#include <QDir>
int MonthDiff(const QDate& a,const QDate& b){
	int Result;
	Result=(a.year()-b.year())*12;
	Result+=a.month()-b.month();
	return Result;
}
bool ValidAnnuityVector(const QString& AnnVect){
	QRegExp Vigil("^[YN]( \\d+S [YN])*$",Qt::CaseInsensitive);
	return Vigil.exactMatch(AnnVect);
}
bool ValidBloombergVector(const QString& BloombergVector){
	QRegExp Vigil("^[0-9]*[,.]?[0-9]+( \\d+[RS] [0-9]*[,.]?[0-9]+)*$",Qt::CaseInsensitive);
	return Vigil.exactMatch(BloombergVector);
}
QList<double> UnpackVect(QString Vect, int PaymFreq, bool AdjustFreq){
	QList<double> Result;
	if(!ValidBloombergVector(Vect)) return Result;
	QStringList StringParts=Vect.trimmed().toUpper().split(' ');
	int StepLen;
	QString TempStr;
	for (int i=1;i<StringParts.size();i+=2){
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\D"),"");
		StepLen=TempStr.toInt();
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\d"),"");
		if(TempStr=="S"){
			for (int j=0;j<StepLen;j++){
				Result.append(StringParts.at(i-1).toDouble());
			}
		}
		else{
			for (int j=0;j<StepLen;j++){
				Result.append(
					StringParts.at(i - 1).toDouble() + ((StringParts.at(i + 1).toDouble() - StringParts.at(i - 1).toDouble()) * static_cast<double>(j) / static_cast<double>(StepLen))
				);
			}
		}
	}
	Result.append(StringParts.last().toDouble());
	for(QList<double>::iterator i=Result.begin();i!=Result.end();i++){
		if(AdjustFreq)
			*i=qPow(1.0 + (*i / 100.0) , static_cast<double>(PaymFreq) / 12.0) - 1.0;
		else
			*i/=100.0;
	}
	return Result;
}
QList<QString> UnpackAnnuityVect(QString Vect){
	QList<QString> Result;
	if(!ValidAnnuityVector(Vect)) return Result;
	QStringList StringParts=Vect.trimmed().toUpper().split(' ');
	int StepLen;
	QString TempStr;
	for (int i=1;i<StringParts.size();i+=2){
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\D"),"");
		StepLen=TempStr.toInt();
		for (int j=0;j<StepLen;j++){
			Result.append(StringParts.at(i-1));
		}
	}
	Result.append(StringParts.last());
	return Result;
}
QString ShiftBloombergVector(const QString& OriginalVect, int ByMonths){
	if(!ValidBloombergVector(OriginalVect)) return "";
	if(ByMonths==0) return OriginalVect;
	QString Result;
	bool Shifted=false;
	int ShiftedValue;
	QStringList VectorParts=OriginalVect.toUpper().split(" ");
	for (int i=0;i<VectorParts.size();i++){
		if(!Result.isEmpty()) Result+= ' ';
		if(i%2==0 || Shifted) {Result+=VectorParts.at(i);}
		else{
			ShiftedValue= VectorParts.at(i).left(VectorParts.at(i).size()-1).toInt()-ByMonths;
			if(ShiftedValue<=0){
				Result="";
				ByMonths=-ShiftedValue;
			}
			else{
				Result+=QString::number(ShiftedValue)+VectorParts.at(i).right(1);
				Shifted=true;
			}
		}
	}
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
	if(Dte.size()!=Flws.size()) return 0.0;
	double Result=Flws.at(0);
	for(int i=1;i<Dte.size();i++){
		Result+=Flws.at(i)/qPow(1.0+Interest,static_cast<double>(Dte.at(0).daysTo(Dte.at(i)))/static_cast<double>(Daycount));
	}
	return Result;
}
double CalculateNPVsimple(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, int Daycount){
	if(Dte.size()!=Flws.size()) return 0.0;
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
	while(qAbs(CurrentNPV)>qPow(10.0,-static_cast<double>(precision))){
		Result-=CurrentNPV*(Result-PreviuousGuess)/(CurrentNPV-PreviousNPV);
		//if(Result<0.0 || Result>10.0) return 0.0;
		PreviousNPV=CurrentNPV;
		PreviuousGuess=CurrentGuess;
		CurrentGuess=Result;
		CurrentNPV=CalculateNPV(Dte,Flws,Result,Daycount);
		if(++CurrentIterations>=MaximumIRRIterations) return 0.0;
	}
	return Result;
}
double CalculateIRRSimple(const QList<QDate>& Dte, const QList<double>& Flws, int Daycount, double Guess,int precision){
	if(Guess<=0 || Guess>10) Guess=0.05;
	double Result=Guess;
	double PreviuousGuess=Guess+0.01;
	double CurrentGuess=Guess;
	double PreviousNPV=CalculateNPVsimple(Dte,Flws,PreviuousGuess,Daycount);
	double CurrentNPV=CalculateNPVsimple(Dte,Flws,Result,Daycount);
	while(qAbs(CurrentNPV)>qPow(10.0,-static_cast<double>(precision))){
		Result-=CurrentNPV*(Result-PreviuousGuess)/(CurrentNPV-PreviousNPV);
		//if(Result<0.0 || Result>10.0) return 0.0;
		PreviousNPV=CurrentNPV;
		PreviuousGuess=CurrentGuess;
		CurrentGuess=Result;
		CurrentNPV=CalculateNPVsimple(Dte,Flws,Result,Daycount);
	}
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