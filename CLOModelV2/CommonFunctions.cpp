#include "CommonFunctions.h"
#include <QDate>
#include <QRegExp>
#include <QStringList>
#include <QList>
#include <qmath.h>
#include <QDir>
#include <QTextStream>
#include <QDate>
#include "BloombergVector.h"
#include <boost/math/tools/roots.hpp>
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

double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, DayCountConvention Daycount){
	if(Dte.size()!=Flws.size() || Dte.size()==0 || Dte.size()<2) return 0.0;
	double Result=Flws.at(0);
	double DiscountFactor=1.0;
	for(int i=1;i<Dte.size();i++){
		DiscountFactor *= 1.0 + AdjustCoupon(Interest, Dte.at(i - 1), Dte.at(i), Daycount);
		Result+=Flws.at(i)/DiscountFactor;
	}
	return Result;
}
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& Interest, DayCountConvention Daycount) {
	if(Dte.size()!=Flws.size() || Dte.size()<2) return 0.0;
	BloombergVector AdjInterest(Interest);
	if(AdjInterest.GetAnchorDate().isNull()) AdjInterest.SetAnchorDate(Dte.at(1));
	double Result=Flws.at(0);
	double DiscountFactor=1.0;
	for(int i=1;i<Dte.size();i++){
		//if (Flws.at(i) == 0.0) continue; //WRONG, Discount factor should be adjusted regardless
		DiscountFactor*=1.0+AdjustCoupon(AdjInterest.GetValue(Dte.at(i)),Dte.at(i-1),Dte.at(i),Daycount);
		Result+=Flws.at(i)/DiscountFactor;
	}
	return Result;
}
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const QString& Interest, DayCountConvention Daycount) {
	return CalculateNPV(Dte,Flws,BloombergVector(Interest),Daycount);
}
double CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, DayCountConvention Daycount, double Guess) {
	if (Guess <= 0 || Guess > 10) Guess = 0.05;
	boost::math::tools::eps_tolerance<double> tol(std::numeric_limits<double>::digits / 2);
	boost::uintmax_t MaxIter(MaximumIRRIterations);
	std::pair<double, double> Result = boost::math::tools::bracket_and_solve_root(
		[&](double Discount) -> double {return CalculateNPV(Dte, Flws, Discount, Daycount); }
	, Guess, 2.0, false, tol, MaxIter);
	if (MaxIter >= MaximumIRRIterations) return 0.0;
	return (Result.first + Result.second) / 2.0;
}
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate, DayCountConvention Daycount, double Guess) {
	return CalculateDM(Dte, Flws, BloombergVector(QString("%1").arg(BaseRate)), Daycount, Guess);
}

