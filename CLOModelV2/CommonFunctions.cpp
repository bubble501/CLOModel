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
#include "DayCountVect.h"
#include <boost/math/tools/roots.hpp>
#include <QStack>
#include "AbstractTrigger.h"
#include <QXmlStreamReader>
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

double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, const DayCountVector& Daycount) {
	if(Dte.size()!=Flws.size() || Dte.size()==0 || Dte.size()<2) return 0.0;
	double Result=Flws.at(0);
	double DiscountFactor=1.0;
	for(int i=1;i<Dte.size();i++){
		DiscountFactor *= 1.0 + AdjustCoupon(Interest, Dte.at(i - 1), Dte.at(i), Daycount.GetValue(Dte.at(i)));
		Result+=Flws.at(i)/DiscountFactor;
	}
	return Result;
}
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& Interest, const DayCountVector& Daycount) {
	if(Dte.size()!=Flws.size() || Dte.size()<2) return 0.0;
	BloombergVector AdjInterest(Interest);
	if(AdjInterest.GetAnchorDate().isNull()) AdjInterest.SetAnchorDate(Dte.at(1));
	double Result=Flws.at(0);
	double DiscountFactor=1.0;
	for(int i=1;i<Dte.size();i++){
		DiscountFactor *= 1.0 + AdjustCoupon(AdjInterest.GetValue(Dte.at(i)), Dte.at(i - 1), Dte.at(i), Daycount.GetValue(Dte.at(i)));
		Result+=Flws.at(i)/DiscountFactor;
	}
	return Result;
}
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const QString& Interest, const DayCountVector& Daycount) {
	return CalculateNPV(Dte,Flws,BloombergVector(Interest),Daycount);
}
double CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, const DayCountVector& Daycount, double Guess) {
	if (Guess <= 0 || Guess > 10) Guess = 0.05;
	boost::math::tools::eps_tolerance<double> tol(std::numeric_limits<double>::digits / 2);
	boost::uintmax_t MaxIter(MaximumIRRIterations);
	std::pair<double, double> Result = boost::math::tools::bracket_and_solve_root(
		[&](double Discount) -> double {return CalculateNPV(Dte, Flws, Discount, Daycount); }
	, Guess, 2.0, false, tol, MaxIter);
	if (MaxIter >= MaximumIRRIterations) return 0.0;
	return (Result.first + Result.second) / 2.0;
}
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate, const DayCountVector& Daycount, double Guess) {
	return CalculateDM(Dte, Flws, BloombergVector(QString("%1").arg(BaseRate)), Daycount, Guess);
}

