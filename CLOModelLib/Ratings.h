#ifndef Ratings_h__
#define Ratings_h__
#include "BackwardCompatibilityInterface.h"
#include <QFlags>
class RatingsPrivate;
class RatingsTest;
namespace QBbgLib { 
    class QBbgSecurity; 
    class QBbgAbstractResponse; 
}
class CLOMODELLIB_EXPORT Ratings : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(Ratings)
    DECLARE_PUBLIC_COMMONS_COPY(Ratings)
public:
    enum class RatingAgency : qint32
    {
        Invalid=0,
        SP = 0x1,
        Moody = 0x2,
        Fitch = 0x4,
        DBRS = 0x8
    };
    Q_DECLARE_FLAGS(RatingAgencies, RatingAgency)
    enum CreditWatch : qint8
    {
        Positive = 1,
        Negative = -1,
        Stable = 0
    };
    enum class RatingValue : qint16
    {
        AAA = 1,
        AAp,
        AA,
        AAm,
        Ap,
        A,
        Am,
        BBBp,
        BBB,
        BBBm,
        BBp,
        BB,
        BBm,
        Bp,
        B,
        Bm,
        CCCp,
        CCC,
        CCCm,
        CCp,
        CC,
        CCm,
        Cp,
        C,
        Cm,
        DDDp,
        DDD,
        DDDm,
        DDp,
        DD,
        DDm,
        Dp,
        D,
        Dm,
        NR = 0
    };
    enum class RatingBucket : qint16
    {
        AAA = static_cast<qint16>(RatingValue::AAA),
        AA = static_cast<qint16>(RatingValue::AA),
        A = static_cast<qint16>(RatingValue::A),
        BBB = static_cast<qint16>(RatingValue::BBB),
        BB = static_cast<qint16>(RatingValue::BB),
        B = static_cast<qint16>(RatingValue::B),
        CCC = static_cast<qint16>(RatingValue::CCC),
        CC = static_cast<qint16>(RatingValue::CC),
        C = static_cast<qint16>(RatingValue::C),
        DDD = static_cast<qint16>(RatingValue::DDD),
        DD = static_cast<qint16>(RatingValue::DD),
        D = static_cast<qint16>(RatingValue::D),
        NR = static_cast<qint16>(RatingValue::NR)
    };
protected:
    enum { CountRatingAcencies = 4 };
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    RatingValue averageRating(double startingVal) const;
public:
    static QString agencyName(RatingAgency ag);
    static RatingBucket getBucket(RatingValue val);
    static QString RatingValueString(RatingValue val, RatingAgency agencySyntax);
    static QString RatingValueString(RatingBucket val, RatingAgency agencySyntax);
    static QString RatingValueString(RatingValue val);
    static QString RatingValueString(RatingBucket val);
    void setRating(RatingValue val, RatingAgency ag, CreditWatch wtch);
    void setRating(RatingValue val, RatingAgency ag);
    bool setRating(const QString& val, RatingAgency ag);
    void setWatch(CreditWatch wtch, RatingAgency ag);
    RatingValue getRating(RatingAgency ag) const;
    CreditWatch getWatch(RatingAgency ag) const;
    RatingValue highestRating() const;
    RatingValue lowestRating() const;
    RatingAgencies highestAgency() const;
    RatingAgencies lowestAgency() const;
    RatingValue averageRating() const;
    int notchesToAverageUpgrade() const;
    int notchesToAverageDowngrade() const;
    int notchesToAverageUpgradeBucket() const;
    int notchesToAverageDowngradeBucket() const;
    RatingValue ratingAtRank(int rnk) const;
    RatingValue ratingAtRankNoDuplicate(int rnk) const;
    RatingAgencies agencyAtRank(int rnk) const;
    bool downloadRatings(const QBbgLib::QBbgSecurity& sec);
    bool downloadRatings(const  QBbgLib::QBbgAbstractResponse  * const res);
    bool downloadRatings(const QString& name, const QString& bbgExtension);
    int numRatings() const;
    void reset();
    friend class RatingsTest;
    friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Ratings& flows);
    friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Ratings& flows);
    //static_assert(std::numeric_limits<RatingAgency>::max() == (1 << CountRatingAcencies), "Count of agencies not equal to the actual count of the agencies");
};
Q_DECLARE_METATYPE(Ratings)
Q_DECLARE_METATYPE(Ratings::RatingValue)
Q_DECLARE_METATYPE(Ratings::RatingAgency)
Q_DECLARE_METATYPE(Ratings::CreditWatch)
Q_DECLARE_OPERATORS_FOR_FLAGS(Ratings::RatingAgencies);
CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Ratings& flows);
CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Ratings& flows);
#endif // Ratings_h__