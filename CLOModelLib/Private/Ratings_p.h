#ifndef Ratings_p_h__
#define Ratings_p_h__

#include "Ratings.h"
#include "Private/BackwardCompatibilityInterface_p.h"
class RatingsPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(Ratings)
    DECLARE_PRIVATE_COMMONS_COPY(Ratings)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(Ratings)
public:
    Ratings::RatingValue m_ratings[static_cast<qint8>(Ratings::RatingAgency::CountAgencies)];
    Ratings::CreditWatch m_watch[static_cast<qint8>(Ratings::RatingAgency::CountAgencies)];
    static const QString m_ratingSyntax[static_cast<qint8>(Ratings::RatingAgency::CountAgencies)][static_cast<qint16>(Ratings::RatingValue::Dm) + 1];
    static const QString m_ratingFields[static_cast<qint8>(Ratings::RatingAgency::CountAgencies)];
    static const QString m_AgencyName[static_cast<qint8>(Ratings::RatingAgency::CountAgencies)];
    static const QString m_reservedChars[static_cast<qint8>(Ratings::RatingAgency::CountAgencies)];
};


#endif // Ratings_p_h__