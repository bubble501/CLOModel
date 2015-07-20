#include "RatingsTest.h"
#include <QtTest>
#include <Ratings.h>

void RatingsTest::averageRating_data()
{
    QTest::addColumn<Ratings>("testRating");
    QTest::addColumn<Ratings::RatingValue>("ratingResult");
    Ratings testRating;
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::AAA); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i) {
        testRating.reset();
        testRating.setRating(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(0));
        QTest::newRow("monoDimensional")
            << testRating
            << static_cast<Ratings::RatingValue>(i);
    }
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::AAA); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i){
        for (qint16 j = static_cast<qint16>(Ratings::RatingValue::AAA); j <= static_cast<qint16>(Ratings::RatingValue::D); ++j) {
            testRating.reset();
            testRating.setRating(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(0));
            testRating.setRating(static_cast<Ratings::RatingValue>(j), static_cast<Ratings::RatingAgency>(1));
            QTest::newRow("biDimensional")
                << testRating
                << static_cast<Ratings::RatingValue>(static_cast<qint16>(std::ceil((static_cast<double>(i)+static_cast<double>(j)) / 2.0)));
        }
    }
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::AAA); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i) {
        for (qint16 j = static_cast<qint16>(Ratings::RatingValue::AAA); j <= static_cast<qint16>(Ratings::RatingValue::D); ++j) {
            for (qint16 k = static_cast<qint16>(Ratings::RatingValue::AAA); k <= static_cast<qint16>(Ratings::RatingValue::D); ++k) {
                testRating.reset();
                testRating.setRating(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(0));
                testRating.setRating(static_cast<Ratings::RatingValue>(j), static_cast<Ratings::RatingAgency>(1));
                testRating.setRating(static_cast<Ratings::RatingValue>(k), static_cast<Ratings::RatingAgency>(2));
                QTest::newRow("triDimensional")
                    << testRating
                    << static_cast<Ratings::RatingValue>(static_cast<qint16>(std::ceil((static_cast<double>(i)+static_cast<double>(j)+static_cast<double>(k)) / 3.0)));
            }
        }
    }
}

void RatingsTest::averageRating()
{
    QFETCH(Ratings, testRating);
    QFETCH(Ratings::RatingValue, ratingResult);
    QCOMPARE(testRating.averageRating(), ratingResult);
}

void RatingsTest::getBucket()
{
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::AAA), Ratings::RatingBucket::AAA);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::AAp), Ratings::RatingBucket::AA);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::AA), Ratings::RatingBucket::AA);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::AAm), Ratings::RatingBucket::AA);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::Ap), Ratings::RatingBucket::A);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::A), Ratings::RatingBucket::A);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::Am), Ratings::RatingBucket::A);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::BBBp), Ratings::RatingBucket::BBB);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::BBB), Ratings::RatingBucket::BBB);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::BBBm), Ratings::RatingBucket::BBB);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::BBp), Ratings::RatingBucket::BB);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::BB), Ratings::RatingBucket::BB);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::BBm), Ratings::RatingBucket::BB);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::Bp), Ratings::RatingBucket::B);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::B), Ratings::RatingBucket::B);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::Bm), Ratings::RatingBucket::B);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::CCCp), Ratings::RatingBucket::CCC);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::CCC), Ratings::RatingBucket::CCC);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::CCCm), Ratings::RatingBucket::CCC);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::CCp), Ratings::RatingBucket::CC);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::CC), Ratings::RatingBucket::CC);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::CCm), Ratings::RatingBucket::CC);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::Cp), Ratings::RatingBucket::C);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::C), Ratings::RatingBucket::C);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::Cm), Ratings::RatingBucket::C);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::D), Ratings::RatingBucket::D);
    QCOMPARE(Ratings::getBucket(Ratings::RatingValue::NR), Ratings::RatingBucket::NR);
}