double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& BaseRate, const DayCountVector& Daycount, double Guess) {
	if (Guess <= 0 || Guess>10) Guess = 0.05;
	boost::math::tools::eps_tolerance<double> tol(std::numeric_limits<double>::digits / 2);
	boost::uintmax_t MaxIter(MaximumIRRIterations);
	std::pair<double, double> Result = boost::math::tools::bracket_and_solve_root(
		[&](double Discount) -> double {return CalculateNPV(Dte, Flws, BaseRate + Discount, Daycount); }
	, Guess, 2.0, false, tol, MaxIter);
	if (MaxIter >= MaximumIRRIterations) return 0.0;
	return 10000.0*(Result.first + Result.second) / 2.0;
}
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const QString& BaseRate, const DayCountVector& Daycount, double Guess) {
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
	double TimeFactor;
	int Offset = 0;
	switch (static_cast<DayCountConvention>(static_cast<qint16>(DayCount) & (((1 << CompoundShift)-1)))) {
	case DayCountConvention::ISMA30360:
		if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30);
		if (CurrIPD.day() == 31) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30);
		TimeFactor=(
			(360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
			(30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
			static_cast<double>(CurrIPD.day() - PrevIPD.day())
			) / 360.0;
		break;
	case DayCountConvention::N30360:
		if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30);
		if (CurrIPD.day() == 31 && PrevIPD.day() == 30) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30);
		if (PrevIPD.day() == 29 && PrevIPD.month() == 2 && QDate::isLeapYear(PrevIPD.year())) Offset = 1;
		if (PrevIPD.day() == 28 && PrevIPD.month() == 2 && !QDate::isLeapYear(PrevIPD.year())) Offset = 2;
		TimeFactor = (
				(360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
				(30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
				static_cast<double>(CurrIPD.day() - PrevIPD.day() - Offset)
			) /360.0;
		break;
	case DayCountConvention::ISDAACTACT:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NISDAACTACT:
		if (QDate::isLeapYear(CurrIPD.year()) || QDate::isLeapYear(PrevIPD.year())) {
			TimeFactor = (
				(static_cast<double>(PrevIPD.daysTo(QDate(PrevIPD.year(), 12, 31))) / static_cast<double>(PrevIPD.daysInYear()))
				+ (static_cast<double>(QDate(CurrIPD.year(), 1, 1).daysTo(CurrIPD)) / static_cast<double>(CurrIPD.daysInYear()))
			);
		}
		else return AdjustCoupon(AnnualCoupon, PrevIPD, CurrIPD, DayCountConvention::ACT365);
		break;
	case DayCountConvention::ACT360:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NACT360:
		TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 360.0;
		break;
	case DayCountConvention::ACT365:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NACT365:
		TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 365.0;
		break;
	case DayCountConvention::AFBACTACT:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NAFBACTACT:
		for (QDate TempDate = PrevIPD; TempDate <= CurrIPD; TempDate = TempDate.addYears(1)) {
			if (QDate(TempDate.year(), 2, 29).isValid()) {
				if(QDate(TempDate.year(), 2, 29) >= PrevIPD && QDate(TempDate.year(), 2, 29) <= CurrIPD)
					TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 366.0;
			}
		}
		TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 365.0;
		break;
	case DayCountConvention::ACTACT:
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
	case DayCountConvention::NACTACT:
		TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / CurrIPD.daysInYear();
		break;
	default:
		TimeFactor = 1.0;
	}
	if (static_cast<qint16>(DayCount)& (1 << CompoundShift)) //Compounded
		return qPow(1.0 + AnnualCoupon, TimeFactor) - 1.0;
	else if (static_cast<qint16>(DayCount)& (1 << (1 + CompoundShift))) //Continuously Compounded
		return qExp(AnnualCoupon*TimeFactor);
	else //Simple
		return AnnualCoupon * TimeFactor;
}
bool IsHoliday(const QDate& a/*, const QString& CountryCode*/) {
	return a.dayOfWeek() >= 6;
}
bool ValidDayCount(qint16 a) {
	DayCountConvention b;
	switch (a) {
	case static_cast<qint16>(DayCountConvention::ACTACT) :
	case static_cast<qint16>(DayCountConvention::ACT360) :
	case static_cast<qint16>(DayCountConvention::ACT365) :
	case static_cast<qint16>(DayCountConvention::N30360) :
	case static_cast<qint16>(DayCountConvention::NACTACT) :
	case static_cast<qint16>(DayCountConvention::NACT360) :
	case static_cast<qint16>(DayCountConvention::NACT365) :
	case static_cast<qint16>(DayCountConvention::ISMA30360) :
	case static_cast<qint16>(DayCountConvention::ISDAACTACT) :
	case static_cast<qint16>(DayCountConvention::AFBACTACT) :
	case static_cast<qint16>(DayCountConvention::NISDAACTACT) :
	case static_cast<qint16>(DayCountConvention::NAFBACTACT) :
	case static_cast<qint16>(DayCountConvention::CompACTACT) :
	case static_cast<qint16>(DayCountConvention::CompACT360) :
	case static_cast<qint16>(DayCountConvention::CompACT365) :
	case static_cast<qint16>(DayCountConvention::CompN30360) :
	case static_cast<qint16>(DayCountConvention::CompNACTACT) :
	case static_cast<qint16>(DayCountConvention::CompNACT360) :
	case static_cast<qint16>(DayCountConvention::CompNACT365) :
	case static_cast<qint16>(DayCountConvention::CompISMA30360) :
	case static_cast<qint16>(DayCountConvention::CompISDAACTACT) :
	case static_cast<qint16>(DayCountConvention::CompAFBACTACT) :
	case static_cast<qint16>(DayCountConvention::CompNISDAACTACT) :
	case static_cast<qint16>(DayCountConvention::CompNAFBACTACT) :
	case static_cast<qint16>(DayCountConvention::ContCompACTACT) :
	case static_cast<qint16>(DayCountConvention::ContCompACT360) :
	case static_cast<qint16>(DayCountConvention::ContCompACT365) :
	case static_cast<qint16>(DayCountConvention::ContCompN30360) :
	case static_cast<qint16>(DayCountConvention::ContCompNACTACT) :
	case static_cast<qint16>(DayCountConvention::ContCompNACT360) :
	case static_cast<qint16>(DayCountConvention::ContCompNACT365) :
	case static_cast<qint16>(DayCountConvention::ContCompISMA30360) :
	case static_cast<qint16>(DayCountConvention::ContCompISDAACTACT) :
	case static_cast<qint16>(DayCountConvention::ContCompAFBACTACT) :
	case static_cast<qint16>(DayCountConvention::ContCompNISDAACTACT) :
	case static_cast<qint16>(DayCountConvention::ContCompNAFBACTACT) :
		return true;
	default:
		return false;
	}
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
		QFile AssumptionsFile(GetFromConfig("Folders", "UnifiedResultsFolder", R"(\\synserver2\Company Share\24AM\Monitoring\Model Results)") + '/' + GetFromConfig("Folders", "AssumptionsFile", "Loans Assumptions"));
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

QString InfixToPostfix(const QString& a) {
	if (a.isEmpty()) return QString();
	QString ops("-+/*");
	{
		//Prevent Operators at the beginning of the string
		QRegExp CheckDoubleOperator("^[" + ops + ']', Qt::CaseInsensitive);
		if (CheckDoubleOperator.indexIn(a) >= 0) return QString();
		//Prevent Unmatched parenthesis
		if (a.count('(') != a.count(')')) return QString();
		//Prevent two operators one after another
		CheckDoubleOperator.setPattern('[' + ops + "]\\s*[" + ops + ']');
		if (CheckDoubleOperator.indexIn(a) >= 0) return QString();
		//Prevent operator just after NOT
		CheckDoubleOperator.setPattern("![" + ops + ']');
		if (CheckDoubleOperator.indexIn(a) >= 0) return QString();

		ops += '!';

		//Prevent operators just before close of parenthesis
		CheckDoubleOperator.setPattern('[' + ops + "]\\)");
		if (CheckDoubleOperator.indexIn(a) >= 0) return QString();
		//Prevent Operators at the end of the string
		CheckDoubleOperator.setPattern('[' + ops + "]$");
		if (CheckDoubleOperator.indexIn(a) >= 0) return QString();
	}
	QRegExp CheckValidNumber("(?:\\d+|F|T)",Qt::CaseInsensitive);
	QString Spaced("");
	for (auto i = a.constBegin(); i != a.constEnd(); ++i) {
		if (i->isSpace()) continue;
		if (Spaced.isEmpty()) {
			Spaced.append(*i);
		}
		else  {
			if (!(i->isDigit() && ((i - 1)->isDigit()))) Spaced.append(' ');
				Spaced.append(*i);
		}
	}
	QStack<qint32> s;
	QString sb("");
	QStringList parts = Spaced.split(QRegExp("\\s"));
	foreach(const QString& token, parts) {
		int idx = ops.indexOf(token.at(0));
		if (idx != -1 && token.size() == 1) {
			if (s.isEmpty())
				s.push(idx);
			else {
				while (!s.isEmpty()) {
					int prec2 = s.top() / 2;
					int prec1 = idx / 2;
					if (prec2 > prec1 || (prec2 == prec1))
						sb.append(ops.at(s.pop())).append(' ');
					else break;
				}
				s.push(idx);
			}
		}
		else if (token.at(0) == '(') {
			s.push(-2);
		}
		else if (token.at(0) == ')') {
			while (s.top() != -2)
				sb.append(ops.at(s.pop())).append(' ');
			s.pop();
		}
		else{
			if (!CheckValidNumber.exactMatch(token)) 
				return QString();
			sb.append(token).append(' ');
		}
	}
	while (!s.isEmpty()) {
		sb.append(ops.at(s.pop())).append(' ');
	}
	return sb.trimmed();
}
QString NormaliseTriggerStructure(QString a) {
	a=a.toLower();
	a.replace("nand", "/");
	a.replace("and", "*");
	a.replace("nor", "-");
	a.replace("or", "+");
	a.replace("not", "!");
	a.replace("^", "!");
	a.replace(QRegExp("&?&"), "*");
	a.replace(QRegExp("|?|"), "+");
	a.replace("false", "f");
	a.replace("true", "t");
	a.replace(QRegExp("\\s"), "");
	return a.toUpper();
}

QString GetFromConfig(const QString& Domain, const QString& Field, const QString& DefaultValue) {
	QFile Source(":/Configs/GlobalConfigs.xml");
	Source.open(QIODevice::ReadOnly);
	QXmlStreamReader xml(&Source);
	bool DomainFound=false;
	bool FieldFound = false;
	while (!xml.atEnd() && !xml.hasError()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name()==Domain) {
				if (DomainFound) return DefaultValue;
				DomainFound = true;
			}
			else if (DomainFound && xml.name() == Field) {
				if (FieldFound) return DefaultValue;
				FieldFound = true;
			}
		}
		else if (xml.isEndElement()) {
			if (xml.name() == Domain) {
				if (!DomainFound) return DefaultValue;
				DomainFound = false;
			}
			else if (xml.name() == Field) {
				if (!FieldFound) return DefaultValue;
				FieldFound = false;
			}
		}
		else if (xml.isCharacters() && FieldFound) {
			return xml.text().toString();
		}
	}
	return DefaultValue;
}
