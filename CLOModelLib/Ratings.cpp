#include "Ratings.h"
#include "Private/Ratings_p.h"
#include <numeric>
#include <QRegularExpression>
#include <QDate>
#ifndef NO_BLOOMBERG
#include <QbbgReferenceDataRequest.h>
#include <QBbgSecurity.h>
#include <QBbgManager.h>
#include <QBbgReferenceDataResponse.h>
#include <QBbgRequestGroup.h>
#include <QVariant>
#endif
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#endif
#include "InternalItems.h"
DEFINE_PUBLIC_COMMONS(Ratings)
DEFINE_PUBLIC_COMMONS_COPY(Ratings)
Ratings::~Ratings()
{

}
RatingsPrivate::~RatingsPrivate()
{

}
RatingsPrivate::RatingsPrivate(Ratings *q)
	:BackwardInterfacePrivate(q)
{
    std::fill(std::begin(m_ratings), std::end(m_ratings), Ratings::RatingValue::NR);
    std::fill(std::begin(m_watch), std::end(m_watch), Ratings::CreditWatch::Stable);
}
RatingsPrivate::RatingsPrivate(Ratings *q,const RatingsPrivate& other)
	:BackwardInterfacePrivate(q,other)
{
    std::copy(std::begin(other.m_ratings), std::end(other.m_ratings), std::begin(m_ratings));
    std::copy(std::begin(other.m_watch), std::end(other.m_watch), std::begin(m_watch));
}
Ratings::Ratings(RatingsPrivate *d, const Ratings& other)
	:BackwardInterface(d,other)
{}
Ratings& Ratings::operator=(const Ratings& other){
	Q_D(Ratings);
	BackwardInterface::operator=(other);
    std::copy(std::begin(other.d_func()->m_ratings), std::end(other.d_func()->m_ratings), std::begin(d->m_ratings));
    std::copy(std::begin(other.d_func()->m_watch), std::end(other.d_func()->m_watch), std::begin(d->m_watch));
	return *this;
}
Ratings::Ratings(RatingsPrivate *d)
	:BackwardInterface(d)
{
    
}

bool Ratings::operator==(const Ratings& other) const
{
    return compare(other, CompareOption::CompareAll);
}

bool Ratings::operator!=(const Ratings& other) const
{
    return !operator==(other);
}

bool Ratings::compare(const Ratings& other, CompareOption opt) const
{
    Q_D(const Ratings);
    bool result = true;
    if(opt & CompareOption::CompareRating)
        result = result && std::equal(std::begin(d->m_ratings), std::end(d->m_ratings), std::begin(other.d_func()->m_ratings));
    if (opt & CompareOption::CompareWatch)
        result = result && std::equal(std::begin(d->m_watch), std::end(d->m_watch), std::begin(other.d_func()->m_watch));
    return result;
}

bool Ratings::compare(const Ratings& other, CompareOption opt, RatingAgencies agencies) const
{
    Q_D(const Ratings);
    bool result = true;
    if (opt & CompareOption::CompareRating){
        int counter = 0;
        for (auto i = std::begin(d->m_ratings); result && i != std::end(d->m_ratings); ++i, ++counter) {
            if (agencies & static_cast<RatingAgency>(1 << counter))
                result = result && (*i == other.d_func()->m_ratings[counter]);
        }
    }  
    if (opt & CompareOption::CompareWatch){
        int counter = 0;
        for (auto i = std::begin(d->m_watch); result && i != std::end(d->m_watch); ++i, ++counter) {
            if (agencies & static_cast<RatingAgency>(1 << counter))
                result = result && (*i == other.d_func()->m_watch[counter]);
        }
    }
    return result;
}

QString Ratings::agencyName(RatingAgency ag)
{
    for (qint32 i = 0;; ++i) {
        Q_ASSERT(i <= CountRatingAcencies);
        if (static_cast<qint32>(ag) == (1 << i)) 
            return RatingsPrivate::m_AgencyName[i];
    }
    Q_UNREACHABLE();
    return RatingsPrivate::m_AgencyName[0];
}

