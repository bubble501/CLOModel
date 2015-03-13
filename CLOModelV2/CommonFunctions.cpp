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
#include <QFile>
#include "simstring.h"
#ifndef NO_DATABASE
QMutex Db_Mutex;
#endif
const QString LoansPropertiesToSearch[] = { "Issuer", "Facility" };
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
	if (Dte.size() != Flws.size() || Dte.isEmpty() || Daycount.IsEmpty()) return 0.0;
	double Result=Flws.at(0);
	double DiscountFactor=1.0;
	for(int i=1;i<Dte.size();i++){
		DiscountFactor *= 1.0 + AdjustCoupon(Interest, Dte.at(i - 1), Dte.at(i), Daycount.GetValue(Dte.at(i)));
		Result+=Flws.at(i)/DiscountFactor;
	}
	return Result;
}
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& Interest, const DayCountVector& Daycount) {
	if (Dte.size() != Flws.size() || Dte.isEmpty() || Interest.IsEmpty() || Daycount.IsEmpty()) return 0.0;
	BloombergVector AdjInterest(Interest);
	if (AdjInterest.GetAnchorDate().isNull() && Dte.size()>1) AdjInterest.SetAnchorDate(Dte.at(1));
	double Result=Flws.first();
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
	if (Flws.size() <= 1 || Dte.size() != Flws.size() || Daycount.IsEmpty()) return 0.0;
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
	if (Flws.size() <= 1 || Dte.size() != Flws.size() || BaseRate.IsEmpty() || Daycount.IsEmpty()) return 0.0;
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

double AdjustCoupon(double AnnualCoupon /*Annualised Coupon*/, QDate PrevIPD /*Interesty start accrual date*/, QDate CurrIPD /*Interesty end accrual date*/, DayCountConvention DayCount) {
	if (PrevIPD.isNull() || CurrIPD.isNull()) return AnnualCoupon; //If dates are invalid return the annual coupon
	if (AnnualCoupon == 0.0) return 0.0; //If the annual coupon is 0 there is no need to adjust it
	if (PrevIPD > CurrIPD) { //If start accrual is after end accrual invert the two dates
		QDate Temp = PrevIPD;
		PrevIPD = CurrIPD;
		CurrIPD = Temp;
	}
	double TimeFactor; //This is the factor by which the coupon must be adjusted
	int Offset = 0;
	/*
	DayCount is a 16 bit integer;
	the bit 1 to CompoundShift contain the day count convention Bloomberg code (see DAY COUNT CODES - MBS <HELP>)
	if the interest is accrued according to the compounding formula the bit CompoundShift+1 is set 
	if the interest is accrued according to the continuous compounding formula the bit CompoundShift+2 is set
	if the interest is accrued according to the simple compounding formula neither bit is set
	*/
	// check the first CompoundShift bits of the DayCount
	switch (static_cast<DayCountConvention>(static_cast<qint16>(DayCount) & (((1 << CompoundShift)-1)))) {
	case DayCountConvention::ISMA30360:
		//if an accrual date falls on day 31 move it to day 30
		if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30);
		if (CurrIPD.day() == 31) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30);
		/* The adjustment factor is:
			(
				(360 * difference in years of the two dates)
				+ (30 * difference in months of the two dates)
				+ difference in days of the two dates
			)/360
		*/
		TimeFactor=(
			(360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
			(30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
			static_cast<double>(CurrIPD.day() - PrevIPD.day())
			) / 360.0;
		break;
	case DayCountConvention::N30360:
		if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30); //if start accrue falls on the 31st move it to the 30th
		if (CurrIPD.day() == 31 && PrevIPD.day() == 30) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30); //if end accrue falls on the 31st and start accrue falls on the 30th move end accrue to the 30th
		// if the previous IPD was on the last day of Feb set an offset to simulate it happening on the 30th of Feb
		if (PrevIPD.day() == 29 && PrevIPD.month() == 2 && QDate::isLeapYear(PrevIPD.year())) Offset = 1;
		if (PrevIPD.day() == 28 && PrevIPD.month() == 2 && !QDate::isLeapYear(PrevIPD.year())) Offset = 2;
		/* The adjustment factor is:
			(
				(360 * difference in years of the two dates)
				+ (30 * difference in months of the two dates)
				+ difference in days of the two dates
				- Offset
			)/360
		*/
		TimeFactor = (
				(360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
				(30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
				static_cast<double>(CurrIPD.day() - PrevIPD.day() - Offset)
			) /360.0;
		break;
	case DayCountConvention::ISDAACTACT:
		// For non nominal day count conventions move payment dates that are in weekends or bank holidays
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
		//fall through next case
	case DayCountConvention::NISDAACTACT:
		// if any of the date is a leap year
		if (QDate::isLeapYear(CurrIPD.year()) || QDate::isLeapYear(PrevIPD.year())) {
			//Adjust the coupon by the actual days between the accrual dates
			TimeFactor = (
				//days from start accrual date to end of year divided by days in that year (365 or 366 if it's leap)
				(static_cast<double>(PrevIPD.daysTo(QDate(PrevIPD.year(), 12, 31))) / static_cast<double>(PrevIPD.daysInYear()))
				// Full years between the two dates
				+ static_cast<double>(CurrIPD.year() - PrevIPD.year()-1)
				//days start of year to end accrual date to end of year divided by days in that year (365 or 366 if it's leap)
				+ (static_cast<double>(QDate(CurrIPD.year(), 1, 1).daysTo(CurrIPD)) / static_cast<double>(CurrIPD.daysInYear()))
			);
		}
		//Otherwise this is the same as ACT/365
		else return AdjustCoupon(AnnualCoupon, PrevIPD, CurrIPD, static_cast<DayCountConvention>((static_cast<qint16>(DayCount)& ((~0) << CompoundShift)) | static_cast<qint16>(DayCountConvention::NACT365)));
		break;
	case DayCountConvention::ACT360:
		// For non nominal day count conventions move payment dates that are in weekends or bank holidays
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
		//fall through next case
	case DayCountConvention::NACT360:
		//Difference between the dates divided 360
		TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 360.0;
		break;
    case DayCountConvention::ACT365:
        // For non nominal day count conventions move payment dates that are in weekends or bank holidays
        while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
        while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
        //fall through next case
    case DayCountConvention::NACT365:
        //Difference between the dates divided 365
        TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 365.0;
        break;
	case DayCountConvention::AFBACTACT:
		// For non nominal day count conventions move payment dates that are in weekends or bank holidays
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
		//fall through next case
	case DayCountConvention::NAFBACTACT:{
		// if any date between start and end accrue is in a leap year use the difference between the dates divided 366
		bool FoundLeap = false;
		for (QDate TempDate = PrevIPD; TempDate <= CurrIPD && !FoundLeap; TempDate = TempDate.addYears(1)) {
			if (QDate(TempDate.year(), 2, 29).isValid()) {
				if (QDate(TempDate.year(), 2, 29) >= PrevIPD && QDate(TempDate.year(), 2, 29) <= CurrIPD) {
					TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 366.0;
					FoundLeap = true;
				}
			}
		}
		//otherwise use the difference between the dates divided 365
		if (!FoundLeap) TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 365.0;
		break;
	}
	case DayCountConvention::ACTACT:
		// For non nominal day count conventions move payment dates that are in weekends or bank holidays
		while (IsHoliday(CurrIPD)) CurrIPD = CurrIPD.addDays(1);
		while (IsHoliday(PrevIPD)) PrevIPD = PrevIPD.addDays(1);
		//fall through next case
	case DayCountConvention::NACTACT:
		//difference between the dates divided by days in the end accrue year
		TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / CurrIPD.daysInYear();
		break;
	default:
		TimeFactor = 1.0;
	}
	//Check the CompoundShift+1 th bit, if it's set calculate the interest as ((1+r)^t)-1
	if (static_cast<qint16>(DayCount)& (1 << CompoundShift)) //Compounded
		return qPow(1.0 + AnnualCoupon, TimeFactor) - 1.0;
	//Check the CompoundShift+2 th bit, if it's set calculate the interest as (e^(r*t))-1
	else if (static_cast<qint16>(DayCount)& (1 << (1 + CompoundShift))) //Continuously Compounded
		return qExp(AnnualCoupon*TimeFactor)-1.0;
	//Otherwise calculate the interest as r*t
	else //Simple
		return AnnualCoupon * TimeFactor;
}
bool IsHoliday(const QDate& a/*, const QString& CountryCode*/) {
	//TODO check for bank holidays
	return a.dayOfWeek() >= 6;
}
bool ValidDayCount(qint16 a) {
	DayCountConvention b;
	switch (a & (((1 << CompoundShift) - 1))) {
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
		return (a >> CompoundShift)<=2; //Special case
		//return NumberOfSetBits(a >> CompoundShift) <= 1; //General case
	default:
		return false;
	}
}


void PrintToTempFile(const QString& TempFileName, const QString& Message, bool PrintTime) {
	QFile TempFile("C:/Temp/" + TempFileName +".log");
	if (!TempFile.open(QIODevice::Append | QIODevice::Text)) return;
	QTextStream  TempWrite(&TempFile);
	TempWrite << (PrintTime ? QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm "):QString()) + Message + '\n';
	TempFile.close();
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
int NumberOfSetBits(quint32 i) {
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
void BuildDBCountries(const QString& path) {
	if (QFile::exists(path)) {
		QFileInfo FiletoDelete(path);
		QDir ParDir = FiletoDelete.absoluteDir();
		ParDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		ParDir.setNameFilters(QStringList() << "*.ssdb.*.cdb");
		auto FilesInFolder = ParDir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		for (auto fifi = FilesInFolder.constBegin(); fifi != FilesInFolder.constEnd(); ++fifi) {
			if (FiletoDelete.baseName() == fifi->baseName()) {
				QFile::remove(fifi->absoluteFilePath());
			}
		}
		QFile::remove(FiletoDelete.absoluteFilePath());
	}
	else {
		QDir TmpDir =QFileInfo(path).absoluteDir();
		if (!TmpDir.exists()) TmpDir.mkpath(".");
	}
	simstring::ngram_generator gen(3, false);
	simstring::writer_base<std::wstring> dbw(gen, path.toStdString());
	QStringList NameList;
	{
		QFile InputFile(":/DataSources/ISO3166-1.xml");
		InputFile.open(QIODevice::ReadOnly);
		bool CountryFound = false;
		bool NameFound = false;
		QXmlStreamReader xml(&InputFile);
		while (!xml.atEnd() && !xml.hasError()) {
			xml.readNext();
			if (xml.isStartElement()) {
				if (xml.name() == "Country") {
					if (CountryFound) { dbw.close(); return; }
					CountryFound = true;
				}
				else if (CountryFound && xml.name() == "Name") {
					if (NameFound) { dbw.close(); return; }
					NameFound = true;
				}
			}
			else if (xml.isEndElement()) {
				if (xml.name() == "Country") {
					if (!CountryFound || NameFound) { dbw.close(); return; }
					CountryFound = false;
				}
				else if (xml.name() == "Name") {
					if (!NameFound) { dbw.close(); return; }
					NameFound = false;
				}
			}
			else if (xml.isCharacters() && NameFound) {
				NameList = xml.text().toString().split("#,#", QString::SkipEmptyParts);
				for (auto nli = NameList.constBegin(); nli != NameList.constEnd(); ++nli) dbw.insert(nli->trimmed().toLower().toStdWString());
			}
		}
		if (xml.hasError()) { dbw.close(); return; }
		InputFile.close();
	}
	dbw.close();
	return;
}