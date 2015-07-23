#include "CommonFunctions.h"
#include <QDate>
#include <QRegularExpression>
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
#include "Private/InternalItems.h"
#include <QLocale>
int MonthDiff(const QDate& FutureDte,const QDate& PresentDte){
	int Result;
	Result=(FutureDte.year()-PresentDte.year())*12;
	Result+=FutureDte.month()-PresentDte.month();
	return Result;
}

double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, const DayCountVector& Daycount) {
	if (Dte.size() != Flws.size() || Dte.isEmpty() || Daycount.IsEmpty()) return 0.0;
	double Result=Flws.at(0);
	double DiscountFactor=1.0;
	for(int i=1;i<Dte.size();i++){
		DiscountFactor *= 1.0 + deannualiseCoupon(Interest, Dte.at(i - 1), Dte.at(i), Daycount.GetValue(Dte.at(i)));
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
		DiscountFactor *= 1.0 + deannualiseCoupon(AdjInterest.GetValue(Dte.at(i)), Dte.at(i - 1), Dte.at(i), Daycount.GetValue(Dte.at(i)));
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
	QDir dir(dirName);
	if (dir.exists(dirName)) {
        const auto allInfos = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
        for (auto info = allInfos.constBegin(); info != allInfos.constEnd(); ++info)
        {
			if (info->isDir()) {
                if (!removeDir(info->absoluteFilePath()))
                    return false;
			}
			else {
                if (!QFile::remove(info->absoluteFilePath()))
                    return false;
			}
		}
        if (!dir.rmdir(dirName))
            return false;
	}
	return true;
}

double deannualiseCoupon(double AnnualCoupon /*Annualised Coupon*/, QDate PrevIPD /*Interesty start accrual date*/, QDate CurrIPD /*Interesty end accrual date*/, DayCountConvention DayCount) {
	
	if (AnnualCoupon == 0.0) return 0.0; //If the annual coupon is 0 there is no need to adjust it
	//Check the CompoundShift+1 th bit, if it's set calculate the interest as ((1+r)^t)-1
	if (DayCount& (1 << CompoundShift)) //Compounded
        return qPow(1.0 + AnnualCoupon, getTimeFactor(PrevIPD,CurrIPD,DayCount)) - 1.0;
	//Check the CompoundShift+2 th bit, if it's set calculate the interest as (e^(r*t))-1
	else if (DayCount& (1 << (1 + CompoundShift))) //Continuously Compounded
        return qExp(AnnualCoupon*getTimeFactor(PrevIPD, CurrIPD, DayCount)) - 1.0;
	//Otherwise calculate the interest as r*t
	else //Simple
        return AnnualCoupon * getTimeFactor(PrevIPD, CurrIPD, DayCount);
}

double annualiseCoupon(double periodicCoupon, QDate PrevIPD, QDate CurrIPD, DayCountConvention DayCount)
{
    if (periodicCoupon == 0.0) return 0.0; //If the annual coupon is 0 there is no need to adjust it
    const double timefactor = getTimeFactor(PrevIPD, CurrIPD, DayCount);
    if (timefactor == 0.0)
        return periodicCoupon;
    //Check the CompoundShift+1 th bit, if it's set calculate the interest as ((1+r)^t)-1
    if (DayCount& (1 << CompoundShift)) //Compounded
        return qPow(1.0 + periodicCoupon, 1.0 / timefactor) - 1.0;
    //Check the CompoundShift+2 th bit, if it's set calculate the interest as (e^(r*t))-1
    else if (DayCount& (1 << (1 + CompoundShift))) //Continuously Compounded
        return qExp(periodicCoupon / timefactor) - 1.0;
    //Otherwise calculate the interest as r*t
    else //Simple
        return periodicCoupon / timefactor;
}

bool IsHoliday(const QDate& a/*, const QString& CountryCode*/) {
	//TODO check for bank holidays
	return a.dayOfWeek() >= 6;
}
bool ValidDayCount(qint16 a) {
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
void PrintToTempFile(const QString& TempFileName, const QString& Message, bool PrintTime)
{
    QFile TempFile("C:/Temp/" + TempFileName + ".log");
    if (!TempFile.open(QIODevice::Append | QIODevice::Text)) return;
    QTextStream  TempWrite(&TempFile);
    TempWrite << (PrintTime ? QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm ") : QString()) + Message + '\n';
    TempFile.close();
}
QString GetFromConfig(const QString& Domain, const QString& Field, const QString& DefaultValue)
{
    QFile Source(":/Configs/GlobalConfigs.xml");
    Source.open(QIODevice::ReadOnly);
    QXmlStreamReader xml(&Source);
    bool DomainFound = false;
    bool FieldFound = false;
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == Domain) {
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