void RatingsTest::rankedRatings_data()
{
    QTest::addColumn<Ratings>("testRating");
    QTest::addColumn<Ratings::RatingValue>("highestRating");
    QTest::addColumn<Ratings::RatingValue>("lowestRating");
    QTest::addColumn<Ratings::RatingValue>("rankRatingNeg1");
    QTest::addColumn<Ratings::RatingValue>("rankRating0");
    QTest::addColumn<Ratings::RatingValue>("rankRating1");
    QTest::addColumn<Ratings::RatingValue>("rankRating2");
    QTest::addColumn<Ratings::RatingValue>("rankRating3");
    QTest::addColumn<Ratings::RatingValue>("rankRating4");
    QTest::addColumn<Ratings::RatingValue>("rankRatingUniqueNeg1");
    QTest::addColumn<Ratings::RatingValue>("rankRatingUnique0");
    QTest::addColumn<Ratings::RatingValue>("rankRatingUnique1");
    QTest::addColumn<Ratings::RatingValue>("rankRatingUnique2");
    QTest::addColumn<Ratings::RatingValue>("rankRatingUnique3");
    QTest::addColumn<Ratings::RatingValue>("rankRatingUnique4");

    {
        Ratings tempRating;
        for (qint8 i = 0; i<static_cast<qint8>(Ratings::RatingAgency::CountAgencies);++i)
            tempRating.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(i));
        QTest::newRow("All Ratings Equal")
            << tempRating
            << Ratings::RatingValue::A //highestRating
            << Ratings::RatingValue::A //lowestRating
            << Ratings::RatingValue::A //rankRatingNeg1
            << Ratings::RatingValue::A //rankRating0
            << Ratings::RatingValue::A //rankRating1
            << Ratings::RatingValue::A //rankRating2
            << Ratings::RatingValue::A //rankRating3
            << Ratings::RatingValue::A //rankRating4
            << Ratings::RatingValue::A //rankRatingUniqueNeg1
            << Ratings::RatingValue::A //rankRatingUnique0
            << Ratings::RatingValue::A //rankRatingUnique1
            << Ratings::RatingValue::A //rankRatingUnique2
            << Ratings::RatingValue::A //rankRatingUnique3
            << Ratings::RatingValue::A //rankRatingUnique4
            ;
    }
    {
        Ratings tempRating;
        for (qint8 i = 0; i < static_cast<qint8>(Ratings::RatingAgency::CountAgencies); ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm)+i), static_cast<Ratings::RatingAgency>(i));
        QTest::newRow("All Ratings Split Uniques")
            << tempRating
            << Ratings::RatingValue::AAm //highestRating
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1) //lowestRating
            << Ratings::RatingValue::AAm //rankRatingNeg1
            << Ratings::RatingValue::AAm //rankRating0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 1)) //rankRating1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 2)) //rankRating2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 3)) //rankRating3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 4)) //rankRating4
            << Ratings::RatingValue::AAm //rankRatingUniqueNeg1
            << Ratings::RatingValue::AAm //rankRatingUnique0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 1)) //rankRatingUnique1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 2)) //rankRatingUnique2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 3)) //rankRatingUnique3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 4)) //rankRatingUnique4
            ;
    }
    {
        Ratings tempRating;
        for (qint8 i = 0; i < 2; ++i)
            tempRating.setRating(Ratings::RatingValue::AAm, static_cast<Ratings::RatingAgency>(i));
        for (qint8 i = 2; i < static_cast<qint8>(Ratings::RatingAgency::CountAgencies); ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + i - 2), static_cast<Ratings::RatingAgency>(i));
        QTest::newRow("All Ratings Split Duplicates")
            << tempRating
            << Ratings::RatingValue::AAm //highestRating
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 3) //lowestRating
            << Ratings::RatingValue::AAm //rankRatingNeg1
            << Ratings::RatingValue::AAm //rankRating0
            << Ratings::RatingValue::AAm //rankRating1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 2) - 2) //rankRating2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 3) - 2) //rankRating3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 4) - 2) //rankRating4
            << Ratings::RatingValue::AAm //rankRatingUniqueNeg1
            << Ratings::RatingValue::AAm //rankRatingUnique0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 2) - 2) //rankRatingUnique1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 3) - 2) //rankRatingUnique2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 4) - 2) //rankRatingUnique3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 1, 5) - 2) //rankRatingUnique4
            ;
    }
    {
        Ratings tempRating;
        for (qint8 i = 0; i < static_cast<qint8>(Ratings::RatingAgency::CountAgencies)-1; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + i), static_cast<Ratings::RatingAgency>(i));
        QTest::newRow("One NR Split Uniques")
            << tempRating
            << Ratings::RatingValue::AAm //highestRating
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2) //lowestRating
            << Ratings::RatingValue::AAm //rankRatingNeg1
            << Ratings::RatingValue::AAm //rankRating0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 1)) //rankRating1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 2)) //rankRating2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 3)) //rankRating3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 4)) //rankRating4
            << Ratings::RatingValue::AAm //rankRatingUniqueNeg1
            << Ratings::RatingValue::AAm //rankRatingUnique0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 1)) //rankRatingUnique1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 2)) //rankRatingUnique2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 3)) //rankRatingUnique3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 4)) //rankRatingUnique4
            ;
    }
    {
        Ratings tempRating;
        for (qint8 i = 0; i < 2; ++i)
            tempRating.setRating(Ratings::RatingValue::AAm, static_cast<Ratings::RatingAgency>(i));
        for (qint8 i = 2; i < static_cast<qint8>(Ratings::RatingAgency::CountAgencies)-1; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + i - 2), static_cast<Ratings::RatingAgency>(i));
        QTest::newRow("All Ratings Split Duplicates")
            << tempRating
            << Ratings::RatingValue::AAm //highestRating
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 4) //lowestRating
            << Ratings::RatingValue::AAm //rankRatingNeg1
            << Ratings::RatingValue::AAm //rankRating0
            << Ratings::RatingValue::AAm //rankRating1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 2) - 2) //rankRating2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 3) - 2) //rankRating3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 4) - 2) //rankRating4
            << Ratings::RatingValue::AAm //rankRatingUniqueNeg1
            << Ratings::RatingValue::AAm //rankRatingUnique0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 2) - 2) //rankRatingUnique1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 3) - 2) //rankRatingUnique2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 4) - 2) //rankRatingUnique3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(static_cast<qint8>(Ratings::RatingAgency::CountAgencies) - 2, 5) - 2) //rankRatingUnique4
            ;
    }
    QTest::newRow("No Ratings") 
        << Ratings()
        << Ratings::RatingValue::NR //highestRating
        << Ratings::RatingValue::NR //lowestRating
        << Ratings::RatingValue::NR //rankRatingNeg1
        << Ratings::RatingValue::NR //rankRating0
        << Ratings::RatingValue::NR //rankRating1
        << Ratings::RatingValue::NR //rankRating2
        << Ratings::RatingValue::NR //rankRating3
        << Ratings::RatingValue::NR //rankRating4
        << Ratings::RatingValue::NR //rankRatingUniqueNeg1
        << Ratings::RatingValue::NR //rankRatingUnique0
        << Ratings::RatingValue::NR //rankRatingUnique1
        << Ratings::RatingValue::NR //rankRatingUnique2
        << Ratings::RatingValue::NR //rankRatingUnique3
        << Ratings::RatingValue::NR //rankRatingUnique4
  ;
}