double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& BaseRate, DayCountConvention Daycount, double Guess) {
	if (Guess <= 0 || Guess>10) Guess = 0.05;
	boost::math::tools::eps_tolerance<double> tol(std::numeric_limits<double>::digits / 2);
	boost::uintmax_t MaxIter(MaximumIRRIterations);
	std::pair<double, double> Result = boost::math::tools::bracket_and_solve_root(
		[&](double Discount) -> double {return CalculateNPV(Dte, Flws, BaseRate + Discount, Daycount); }
	, Guess, 2.0, false, tol, MaxIter);
	if (MaxIter >= MaximumIRRIterations) return 0.0;
	return 10000.0*(Result.first + Result.second) / 2.0;
}
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const QString& BaseRate, DayCountConvention Daycount, double Guess) {
	return CalculateDM(Dte,Flws,BloombergVector(BaseRate),Daycount,Guess);
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

double AdjustCoupon(double AnnualCoupon, QDate PrevIPD, QDate CurrIPD, DayCountConvention DayCount) {
	if (PrevIPD.isNull() || CurrIPD.isNull()) return AnnualCoupon;
	if (AnnualCoupon == 0.0) return 0.0;
	if (PrevIPD > CurrIPD) {
		QDate Temp = PrevIPD;
		PrevIPD = CurrIPD;
		CurrIPD = Temp;
	}
	int Offset = 0;
	switch (DayCount) {
	case DayCountConvention::ISMA30360:
		if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30);
		if (CurrIPD.day() == 31) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30);
		return AnnualCoupon * (
			(360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
			(30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
			static_cast<double>(CurrIPD.day() - PrevIPD.day())
			) / 360.0;
	case DayCountConvention::N30360:
		if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30);
		if (CurrIPD.day() == 31 && PrevIPD.day() == 30) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30);
		if (PrevIPD.day() == 29 && PrevIPD.month() == 2 && QDate::isLeapYear(PrevIPD.year())) Offset = 1;
		if (PrevIPD.day() == 28 && PrevIPD.month() == 2 && !QDate::isLeapYear(PrevIPD.year())) Offset = 2;
		return AnnualCoupon * (
				(360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
				(30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
				static_cast<double>(CurrIPD.day() - PrevIPD.day() - Offset)
			) /360.0;
	case DayCountConvention::ISDAACTACT:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NISDAACTACT:
		if (QDate::isLeapYear(CurrIPD.year()) || QDate::isLeapYear(PrevIPD.year())) {
			return AnnualCoupon * (
				(static_cast<double>(PrevIPD.daysTo(QDate(PrevIPD.year(), 12, 31))) / static_cast<double>(PrevIPD.daysInYear()))
				+ (static_cast<double>(QDate(CurrIPD.year(), 1, 1).daysTo(CurrIPD)) / static_cast<double>(CurrIPD.daysInYear()))
			);
		}
		else return AdjustCoupon(AnnualCoupon, PrevIPD, CurrIPD, DayCountConvention::ACT365);
	case DayCountConvention::ACT360:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NACT360:
		return AnnualCoupon * static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 360.0;
	case DayCountConvention::ACT365:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NACT365:
		return AnnualCoupon * static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 365.0;
	case DayCountConvention::AFBACTACT:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NAFBACTACT:
		for (QDate TempDate = PrevIPD; TempDate <= CurrIPD; TempDate = TempDate.addYears(1)) {
			if (QDate(TempDate.year(), 2, 29).isValid()) {
				if(QDate(TempDate.year(), 2, 29) >= PrevIPD && QDate(TempDate.year(), 2, 29) <= CurrIPD)
					return AnnualCoupon * static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 366.0;
			}
		}
		return AnnualCoupon * static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 365.0;
	case DayCountConvention::ACTACT:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NACTACT:
		return AnnualCoupon * static_cast<double>(PrevIPD.daysTo(CurrIPD)) / CurrIPD.daysInYear();
	default:
		return AnnualCoupon;
	}
}
bool IsHoliday(const QDate& a/*, const QString& CountryCode*/) {
	return a.dayOfWeek() >= 6;
}


#include <QFile>
void PrintToTempFile(const QString& TempFileName, const QString& Message, bool PrintTime) {
	QFile TempFile("C:/Temp/" + TempFileName +".log");
	if (!TempFile.open(QIODevice::Append | QIODevice::Text)) return;
	QTextStream  TempWrite(&TempFile);
	TempWrite << (PrintTime ? QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm "):QString()) + Message + '\n';
	TempFile.close();
}
#include <QSettings>
#include <QRegExp>
//UGLY!!!
double GetLoanAssumption(const QString& LoanName, int columnIndex, QDate RefDate) {
/*columnIndex
0-Loan Identifier
1-Senior Price
2-Sub Price
3-Default
4-Senior Haircut Amt
5-Senior Haircut Period
6-Sub Haircut Amt
7-Sub Haircut Period
8-Prepay Period
9-View
*/
	
	QStringList Assumptions;
	QStringList AKAs;
	int FoundLoan = -1;
	int LineCounter = 0;
	if (columnIndex >= 0 && columnIndex <= 9 && !LoanName.isEmpty()) {
		QSettings ConfigIni(":/Configs/GlobalConfigs.ini", QSettings::IniFormat);
		ConfigIni.beginGroup("Folders");
		QFile AssumptionsFile(ConfigIni.value("UnifiedResultsFolder", "Z:/24AM/Monitoring/Model Results").toString() + '/' + ConfigIni.value("AssumptionsFile", "Loans Assumptions").toString());
		if (AssumptionsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QString CurrentLoan;
			QTextStream Streamer(&AssumptionsFile);
			while (!Streamer.atEnd() && FoundLoan < 0) {
				CurrentLoan = Streamer.readLine();
				CurrentLoan=CurrentLoan.right(CurrentLoan.size() - 1);
				CurrentLoan=CurrentLoan.left(CurrentLoan.size() - 1);
				Assumptions = CurrentLoan.split("#,#");
				AKAs = Assumptions.first().split("$,$");
				QRegExp WholeWordRX;
				WholeWordRX.setCaseSensitivity(Qt::CaseInsensitive);
				for (FoundLoan = 0; FoundLoan < AKAs.size(); ++FoundLoan) {
					WholeWordRX.setPattern("\\b" + LoanName + "\\b");
					if (AKAs.at(FoundLoan).contains(WholeWordRX))
						break;
					WholeWordRX.setPattern("\\b" + AKAs.at(FoundLoan) + "\\b");
					if (LoanName.contains(WholeWordRX))
						break;
				}
				if (FoundLoan >= AKAs.size()) FoundLoan = -1;
				++LineCounter;
			}
			AssumptionsFile.close();
		}
	}
	if (FoundLoan < 0) {
		return -1.0;
	}
	else {
		switch (columnIndex) {
		case 0: //Scenario Name
			return static_cast<double>(LineCounter);
		case 3: //Default
			return (Assumptions.at(3) == "False" ? 0.0 : 1.0);
		case 5: //Senior Haircut Period
		case 7: //Sub Haircut Period
		case 8: //Prepay Period
			if (RefDate.isNull()) RefDate = QDate::currentDate();
			if (Assumptions.at(columnIndex).isEmpty()) return 0.0;
			return qMax(1.0,static_cast<double>(Assumptions.at(columnIndex).toDouble() - MonthDiff(RefDate,QDate::fromString(Assumptions.last(), "yyyy-MM-dd"))));
		default:
			return Assumptions.at(columnIndex).toDouble();
		}
	}
}

