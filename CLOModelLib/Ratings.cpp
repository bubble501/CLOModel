#include "Ratings.h"
#include "Private/Ratings_p.h"
#include <numeric>
#include <QRegExp>
#ifndef NO_BLOOMBERG
#include <QbbgReferenceDataRequest.h>
#include <QBbgSecurity.h>
#include <QBbgManager.h>
#include <QBbgReferenceDataResponse.h>
#include <QBbgRequestGroup.h>
#include <QVariant>
#endif
DEFINE_PUBLIC_COMMONS(Ratings)
DEFINE_PUBLIC_COMMONS_COPY(Ratings)
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
{}



Ratings::RatingBucket Ratings::getBucket(RatingValue val)
{
    if (val == RatingValue::AAA)
        return RatingBucket::AAA;
    else if (val == RatingValue::NR)
        return RatingBucket::NR;
    else
        return static_cast<RatingBucket>((((static_cast<qint16>(val)-2) / 3) * 3) + 3);
}

QString Ratings::RatingValueString(RatingBucket val, RatingAgency agencySyntax)
{
    return RatingValueString(static_cast<RatingValue>(static_cast<qint16>(val)), agencySyntax);
}

QString Ratings::RatingValueString(RatingValue val, RatingAgency agencySyntax )
{
    if (agencySyntax == RatingAgency::CountAgencies)
        return QString();
    return RatingsPrivate::m_ratingSyntax[static_cast<qint8>(agencySyntax)][static_cast<qint16>(val)];
}

void Ratings::setRating(RatingValue val, RatingAgency ag, CreditWatch wtch)
{
    if (ag == RatingAgency::CountAgencies)
        return;
    Q_D(Ratings);
    d->m_ratings[static_cast<qint8>(ag)] = val;
    d->m_watch[static_cast<qint8>(ag)] = wtch;
}