void RatingsTest::rankedRatings()
{
    QFETCH(Ratings, testRating);
    QFETCH(Ratings::RatingValue, highestRating);
    QFETCH(Ratings::RatingValue, lowestRating);
    QFETCH(Ratings::RatingValue, rankRatingNeg1);
    QFETCH(Ratings::RatingValue, rankRating0);
    QFETCH(Ratings::RatingValue, rankRating1);
    QFETCH(Ratings::RatingValue, rankRating2);
    QFETCH(Ratings::RatingValue, rankRating3);
    QFETCH(Ratings::RatingValue, rankRatingUniqueNeg1);
    QFETCH(Ratings::RatingValue, rankRatingUnique0);
    QFETCH(Ratings::RatingValue, rankRatingUnique1);
    QFETCH(Ratings::RatingValue, rankRatingUnique2);
    QFETCH(Ratings::RatingValue, rankRatingUnique3);

    QCOMPARE(testRating.highestRating(), highestRating);
    QCOMPARE(testRating.lowestRating(), lowestRating);
    QCOMPARE(testRating.ratingAtRank(-1), rankRatingNeg1);
    QCOMPARE(testRating.ratingAtRank(0), rankRating0);
    QCOMPARE(testRating.ratingAtRank(1), rankRating1);
    QCOMPARE(testRating.ratingAtRank(2), rankRating2);
    QCOMPARE(testRating.ratingAtRank(3), rankRating3);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(-1), rankRatingUniqueNeg1);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(0), rankRatingUnique0);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(1), rankRatingUnique1);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(2), rankRatingUnique2);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(3), rankRatingUnique3);

}
