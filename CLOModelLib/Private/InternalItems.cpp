#include "Private/InternalItems.h"
#include <QRegularExpression>
#include <QStringList>
#include <QStack>
#include <QFile>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QDateTime>
#include "CommonFunctions.h"
#ifdef _DEBUG
#include <QSqlQuery>
QString getLastExecutedQuery(const QSqlQuery& query)
{
    QString str = query.lastQuery();
    QMapIterator<QString, QVariant> it(query.boundValues());
    while (it.hasNext()) {
        it.next();
        str.replace(it.key(), it.value().toString());
    }
    return str;
}
#else
QString getLastExecutedQuery(const QSqlQuery& query)
{return QString();}
#endif // !_DEBUG


#ifndef NO_DATABASE
QMutex Db_Mutex;
#endif



const QString LoansPropertiesToSearch[] = { "Issuer", "Facility" };
QString Commarize(double num, unsigned int precision)
{
    return QLocale().toString(num, 'f', precision);
    /*QString Result=QString::number(num,'f',precision);
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
    return Commarized;*/
}
QString InfixToPostfix(const QString& a)
{
    if (a.isEmpty()) return QString();
    QString ops("-+/*");
    {
        //Prevent Operators at the beginning of the string
        QRegularExpression CheckDoubleOperator("^[" + ops + ']', QRegularExpression::CaseInsensitiveOption);
        if (CheckDoubleOperator.match(a).hasMatch()) 
            return QString();
        //Prevent Unmatched parenthesis
        if (a.count('(') != a.count(')')) 
            return QString();
        //Prevent two operators one after another
        CheckDoubleOperator.setPattern('[' + ops + "]\\s*[" + ops + ']');
        if (CheckDoubleOperator.match(a).hasMatch())
            return QString();
        //Prevent operator just after NOT
        CheckDoubleOperator.setPattern("![" + ops + ']');
        if (CheckDoubleOperator.match(a).hasMatch()) 
            return QString();
        ops += '!';

        //Prevent operators just before close of parenthesis
        CheckDoubleOperator.setPattern('[' + ops + "]\\)");
        if (CheckDoubleOperator.match(a).hasMatch())
            return QString();
        //Prevent Operators at the end of the string
        CheckDoubleOperator.setPattern('[' + ops + "]$");
        if (CheckDoubleOperator.match(a).hasMatch())
            return QString();
    }
    QRegularExpression CheckValidNumber("^(?:\\d+|F|T)$", QRegularExpression::CaseInsensitiveOption);
    QString Spaced("");
    for (auto i = a.constBegin(); i != a.constEnd(); ++i) {
        if (i->isSpace()) continue;
        if (Spaced.isEmpty()) {
            Spaced.append(*i);
        }
        else {
            if (!(i->isDigit() && ((i - 1)->isDigit()))) Spaced.append(' ');
            Spaced.append(*i);
        }
    }
    QStack<qint32> s;
    QString sb("");
    const QStringList parts = Spaced.split(QRegularExpression("\\s"));
    for (auto token = parts.constBegin(); token != parts.constEnd();++token)
    {
        int idx = ops.indexOf(token->at(0));
        if (idx != -1 && token->size() == 1) {
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
        else if (token->at(0) == '(') {
            s.push(-2);
        }
        else if (token->at(0) == ')') {
            while (s.top() != -2)
                sb.append(ops.at(s.pop())).append(' ');
            s.pop();
        }
        else {
            if (!CheckValidNumber.match(*token).hasMatch())
                return QString();
            sb.append(*token).append(' ');
        }
    }
    while (!s.isEmpty()) {
        sb.append(ops.at(s.pop())).append(' ');
    }
    return sb.trimmed();
}
QString NormaliseTriggerStructure(QString a)
{
    a = a.toLower();
    a.replace("nand", "/");
    a.replace("and", "*");
    a.replace("nor", "-");
    a.replace("or", "+");
    a.replace("not", "!");
    a.replace("^", "!");
    a.replace(QRegularExpression("&?&"), "*");
    a.replace(QRegularExpression("|?|"), "+");
    a.replace("false", "f");
    a.replace("true", "t");
    a.replace(QRegularExpression("\\s"), "");
    return a.toUpper();
}
int NumberOfSetBits(quint32 i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

double getTimeFactor(QDate PrevIPD, QDate CurrIPD, DayCountConvention DayCount)
{
    if (PrevIPD.isNull() || CurrIPD.isNull()) return 1.0; //If dates are invalid return 1
    if (PrevIPD > CurrIPD) { //If start accrual is after end accrual invert the two dates
        QDate Temp = PrevIPD;
        PrevIPD = CurrIPD;
        CurrIPD = Temp;
    }
    double TimeFactor=1.0; //This is the factor by which the coupon must be adjusted
    int CurrOffset = 0;
    int PrevOffset = 0;
    const QDate PrevIPDeom(PrevIPD.year(), PrevIPD.month(), PrevIPD.daysInMonth());
    const QDate CurrIPDeom(CurrIPD.year(), CurrIPD.month(), CurrIPD.daysInMonth());
    /*
    DayCount is a 16 bit integer;
    the bit 1 to CompoundShift contain the day count convention Bloomberg code (see DAY COUNT CODES - MBS <HELP>)
    if the interest is accrued according to the compounding formula the bit CompoundShift+1 is set
    if the interest is accrued according to the continuous compounding formula the bit CompoundShift+2 is set
    if the interest is accrued according to the simple compounding formula neither bit is set
    */
    // check the first CompoundShift bits of the DayCount
    switch (static_cast<DayCountConvention>(static_cast<qint16>(DayCount)& (((1 << CompoundShift) - 1)))) {
    case DayCountConvention::FiSIA30360:
        if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30); //if start accrue falls on the 31st move it to the 30th
        if (CurrIPD.day() == 31 && PrevIPD.day() == 30) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30); //if end accrue falls on the 31st and start accrue falls on the 30th move end accrue to the 30th
        //fall through next case
    case DayCountConvention::FiSIA30360neom:
        if (PrevIPD.day() == 29 && PrevIPD.month() == 2 && QDate::isLeapYear(PrevIPD.year())) PrevOffset = 1;
        if (PrevIPD.day() == 28 && PrevIPD.month() == 2 && !QDate::isLeapYear(PrevIPD.year())) PrevOffset = 2;
        if (PrevOffset > 0 && CurrIPD.day() == 29 && CurrIPD.month() == 2 && QDate::isLeapYear(CurrIPD.year())) CurrOffset = 1;
        if (PrevOffset > 0 && CurrIPD.day() == 28 && CurrIPD.month() == 2 && !QDate::isLeapYear(CurrIPD.year())) CurrOffset = 2;
        TimeFactor = (
            (360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
            (30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
            static_cast<double>(CurrIPD.day() - PrevIPD.day() + CurrOffset - PrevOffset)
            ) / 360.0;
        break;
    case DayCountConvention::FiISMA30365:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiISMA30365neom:
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
        TimeFactor = (
            (360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
            (30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
            static_cast<double>(CurrIPD.day() - PrevIPD.day())
            ) / 365.0;
        break;
    case DayCountConvention::FiISMA30ACT:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiISMA30ACTneom:
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
        TimeFactor = (
            (360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
            (30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
            static_cast<double>(CurrIPD.day() - PrevIPD.day())
            ) / CurrIPD.daysInYear();
        break;
    case DayCountConvention::Fi30ACT:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::Fi30ACTneom:
        if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30); //if start accrue falls on the 31st move it to the 30th
        if (CurrIPD.day() == 31 && PrevIPD.day() == 30) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30); //if end accrue falls on the 31st and start accrue falls on the 30th move end accrue to the 30th
        // if the previous IPD was on the last day of Feb set an offset to simulate it happening on the 30th of Feb
        if (PrevIPD.day() == 29 && PrevIPD.month() == 2 && QDate::isLeapYear(PrevIPD.year())) PrevOffset = 1;
        if (PrevIPD.day() == 28 && PrevIPD.month() == 2 && !QDate::isLeapYear(PrevIPD.year())) PrevOffset = 2;

        TimeFactor = (
            (360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
            (30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
            static_cast<double>(CurrIPD.day() - PrevIPD.day() - PrevOffset)
            ) / CurrIPD.daysInYear();
        break;
    case DayCountConvention::Fi30365neom:
        if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30); //if start accrue falls on the 31st move it to the 30th
        if (CurrIPD.day() == 31 && PrevIPD.day() == 30) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30); //if end accrue falls on the 31st and start accrue falls on the 30th move end accrue to the 30th
        // if the previous IPD was on the last day of Feb set an offset to simulate it happening on the 30th of Feb
        if (PrevIPD.day() == 29 && PrevIPD.month() == 2 && QDate::isLeapYear(PrevIPD.year())) PrevOffset = 1;
        if (PrevIPD.day() == 28 && PrevIPD.month() == 2 && !QDate::isLeapYear(PrevIPD.year())) PrevOffset = 2;

        TimeFactor = (
            (360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
            (30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
            static_cast<double>(CurrIPD.day() - PrevIPD.day() - PrevOffset)
            ) / 365.0;
        break;
    case DayCountConvention::FiACTACT:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiACTACTneom:
        //difference between the dates divided by days in the end accrue year
        TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / CurrIPD.daysInYear();
        break;
    case DayCountConvention::FiACT364:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //difference between the dates divided by days in the end accrue year
        TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 364.0;
        break;
    case DayCountConvention::FiNLACT:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiNLACTneom:
        if (PrevIPD.month() == 2 && PrevIPD.day() > 28) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 28);
        if (CurrIPD.month() == 2 && CurrIPD.day() > 28) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 28);
        //difference between the dates divided by days in the end accrue year
        TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / CurrIPD.daysInYear();
        break;
    case DayCountConvention::FiNL360:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiNL360neom:
        if (PrevIPD.month() == 2 && PrevIPD.day() > 28) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 28);
        if (CurrIPD.month() == 2 && CurrIPD.day() > 28) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 28);
        //difference between the dates divided by days in the end accrue year
        TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 360.0;
        break;
    case DayCountConvention::FiNL365:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiNL365neom:
        if (PrevIPD.month() == 2 && PrevIPD.day() > 28) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 28);
        if (CurrIPD.month() == 2 && CurrIPD.day() > 28) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 28);
        //difference between the dates divided by days in the end accrue year
        TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 365.0;
        break;
    case DayCountConvention::FiACT360:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiACT360neom:
        //Difference between the dates divided 360
        TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 360.0;
        break;
    case DayCountConvention::FiACT365:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiACT365neom:
        //Difference between the dates divided 365
        TimeFactor = static_cast<double>(PrevIPD.daysTo(CurrIPD)) / 365.0;
        break;
    case DayCountConvention::FiISMA30360:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiISMA30360neom:
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
        TimeFactor = (
            (360.0*static_cast<double>(CurrIPD.year() - PrevIPD.year())) +
            (30.0*static_cast<double>(CurrIPD.month() - PrevIPD.month())) +
            static_cast<double>(CurrIPD.day() - PrevIPD.day())
            ) / 360.0;
        break;
    case DayCountConvention::FiGerman30360:
        if (PrevIPD == PrevIPDeom || CurrIPD == CurrIPDeom) {
            PrevIPD = PrevIPDeom;
            CurrIPD = CurrIPDeom;
        }
        //fall through next case
    case DayCountConvention::FiGerman30360neom:
    case DayCountConvention::N30360:
        if (PrevIPD.day() == 31) PrevIPD.setDate(PrevIPD.year(), PrevIPD.month(), 30); //if start accrue falls on the 31st move it to the 30th
        if (CurrIPD.day() == 31 && PrevIPD.day() == 30) CurrIPD.setDate(CurrIPD.year(), CurrIPD.month(), 30); //if end accrue falls on the 31st and start accrue falls on the 30th move end accrue to the 30th
        // if the previous IPD was on the last day of Feb set an offset to simulate it happening on the 30th of Feb
        if (PrevIPD.day() == 29 && PrevIPD.month() == 2 && QDate::isLeapYear(PrevIPD.year())) CurrOffset = 1;
        if (PrevIPD.day() == 28 && PrevIPD.month() == 2 && !QDate::isLeapYear(PrevIPD.year())) CurrOffset = 2;
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
            static_cast<double>(CurrIPD.day() - PrevIPD.day() - CurrOffset)
            ) / 360.0;
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
                + static_cast<double>(CurrIPD.year() - PrevIPD.year() - 1)
                //days start of year to end accrual date to end of year divided by days in that year (365 or 366 if it's leap)
                + (static_cast<double>(QDate(CurrIPD.year(), 1, 1).daysTo(CurrIPD)) / static_cast<double>(CurrIPD.daysInYear()))
                );
        }
        //Otherwise this is the same as ACT/365
        else return getTimeFactor(PrevIPD, CurrIPD, static_cast<DayCountConvention>((DayCount & ((~0) << CompoundShift)) | DayCountConvention::NACT365));
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
    return TimeFactor;
}