Ratings::RatingBucket Ratings::getBucket(RatingValue val)
{
    if (val == RatingValue::AAA)
        return RatingBucket::AAA;
    else if (val == RatingValue::NR)
        return RatingBucket::NR;
   /* else if (val == RatingValue::D)
        return RatingBucket::D;*/
    else
        return static_cast<RatingBucket>((((static_cast<qint16>(val)-2) / 3) * 3) + 3);
}

QString Ratings::RatingValueString(RatingBucket val, RatingAgency agencySyntax)
{
    return RatingValueString(static_cast<RatingValue>(static_cast<qint16>(val)), agencySyntax);
}
QString Ratings::RatingValueString(RatingBucket val)
{
    return RatingValueString(static_cast<RatingValue>(static_cast<qint16>(val)));
}

QString Ratings::RatingValueString(RatingValue val, RatingAgency agencySyntax )
{
    for (qint32 i = 0;; ++i) {
        Q_ASSERT(i <= CountRatingAcencies);
        if (static_cast<qint32>(agencySyntax) == (1 << i))
            return RatingsPrivate::m_ratingSyntax[i][static_cast<qint16>(val)];
    }
    Q_UNREACHABLE();
    return RatingsPrivate::m_ratingSyntax[0][static_cast<qint16>(val)];
}

QString Ratings::RatingValueString(RatingValue val)
{
    const QString BloombergCompositeSyntax[static_cast<qint16>(Ratings::RatingValue::Dm) + 1] =
    {
        "NR",
        "AAA",
        "AA+",
        "AA",
        "AA-",
        "A+",
        "A",
        "A-",
        "BBB+",
        "BBB",
        "BBB-",
        "BB+",
        "BB",
        "BB-",
        "B+",
        "B",
        "B-",
        "CCC+",
        "CCC",
        "CCC-",
        "CC+", 
        "CC",
        "CC-", 
        "C+", 
        "C",
        "C-", 
        "DDD+", 
        "DDD", 
        "DDD-", 
        "DD+", 
        "DD", 
        "DD-", 
        "D+", 
        "D"
        "D-", 
    };
    return BloombergCompositeSyntax[static_cast<qint16>(val)];

}

void Ratings::setRating(RatingValue val, RatingAgency ag, CreditWatch wtch)
{
    setRating(val,ag);
    setWatch(wtch, ag);
}

bool Ratings::setRating(const QString& val, RatingAgency ag)
{
    qint32 agencyIndex;

    for (agencyIndex = 0;; ++agencyIndex) {
        Q_ASSERT(agencyIndex <= CountRatingAcencies);
        if (static_cast<qint32>(ag) == (1 << agencyIndex))
            break;
    }
    const  auto agencySyntax = RatingsPrivate::m_ratingSyntax[agencyIndex];
    QRegularExpression syntaxCheck;
    syntaxCheck.setPatternOptions(QRegularExpression::CaseInsensitiveOption | QRegularExpression::DontCaptureOption);
    for (int i = static_cast<qint16>(RatingValue::AAA); i <= static_cast<qint16>(RatingValue::Dm); ++i){
        if (!agencySyntax[i].isEmpty()) {
            syntaxCheck.setPattern(
                "(^|[^" + QRegularExpression::escape(RatingsPrivate::m_reservedChars[agencyIndex]) + "])"
                + QRegularExpression::escape(agencySyntax[i])
                + "($|[^" + QRegularExpression::escape(RatingsPrivate::m_reservedChars[agencyIndex]) + "])"
                );
            Q_ASSERT(syntaxCheck.isValid());
            if (syntaxCheck.match(val).hasMatch()){
                    setRating(static_cast<RatingValue>(i), ag);
                    break;
            }
        }
        if (i == static_cast<qint16>(RatingValue::Dm)) {
            setRating(RatingValue::NR, ag, Stable);
            return false;
        }
    }
    if (val.indexOf("*-", 0, Qt::CaseInsensitive) >= 0)
        setWatch(Negative, ag);
    else if (val.indexOf("*+", 0, Qt::CaseInsensitive) >= 0)
        setWatch(Positive, ag);
    else
        setWatch(Stable, ag);
    return true;
}

