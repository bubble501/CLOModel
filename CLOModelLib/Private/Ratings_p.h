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
    Ratings::RatingValue m_ratings[Ratings::CountRatingAcencies];
    Ratings::CreditWatch m_watch[Ratings::CountRatingAcencies];
    static const QString m_ratingSyntax[Ratings::CountRatingAcencies][static_cast<qint16>(Ratings::RatingValue::Dm) + 1];
    static const QString m_ratingFields[Ratings::CountRatingAcencies];
    static const QString m_AgencyName[Ratings::CountRatingAcencies];
    static const QString m_reservedChars[Ratings::CountRatingAcencies];
};


#endif // Ratings_p_h__