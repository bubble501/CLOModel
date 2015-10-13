#ifndef RATINGSTEST_H
#define RATINGSTEST_H

#include <QObject>

class RatingsTest : public QObject
{
    Q_OBJECT
private slots:
    void averageRating_data();
    void averageRating();
    void getBucket();
    void rankedRatings_data();
    void rankedRatings();
    void rankedAgency_data();
    void rankedAgency();
    void ratingFromString();
    void ratingFromString_data();
    void testCompare();
    void testCompare_data();
};

#endif // RATINGSTEST_H