bool Ratings::setRating(const QString& val, RatingAgency ag)
{
    if (ag == RatingAgency::CountAgencies)
        return false;
    const auto & agencySyntax = RatingsPrivate::m_ratingSyntax[static_cast<qint8>(ag)];
    for (int i = static_cast<qint16>(RatingValue::AAA); i <= static_cast<qint16>(RatingValue::D); ++i){
        if (val.indexOf(QRegExp("[^ABC]" + agencySyntax[i] +"[^ABC]" ,Qt::CaseInsensitive))>=0){
            setRating(static_cast<RatingValue>(i), ag);
            break;
        }
        if (i == static_cast<qint16>(RatingValue::D)) {
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
    if (ag == RatingAgency::CountAgencies)
        return;
    Q_D(Ratings);
    d->m_ratings[static_cast<qint8>(ag)] = val;
}

void Ratings::setWatch(CreditWatch wtch, RatingAgency ag)
{
    if (ag == RatingAgency::CountAgencies)
        return;
    Q_D(Ratings);
    d->m_watch[static_cast<qint8>(ag)] = wtch;
}

Ratings::RatingValue Ratings::getRating(RatingAgency ag) const
{
    if (ag == RatingAgency::CountAgencies)
        return RatingValue::NR;
    Q_D(const Ratings);
    return  d->m_ratings[static_cast<qint8>(ag)];
}

Ratings::CreditWatch Ratings::getWatch(RatingAgency ag) const
{
    if (ag == RatingAgency::CountAgencies)
        return CreditWatch::Stable;
    Q_D(const Ratings);
    return  d->m_watch[static_cast<qint8>(ag)];
}

Ratings::RatingValue Ratings::highestRating() const
{
    return ratingAtRank(0);
}

Ratings::RatingValue Ratings::lowestRating() const
{
    return ratingAtRank(static_cast<qint8>(RatingAgency::CountAgencies) - 1);
}

Ratings::RatingAgency Ratings::highestAgency() const
{
    return agencyAtRank(0);
}

Ratings::RatingAgency Ratings::lowestAgency() const
{
    return agencyAtRank(static_cast<qint8>(RatingAgency::CountAgencies) - 1);
}

Ratings::RatingValue Ratings::averageRating() const
{
    Q_D(const Ratings);
    return static_cast<RatingValue>(static_cast<qint16>(std::ceil(
    std::accumulate(std::begin(d->m_ratings), std::end(d->m_ratings), 0.0, [](double sum, RatingValue val)->double {return qMax(sum, sum + static_cast<double>(val)); })
        /static_cast<double>(numRatings())
        )));
}

int Ratings::notchesToAverageUpgrade() const
{
    Q_D(const Ratings);
    int result = 1;
    const RatingValue currAvg = averageRating();
    while (
        static_cast<RatingValue>(static_cast<qint16>(std::ceil(
        std::accumulate(std::begin(d->m_ratings), std::end(d->m_ratings), -result, [](double sum, RatingValue val)->double {return qMax(sum, sum + static_cast<double>(val)); })
        / static_cast<double>(numRatings())
        ))) == currAvg
        )
        ++result;
    return result;
}

int Ratings::notchesToAverageDowngrade() const
{
    Q_D(const Ratings);
    int result = 1;
    const RatingValue currAvg = averageRating();
    while (
        static_cast<RatingValue>(static_cast<qint16>(std::ceil(
        std::accumulate(std::begin(d->m_ratings), std::end(d->m_ratings), result, [](double sum, RatingValue val)->double {return qMax(sum, sum + static_cast<double>(val)); })
        / static_cast<double>(numRatings())
        ))) == currAvg
        )
        ++result;
    return result;
}

int Ratings::notchesToAverageUpgradeBucket() const
{
    Q_D(const Ratings);
    int result = 1;
    const RatingBucket currAvg = getBucket(averageRating());
    while (
        getBucket(static_cast<RatingValue>(static_cast<qint16>(std::ceil(
        std::accumulate(std::begin(d->m_ratings), std::end(d->m_ratings), -result, [](double sum, RatingValue val)->double {return qMax(sum, sum + static_cast<double>(val)); })
        / static_cast<double>(numRatings())
        )))) == currAvg
        )
        ++result;
    return result;
}

int Ratings::notchesToAverageDowngradeBucket() const
{
    Q_D(const Ratings);
    int result = 1;
    const RatingBucket currAvg = getBucket(averageRating());
    while (
        getBucket(static_cast<RatingValue>(static_cast<qint16>(std::ceil(
        std::accumulate(std::begin(d->m_ratings), std::end(d->m_ratings), result, [](double sum, RatingValue val)->double {return qMax(sum, sum + static_cast<double>(val)); })
        / static_cast<double>(numRatings())
        )))) == currAvg
        )
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

Ratings::RatingAgency Ratings::agencyAtRank(int rnk) const
{
    Q_D(const Ratings);
    const auto ratingToFind = ratingAtRank(rnk);
    for (qint8 i = 0; i < static_cast<qint8>(Ratings::RatingAgency::CountAgencies); ++i) {
        if (d->m_ratings[i] == ratingToFind)
            return static_cast<RatingAgency>(i);
    }
    Q_UNREACHABLE();
    return Ratings::RatingAgency::CountAgencies;
}

bool Ratings::downloadRatings(const QBbgLib::QBbgSecurity& sec)
{
    bool oneRatingFound = false;
#ifndef NO_BLOOMBERG
    Ratings bachupRating(*this);
    reset();
    using namespace QBbgLib;
    QBbgRequestGroup allRq;
    QBbgReferenceDataRequest singleRq;
    singleRq.setSecurity(sec);
    for (int i = 0; i < static_cast<qint8>(RatingAgency::CountAgencies); ++i)
    {
        singleRq.setField(RatingsPrivate::m_ratingFields[i]);
        singleRq.setID(i + 100);
        allRq.addRequest(singleRq);
    }
    QBbgManager manager;
    const auto& allres= manager.processRequest(allRq);
    
    for (int i = 0; i < static_cast<qint8>(RatingAgency::CountAgencies); ++i){
        Q_ASSERT(allres.contains(i + 100));
        if (!allres.value(i + 100)->hasErrors()) {
            oneRatingFound = oneRatingFound || setRating(dynamic_cast<const QBbgReferenceDataResponse*>(allres.value(i + 100))->value().toString(), static_cast<RatingAgency>(i));
        }
    }
    if (!oneRatingFound)
        operator=(bachupRating);
#endif // !NO_BLOOMBERG
    return oneRatingFound;
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

const QString RatingsPrivate::m_ratingFields[static_cast<qint8>(Ratings::RatingAgency::CountAgencies)] = { "RTG_SP", "RTG_MOODY", "RTG_FITCH", "RTG_DBRS" };
const QString RatingsPrivate::m_ratingSyntax[static_cast<qint8>(Ratings::RatingAgency::CountAgencies)][static_cast<qint16>(Ratings::RatingValue::D) + 1] = {
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
        "CC",
        "CC",
        "CC",
        "C",
        "C",
        "C",
        "D"
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
        "Ca",
        "Ca",
        "Ca",
        "C",
        "C",
        "C",
        "D"
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
        "D"
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
        "D"
    }
};

CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Ratings& flows)
{
    stream << static_cast<qint32>(std::distance(std::begin(flows.d_func()->m_ratings), std::end(flows.d_func()->m_ratings)));
    std::for_each(std::begin(flows.d_func()->m_ratings), std::end(flows.d_func()->m_ratings), [&stream](Ratings::RatingValue val)->void {stream << static_cast<qint16>(val); });
    stream << static_cast<qint32>(std::distance(std::begin(flows.d_func()->m_watch), std::end(flows.d_func()->m_watch)));
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