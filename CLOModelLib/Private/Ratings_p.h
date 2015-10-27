#ifndef Ratings_p_h__
#define Ratings_p_h__
#include <array>
#include "Ratings.h"
#include "Private/BackwardCompatibilityInterface_p.h"
class RatingsPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(Ratings)
    DECLARE_PRIVATE_COMMONS_COPY(Ratings)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(Ratings)
public:
    std::array<Ratings::RatingValue, Ratings::CountRatingAcencies> m_ratings;
    std::array<Ratings::CreditWatch, Ratings::CountRatingAcencies> m_watch;
    static const QString m_ratingSyntax[Ratings::CountRatingAcencies][static_cast<qint16>(Ratings::RatingValue::Dm) + 1];
    static const QString m_ratingFields[Ratings::CountRatingAcencies];
    static const QString m_AgencyName[Ratings::CountRatingAcencies];
    static const QString m_reservedChars[Ratings::CountRatingAcencies];
};


#endif // Ratings_p_h__