void Ratings::setRating(RatingValue val, RatingAgency ag)
{
    Q_D(Ratings);
    for (qint32 i = 0;;++i){
        if (i > CountRatingAcencies)
            i = i;
        Q_ASSERT(i <= CountRatingAcencies);    
        if (static_cast<qint32>(ag) == (1 << i)) {
            d->m_ratings[i] = val;
            return;
        }
    }
    
}

void Ratings::setWatch(CreditWatch wtch, RatingAgency ag)
{
    Q_D(Ratings);
    for (qint32 i = 0;; ++i) {
        Q_ASSERT(i <= CountRatingAcencies);
        if (static_cast<qint32>(ag) == (1 << i)) {
            d->m_watch[i] = wtch;
            return;
        }
    }
}

Ratings::RatingValue Ratings::getRating(RatingAgency ag) const
{
    Q_D(const Ratings);
    for (qint32 i = 0;; ++i) {
        Q_ASSERT(i <= CountRatingAcencies);
        if (static_cast<qint32>(ag) == (1 << i))
            return  d->m_ratings[i];
    }
    Q_UNREACHABLE();
    return  d->m_ratings[0];
}

Ratings::CreditWatch Ratings::getWatch(RatingAgency ag) const
{
    Q_D(const Ratings);
    for (qint32 i = 0;; ++i) {
        Q_ASSERT(i <= CountRatingAcencies);
        if (static_cast<qint32>(ag) == (1 << i))
            return  d->m_watch[i];
    }
    Q_UNREACHABLE();
    return  d->m_watch[0];
}

Ratings::RatingValue Ratings::highestRating() const
{
    return ratingAtRank(0);
}

Ratings::RatingValue Ratings::lowestRating() const
{
    return ratingAtRank(CountRatingAcencies-1);
}

Ratings::RatingAgencies Ratings::highestAgency() const
{
    return agencyAtRank(0);
}

Ratings::RatingAgencies Ratings::lowestAgency() const
{
    return agencyAtRank(CountRatingAcencies-1);
}

Ratings::RatingValue Ratings::averageRating() const
{
    return averageRating(0.0);
}

Ratings::RatingValue Ratings::averageRating(double startingVal) const
{
    Q_D(const Ratings);
    if (numRatings() == 0)
        return RatingValue::NR;
    return static_cast<RatingValue>(static_cast<qint16>(std::ceil(
        std::accumulate(std::begin(d->m_ratings), std::end(d->m_ratings), startingVal, [](double sum, RatingValue val)->double {return qMax(sum, sum + static_cast<double>(val)); })
        / static_cast<double>(numRatings())
        )));
}

int Ratings::notchesToAverageUpgrade() const
{
    if (numRatings() == 0)
        return 0;
    const RatingValue currAvg = averageRating();
    if (currAvg == RatingValue::AAA)
        return 0;
    int result = 1;
    while (averageRating(-result) == currAvg)
        ++result;
    return result;
}

int Ratings::notchesToAverageDowngrade() const
{
    if (numRatings() == 0)
        return 0;
    const RatingValue currAvg = averageRating();
    if (currAvg == RatingValue::Dm)
        return 0;
    int result = 1;
    while (averageRating(result) == currAvg)
        ++result;
    return result;
}

int Ratings::notchesToAverageUpgradeBucket() const
{
    if (numRatings() == 0)
        return 0;
    const RatingBucket currAvg = getBucket(averageRating());
    if (currAvg == RatingBucket::AAA)
        return 0;
    int result = 1;
    while (getBucket(averageRating(-result)) == currAvg)
        ++result;
    return result;
}

int Ratings::notchesToAverageDowngradeBucket() const
{
    if (numRatings() == 0)
        return 0;
    const RatingBucket currAvg = getBucket(averageRating());
    if (currAvg == RatingBucket::D)
        return 0;
    int result = 1;
    while (getBucket(averageRating(result)) == currAvg)
        ++result;
    return result;
}

Ratings::RatingValue Ratings::ratingAtRank(int rnk) const
{
    Q_D(const Ratings);
    decltype(d->m_ratings) sortedRatings;
    std::copy(std::begin(d->m_ratings), std::end(d->m_ratings), std::begin(sortedRatings));
    std::sort(std::begin(sortedRatings), std::end(sortedRatings));
    auto result = std::begin(sortedRatings);
    while (*result == RatingValue::NR) {
        ++result;
        if (result == std::end(sortedRatings))
            break;
    }
    while (rnk>0 && result != std::end(sortedRatings)) {
        ++result;
        --rnk;
    }
    if (result == std::end(sortedRatings))
        --result;
    return *result;
}

Ratings::RatingValue Ratings::ratingAtRankNoDuplicate(int rnk) const
{
    Q_D(const Ratings);
    decltype(d->m_ratings) sortedRatings;
    std::copy(std::begin(d->m_ratings), std::end(d->m_ratings), std::begin(sortedRatings));
    std::sort(std::begin(sortedRatings), std::end(sortedRatings));
    {
        decltype(d->m_ratings) filteredRatings;
        std::fill(std::begin(filteredRatings), std::end(filteredRatings), Ratings::RatingValue::NR);
        std::unique_copy(std::begin(sortedRatings), std::end(sortedRatings), std::begin(filteredRatings));
        std::move(std::begin(filteredRatings), std::end(filteredRatings), std::begin(sortedRatings));
    }
    std::sort(std::begin(sortedRatings), std::end(sortedRatings));
    auto result = std::begin(sortedRatings);
    while (*result == RatingValue::NR) {
        ++result;
        if (result == std::end(sortedRatings))
            break;
    }
    while (rnk > 0 && result != std::end(sortedRatings)) {
        ++result;
        --rnk;
    }
    if (result == std::end(sortedRatings))
        --result;
    return *result;
}

Ratings::RatingAgencies Ratings::agencyAtRank(int rnk) const
{
    RatingAgencies result(Ratings::RatingAgency::Invalid);
    Q_D(const Ratings);
    const auto ratingToFind = ratingAtRankNoDuplicate(rnk);
    for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i) {
        if (d->m_ratings[i] == ratingToFind)
            result |= static_cast<RatingAgency>(1 << i);
    }
    return result;
}

bool Ratings::downloadRatings(const QBbgLib::QBbgSecurity& sec)
{
    bool oneRatingFound = false;
#ifndef NO_BLOOMBERG
    if (!sec.isValid())
        return false;
    Ratings bachupRating(*this);
    reset();
    using namespace QBbgLib;
    QBbgRequestGroup allRq;
    QBbgReferenceDataRequest singleRq;
    singleRq.setSecurity(sec);
    for (int i = 0; i < CountRatingAcencies; ++i)
    {
        singleRq.setField(RatingsPrivate::m_ratingFields[i]);
        singleRq.setID(i + 100);
        allRq.addRequest(singleRq);
    }
    QBbgManager manager;
    const auto& allres= manager.processRequest(allRq);
    
    for (int i = 0; i < CountRatingAcencies; ++i) {
        Q_ASSERT(allres.contains(i + 100));
        if (!allres.value(i + 100)->hasErrors()) {
            if (setRating(dynamic_cast<const QBbgReferenceDataResponse*>(allres.value(i + 100))->value().toString(), static_cast<RatingAgency>(1 << i)))
                oneRatingFound = true;
        }
    }
    if (!oneRatingFound)
        operator=(bachupRating);
#endif // !NO_BLOOMBERG
    return oneRatingFound;
}

bool Ratings::downloadRatings(const QString& name, const QString& bbgExtension)
{
    return downloadRatings(QBbgLib::QBbgSecurity(name, QBbgLib::QBbgSecurity::stringToYellowKey(bbgExtension)));
}

bool Ratings::downloadRatings(const QBbgLib::QBbgAbstractResponse * const res)
{
    using namespace QBbgLib;
    if(res->hasErrors())
        return false;
    auto refResponse = dynamic_cast<const QBbgReferenceDataResponse*>(res);
    if (!refResponse)
        return false;
    for (auto i = std::begin(RatingsPrivate::m_ratingFields); i != std::end(RatingsPrivate::m_ratingFields);++i){
        QString currentHeader = refResponse->header().toUpper();
        currentHeader.replace(QRegularExpression("\\s+"), "_");
        if (currentHeader.indexOf(*i, 0, Qt::CaseInsensitive) >= 0) {
            if(refResponse->hasValue()){
                if(!refResponse->value().toString().isEmpty()){
                    return setRating(refResponse->value().toString(), static_cast<RatingAgency>(1 << std::distance(std::begin(RatingsPrivate::m_ratingFields), i)));
                }
            }
        }
    }
    return false;
}

bool Ratings::saveToDatabase(const QString& isin) const
{
#ifndef NO_DATABASE
    if (isin.isEmpty())
        return false;
    QMutexLocker dbLocker(&Db_Mutex);
    QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
        db.setDatabaseName(
            "Driver={" + GetFromConfig("Database", "Driver")
            + "}; "
            + GetFromConfig("Database", "DataSource")
            );

    }
    bool DbOpen = db.isOpen();
    if (!DbOpen) DbOpen = db.open();
    if (DbOpen) {
        for (int i = 0; i != CountRatingAcencies; ++i) {
            const RatingValue currRtg = getRating(static_cast<RatingAgency>(1 << i));
            QSqlQuery CheckBondExistQuery(db);
            CheckBondExistQuery.setForwardOnly(true);
            CheckBondExistQuery.prepare("{CALL " + GetFromConfig("Database", "AddRatingHistoryStoredProc") + "}");
            CheckBondExistQuery.bindValue(":isin", isin);
            CheckBondExistQuery.bindValue(":agency", agencyName(static_cast<RatingAgency>(1 << i)));
            CheckBondExistQuery.bindValue(":rating", static_cast<qint32>(currRtg));
            CheckBondExistQuery.bindValue(":watch", static_cast<qint8>(getWatch(static_cast<RatingAgency>(1 << i))));
            if (!CheckBondExistQuery.exec())
                return false;
        }
        return true;
    }
#endif
    return false;
}

bool Ratings::getFromDatabase(const QString& isin)
{
    return getFromDatabase(isin, QDate::currentDate());
}

bool Ratings::getFromDatabase(const QString& isin, const QDate& refDate)
{
    reset();
#ifndef NO_DATABASE
    QMutexLocker dbLocker(&Db_Mutex);
    QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
        db.setDatabaseName(
            "Driver={" + GetFromConfig("Database", "Driver")
            + "}; "
            + GetFromConfig("Database", "DataSource")
            );
    }
    bool DbOpen = db.isOpen();
    if (!DbOpen) DbOpen = db.open();
    if (DbOpen) {
        QSqlQuery getRatingQuery(db);
        getRatingQuery.setForwardOnly(true);
        getRatingQuery.prepare("{CALL " + GetFromConfig("Database", "GetRatingStoredProc") + "}");
        getRatingQuery.bindValue(":isin", isin);
        getRatingQuery.bindValue(":referenceDate", refDate.toString(Qt::ISODate));
        if (!getRatingQuery.exec())
            return false;
        Q_D(const Ratings);
        bool oneRatingFound=false;
        while (getRatingQuery.next()){
            oneRatingFound = true;
            const QSqlRecord getRatingRecord = getRatingQuery.record();
            if (std::find(std::begin(d->m_AgencyName), std::end(d->m_AgencyName), getRatingRecord.value("RatingAgency").toString()) == std::end(d->m_AgencyName))
                continue;
            setRating(
                static_cast<RatingValue>(getRatingRecord.value("Rating").toInt()),
                static_cast<RatingAgency>(1 << std::distance(std::begin(d->m_AgencyName), std::find(std::begin(d->m_AgencyName), std::end(d->m_AgencyName), getRatingRecord.value("RatingAgency").toString()))),
                static_cast<CreditWatch>(getRatingRecord.value("RatingWatch").toInt())
                );
        }
        return oneRatingFound;
    }
#endif
    return false;
}

int Ratings::numRatings() const
{
    Q_D(const Ratings);
    return std::count_if(std::begin(d->m_ratings), std::end(d->m_ratings), [](RatingValue val)->bool {return val != RatingValue::NR; });
}

void Ratings::reset()
{
    Q_D(Ratings);
    std::fill(std::begin(d->m_ratings), std::end(d->m_ratings), Ratings::RatingValue::NR);
    std::fill(std::begin(d->m_watch), std::end(d->m_watch), Ratings::CreditWatch::Stable);
}

const QString RatingsPrivate::m_ratingFields[Ratings::CountRatingAcencies] = { "RTG_SP", "RTG_MOODY", "RTG_FITCH", "RTG_DBRS" };
const QString RatingsPrivate::m_AgencyName[Ratings::CountRatingAcencies] = { "S&P", "Moody's", "Fitch", "DBRS" };
const QString RatingsPrivate::m_reservedChars[Ratings::CountRatingAcencies] = { "WABC+-", "WABCa+-", "WABC+-", "WABCHL" };
const QString RatingsPrivate::m_ratingSyntax[Ratings::CountRatingAcencies][static_cast<qint16>(Ratings::RatingValue::Dm) + 1] = {
    { //S&P
        "NR",
        "AAA",
        "AA+",
        "AA",
        "AA-",
        "A+",
        "A",
        "A-",
        "BBB+",
        "BBB",
        "BBB-",
        "BB+",
        "BB",
        "BB-",
        "B+",
        "B",
        "B-",
        "CCC+",
        "CCC",
        "CCC-",
        "", // Does not exist
        "CC",
        "", // Does not exist
        "", // Does not exist
        "C", 
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "D"
        "", // Does not exist
    }, 
    { // Moody's
        "NR",
        "Aaa",
        "Aa1",
        "Aa2",
        "Aa3",
        "A1",
        "A2",
        "A3",
        "Baa1",
        "Baa2",
        "Baa3",
        "Ba1",
        "Ba2",
        "Ba3",
        "B1",
        "B2",
        "B3",
        "Caa1",
        "Caa2",
        "Caa3",
        "", // Does not exist
        "Ca",
        "", // Does not exist
        "", // Does not exist
        "C",
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "D"
        "", // Does not exist
    }, 
    { // Fitch
        "NR",
        "AAA",
        "AA+",
        "AA",
        "AA-",
        "A+",
        "A",
        "A-",
        "BBB+",
        "BBB",
        "BBB-",
        "BB+",
        "BB",
        "BB-",
        "B+",
        "B",
        "B-",
        "CCC+",
        "CCC",
        "CCC-",
        "CC+",
        "CC",
        "CC-",
        "C+",
        "C",
        "C-",
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "D"
        "", // Does not exist
    },
    { //DBRS
        "NR",
        "AAA",
        "AAH",
        "AA",
        "AAL",
        "AH",
        "A",
        "AL",
        "BBBH",
        "BBB",
        "BBBL",
        "BBH",
        "BB",
        "BBL",
        "BH",
        "B",
        "BL",
        "CCCH",
        "CCC",
        "CCCL",
        "CCH",
        "CC",
        "CCL",
        "CH",
        "C",
        "CL",
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "", // Does not exist
        "D"
        "", // Does not exist
    }
};

CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Ratings& flows)
{
    stream << static_cast<qint32>(flows.d_func()->m_ratings.size());
    std::for_each(std::begin(flows.d_func()->m_ratings), std::end(flows.d_func()->m_ratings), [&stream](Ratings::RatingValue val)->void {stream << static_cast<qint16>(val); });
    stream << static_cast<qint32>(flows.d_func()->m_watch.size());
    std::for_each(std::begin(flows.d_func()->m_watch), std::end(flows.d_func()->m_watch), [&stream](Ratings::CreditWatch val)->void {stream << static_cast<qint8>(val); });
    return stream;
}

CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Ratings& flows)
{
    return flows.LoadOldVersion(stream);
}
QDataStream& Ratings::LoadOldVersion(QDataStream& stream)
{
    Q_D(Ratings);
    reset();
    qint32 tempSize;
    qint16 tempRating;
    qint8 tempWatch;
    stream >> tempSize;
    for (int i = 0; i < tempSize;++i){
        stream >> tempRating;
        d->m_ratings[i] = static_cast<RatingValue>(tempRating);
    }
    stream >> tempSize;
    for (int i = 0; i < tempSize; ++i) {
        stream >> tempWatch;
        d->m_watch[i] = static_cast<CreditWatch>(tempWatch);
    }
    return stream;
}