#include "RatingsTest.h"
#include <QtTest>
#include <Ratings.h>

void RatingsTest::averageRating_data()
{
    QTest::addColumn<Ratings>("testRating");
    QTest::addColumn<Ratings::RatingValue>("ratingResult");
    Ratings testRating;
    QTest::newRow("Non Rated")
        << testRating
        << Ratings::RatingValue::NR;
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::AAA); i <= static_cast<qint16>(Ratings::RatingValue::Dm); ++i) {
        testRating.reset();
        testRating.setRating(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(0x1));
        QTest::newRow((QString("monoDimensional %1").arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i)))).toLatin1().data())
            << testRating
            << static_cast<Ratings::RatingValue>(i);
    }
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::AAA); i <= static_cast<qint16>(Ratings::RatingValue::Dm); ++i){
        for (qint16 j = static_cast<qint16>(Ratings::RatingValue::AAA); j <= static_cast<qint16>(Ratings::RatingValue::Dm); ++j) {
            testRating.reset();
            testRating.setRating(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(0x1));
            testRating.setRating(static_cast<Ratings::RatingValue>(j), static_cast<Ratings::RatingAgency>(0x2));
            QTest::newRow((QString("biDimensional %1 %2").arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i))).arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(j)))).toLatin1().data())
                << testRating
                << static_cast<Ratings::RatingValue>(static_cast<qint16>(std::ceil((static_cast<double>(i)+static_cast<double>(j)) / 2.0)));
        }
    }
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::AAA); i <= static_cast<qint16>(Ratings::RatingValue::Dm); ++i) {
        for (qint16 j = static_cast<qint16>(Ratings::RatingValue::AAA); j <= static_cast<qint16>(Ratings::RatingValue::Dm); ++j) {
            for (qint16 k = static_cast<qint16>(Ratings::RatingValue::AAA); k <= static_cast<qint16>(Ratings::RatingValue::Dm); ++k) {
                testRating.reset();
                testRating.setRating(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(0x1));
                testRating.setRating(static_cast<Ratings::RatingValue>(j), static_cast<Ratings::RatingAgency>(0x2));
                testRating.setRating(static_cast<Ratings::RatingValue>(k), static_cast<Ratings::RatingAgency>(0x4));
                QTest::newRow((QString("triDimensional %1 %2 %3").arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i))).arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(j))).arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(k)))).toLatin1().data())
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

void RatingsTest::rankedAgency_data()
{
    static_assert(Ratings::CountRatingAcencies >= 2, "You need at least 2 agencies to run the test" );
    QTest::addColumn<Ratings>("testRating");
    QTest::addColumn<Ratings::RatingAgency>("highestRating");
    QTest::addColumn<Ratings::RatingAgency>("lowestRating");
    QTest::addColumn<Ratings::RatingAgency>("rankRatingNeg1");
    QTest::addColumn<Ratings::RatingAgency>("rankRating0");
    QTest::addColumn<Ratings::RatingAgency>("rankRating1");
    QTest::addColumn<Ratings::RatingAgency>("rankRating2");
    QTest::addColumn<Ratings::RatingAgency>("rankRating3");
    QTest::addColumn<Ratings::RatingAgency>("rankRating4");
    QTest::addColumn<Ratings::RatingAgency>("rankRatingUniqueNeg1");
    QTest::addColumn<Ratings::RatingAgency>("rankRatingUnique0");
    QTest::addColumn<Ratings::RatingAgency>("rankRatingUnique1");
    QTest::addColumn<Ratings::RatingAgency>("rankRatingUnique2");
    QTest::addColumn<Ratings::RatingAgency>("rankRatingUnique3");
    QTest::addColumn<Ratings::RatingAgency>("rankRatingUnique4");

    {
        Ratings tempRating;
        for (qint32 i = 0; i<Ratings::CountRatingAcencies; ++i)
            tempRating.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("All Ratings Equal")
            << tempRating
            << static_cast<Ratings::RatingAgency>(0x1) //highestRating
            << static_cast<Ratings::RatingAgency>(0x1)  //lowestRating
            << static_cast<Ratings::RatingAgency>(0x1)  //rankRatingNeg1
            << static_cast<Ratings::RatingAgency>(0x1)  //rankRating0
            << static_cast<Ratings::RatingAgency>(0x1)  //rankRating1
            << static_cast<Ratings::RatingAgency>(0x1)  //rankRating2
            << static_cast<Ratings::RatingAgency>(0x1)  //rankRating3
            << static_cast<Ratings::RatingAgency>(0x1)  //rankRating4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm)+i), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("All Ratings Split Uniques")
            << tempRating
            << static_cast<Ratings::RatingAgency>(0x1) //highestRating
            << static_cast<Ratings::RatingAgency>(1 << (Ratings::CountRatingAcencies-1)) //lowestRating
            << static_cast<Ratings::RatingAgency>(0x1) //rankRatingNeg1
            << static_cast<Ratings::RatingAgency>(0x1) //rankRating0
            << static_cast<Ratings::RatingAgency>(qMin(0x1, 1 << (Ratings::CountRatingAcencies -3))) //rankRating1
            << static_cast<Ratings::RatingAgency>(qMin(0x2, 1 << (Ratings::CountRatingAcencies -2))) //rankRating2
            << static_cast<Ratings::RatingAgency>(qMin(0x4, 1 << (Ratings::CountRatingAcencies - 1))) //rankRating3
            << static_cast<Ratings::RatingAgency>(qMin(0x8, 1 << (Ratings::CountRatingAcencies - 1))) //rankRating4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < 2; ++i)
            tempRating.setRating(Ratings::RatingValue::AAm, static_cast<Ratings::RatingAgency>(1 << i));
        for (qint32 i = 2; i < Ratings::CountRatingAcencies; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + i - 2), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("All Ratings Split Duplicates")
            << tempRating
            << static_cast<Ratings::RatingAgency>(0x1) //highestRating
            << static_cast<Ratings::RatingAgency>(1 << (Ratings::CountRatingAcencies - 1)) //lowestRating
            << static_cast<Ratings::RatingAgency>(0x1) //rankRatingNeg1
            << static_cast<Ratings::RatingAgency>(0x1) //rankRating0
            << static_cast<Ratings::RatingAgency>(0x1) //rankRating1
            << static_cast<Ratings::RatingAgency>(1 << (Ratings::CountRatingAcencies - 2)) //rankRating2
            << static_cast<Ratings::RatingAgency>(1 << (Ratings::CountRatingAcencies - 1)) //rankRating3
            << static_cast<Ratings::RatingAgency>(1 << (Ratings::CountRatingAcencies - 1)) //rankRating4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < Ratings::CountRatingAcencies - 1; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + i), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("One NR Split Uniques")
            << tempRating
            << static_cast<Ratings::RatingAgency>(0x1) //highestRating
            << static_cast<Ratings::RatingAgency>(1 << (Ratings::CountRatingAcencies - 2)) //lowestRating
            << static_cast<Ratings::RatingAgency>(0x1) //rankRatingNeg1
            << static_cast<Ratings::RatingAgency>(0x1) //rankRating0
            << static_cast<Ratings::RatingAgency>(qMin(0x1, 1 << (Ratings::CountRatingAcencies - 3))) //rankRating1
            << static_cast<Ratings::RatingAgency>(qMin(0x2, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating2
            << static_cast<Ratings::RatingAgency>(qMin(0x4, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating3
            << static_cast<Ratings::RatingAgency>(qMin(0x8, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < 2; ++i)
            tempRating.setRating(Ratings::RatingValue::AAm, static_cast<Ratings::RatingAgency>(1 << i));
        for (qint32 i = 2; i < Ratings::CountRatingAcencies - 1; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + i - 2), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("All Ratings Split Duplicates one NR")
            << tempRating
            << static_cast<Ratings::RatingAgency>(0x1) //highestRating
            << static_cast<Ratings::RatingAgency>(1 << (Ratings::CountRatingAcencies - 2)) //lowestRating
            << static_cast<Ratings::RatingAgency>(0x1) //rankRatingNeg1
            << static_cast<Ratings::RatingAgency>(0x1) //rankRating0
            << static_cast<Ratings::RatingAgency>(0x1) //rankRating1
            << static_cast<Ratings::RatingAgency>(qMin(0x2, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating2
            << static_cast<Ratings::RatingAgency>(qMin(0x4, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating3
            << static_cast<Ratings::RatingAgency>(qMin(0x8, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating4
            ;
    }
    QTest::newRow("No Ratings") 
        << Ratings()
        << static_cast<Ratings::RatingAgency>(0x1) //highestRating
        << static_cast<Ratings::RatingAgency>(0x1)  //lowestRating
        << static_cast<Ratings::RatingAgency>(0x1) //rankRatingNeg1
        << static_cast<Ratings::RatingAgency>(0x1)  //rankRating0
        << static_cast<Ratings::RatingAgency>(0x1)  //rankRating1
        << static_cast<Ratings::RatingAgency>(0x1)  //rankRating2
        << static_cast<Ratings::RatingAgency>(0x1)  //rankRating3
        << static_cast<Ratings::RatingAgency>(0x1)  //rankRating4
  ;
}

void RatingsTest::rankedAgency()
{
    QFETCH(Ratings, testRating);
    QFETCH(Ratings::RatingAgency, highestRating);
    QFETCH(Ratings::RatingAgency, lowestRating);
    QFETCH(Ratings::RatingAgency, rankRatingNeg1);
    QFETCH(Ratings::RatingAgency, rankRating0);
    QFETCH(Ratings::RatingAgency, rankRating1);
    QFETCH(Ratings::RatingAgency, rankRating2);
    QFETCH(Ratings::RatingAgency, rankRating3);
    QFETCH(Ratings::RatingAgency, rankRating4);

    QVERIFY2(testRating.highestAgency() | highestRating, QString("Actual: %1 Expected: %2").arg(testRating.highestAgency().operator int(), 0, 2).arg(static_cast<qint32>(highestRating), 0, 2).toLatin1().data());
    QVERIFY2(testRating.lowestAgency()|lowestRating, QString("Actual: %1 Expected: %2").arg(testRating.highestAgency().operator int(), 0, 2).arg(static_cast<qint32>(lowestRating), 0, 2).toLatin1().data());
    QVERIFY2(testRating.agencyAtRank(-1)|rankRatingNeg1, QString("Actual: %1 Expected: %2").arg(testRating.highestAgency().operator int(), 0, 2).arg(static_cast<qint32>(rankRatingNeg1), 0, 2).toLatin1().data());
    QVERIFY2(testRating.agencyAtRank(0)|rankRating0, QString("Actual: %1 Expected: %2").arg(testRating.highestAgency().operator int(), 0, 2).arg(static_cast<qint32>(rankRating0), 0, 2).toLatin1().data());
    QVERIFY2(testRating.agencyAtRank(1)|rankRating1, QString("Actual: %1 Expected: %2").arg(testRating.highestAgency().operator int(), 0, 2).arg(static_cast<qint32>(rankRating1), 0, 2).toLatin1().data());
    QVERIFY2(testRating.agencyAtRank(2)|rankRating2, QString("Actual: %1 Expected: %2").arg(testRating.highestAgency().operator int(), 0, 2).arg(static_cast<qint32>(rankRating2), 0, 2).toLatin1().data());
    QVERIFY2(testRating.agencyAtRank(3)|rankRating3, QString("Actual: %1 Expected: %2").arg(testRating.highestAgency().operator int(), 0, 2).arg(static_cast<qint32>(rankRating3), 0, 2).toLatin1().data());
    QVERIFY2(testRating.agencyAtRank(4)|rankRating4, QString("Actual: %1 Expected: %2").arg(testRating.highestAgency().operator int(), 0, 2).arg(static_cast<qint32>(rankRating4), 0, 2).toLatin1().data());
}

void RatingsTest::ratingFromString()
{
    QFETCH(QString, testRating);
    QFETCH(Ratings::RatingAgency, rtgSyntax);
    Ratings tmp;
    tmp.setRating(testRating, rtgSyntax);
    QTEST(tmp.getRating(rtgSyntax), "resultRating");
    QTEST(tmp.getWatch(rtgSyntax), "resultWatch");
}

void RatingsTest::ratingFromString_data()
{
    QTest::addColumn<QString>("testRating");
    QTest::addColumn<Ratings::RatingAgency>("rtgSyntax");
    QTest::addColumn<Ratings::RatingValue>("resultRating");
    QTest::addColumn<Ratings::CreditWatch>("resultWatch");

    for (qint32 j = 0; j < Ratings::CountRatingAcencies; ++j) {
        for (qint16 i = static_cast<qint16>(Ratings::RatingValue::NR); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i) {
            if (!Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(1 << j)).isEmpty())
                QTest::newRow(QString("%1 No Watch %2").arg(Ratings::agencyName(static_cast<Ratings::RatingAgency>(1 << j))).arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i))).toLatin1().data())
                << Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(1 << j))
                << static_cast<Ratings::RatingAgency>(1 << j) //rtgSyntax
                << static_cast<Ratings::RatingValue>(i) //resultRating
                << Ratings::CreditWatch::Stable //resultWatch
                ;
        }
    }
    for (qint32 j = 0; j < Ratings::CountRatingAcencies; ++j) {
        for (qint16 i = static_cast<qint16>(Ratings::RatingValue::AAA); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i) {
            if (!Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(1 << j)).isEmpty())
                QTest::newRow(QString("%1 Positive Watch %2").arg(Ratings::agencyName(static_cast<Ratings::RatingAgency>(1 << j))).arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i))).toLatin1().data())
                << Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(1 << j)) + " *+"
                << static_cast<Ratings::RatingAgency>(1 << j) //rtgSyntax
                << static_cast<Ratings::RatingValue>(i) //resultRating
                << Ratings::CreditWatch::Positive //resultWatch
                ;
        }
    }
    for (qint8 j = 0; j < Ratings::CountRatingAcencies; ++j) {
        for (qint16 i = static_cast<qint16>(Ratings::RatingValue::AAA); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i) {
            if (!Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(1 << j)).isEmpty())
                QTest::newRow(QString("%1 Negative Watch %2").arg(Ratings::agencyName(static_cast<Ratings::RatingAgency>(1 << j))).arg(Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i))).toLatin1().data())
                << Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), static_cast<Ratings::RatingAgency>(1 << j)) + " *-"
                << static_cast<Ratings::RatingAgency>(1 << j) //rtgSyntax
                << static_cast<Ratings::RatingValue>(i) //resultRating
                << Ratings::CreditWatch::Negative //resultWatch
                ;
        }
    }
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::NR); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i) {
        if (!Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), Ratings::RatingAgency::SP).isEmpty())
        QTest::newRow("S&P Expected No Watch")
            << "(P)" + Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), Ratings::RatingAgency::SP)
            << Ratings::RatingAgency::SP //rtgSyntax
            << static_cast<Ratings::RatingValue>(i) //resultRating
            << Ratings::CreditWatch::Stable //resultWatch
            ;
    }
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::NR); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i) {
        if (!Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), Ratings::RatingAgency::Moody).isEmpty())
        QTest::newRow("Moody's Expected No Watch")
            << "(P)" + Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), Ratings::RatingAgency::Moody)
            << Ratings::RatingAgency::Moody //rtgSyntax
            << static_cast<Ratings::RatingValue>(i) //resultRating
            << Ratings::CreditWatch::Stable //resultWatch
            ;
    }
    for (qint16 i = static_cast<qint16>(Ratings::RatingValue::NR); i <= static_cast<qint16>(Ratings::RatingValue::D); ++i) {
        if (!Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), Ratings::RatingAgency::Fitch).isEmpty())
        QTest::newRow("Fitch Expected No Watch")
            << Ratings::RatingValueString(static_cast<Ratings::RatingValue>(i), Ratings::RatingAgency::Fitch) + 'e'
            << Ratings::RatingAgency::Fitch //rtgSyntax
            << static_cast<Ratings::RatingValue>(i) //resultRating
            << Ratings::CreditWatch::Stable //resultWatch
            ;
    }
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
    QFETCH(Ratings::RatingValue, rankRating4);
    QFETCH(Ratings::RatingValue, rankRatingUniqueNeg1);
    QFETCH(Ratings::RatingValue, rankRatingUnique0);
    QFETCH(Ratings::RatingValue, rankRatingUnique1);
    QFETCH(Ratings::RatingValue, rankRatingUnique2);
    QFETCH(Ratings::RatingValue, rankRatingUnique3);
    QFETCH(Ratings::RatingValue, rankRatingUnique4);

    QCOMPARE(testRating.highestRating(), highestRating);
    QCOMPARE(testRating.lowestRating(), lowestRating);
    QCOMPARE(testRating.ratingAtRank(-1), rankRatingNeg1);
    QCOMPARE(testRating.ratingAtRank(0), rankRating0);
    QCOMPARE(testRating.ratingAtRank(1), rankRating1);
    QCOMPARE(testRating.ratingAtRank(2), rankRating2);
    QCOMPARE(testRating.ratingAtRank(3), rankRating3);
    QCOMPARE(testRating.ratingAtRank(4), rankRating4);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(-1), rankRatingUniqueNeg1);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(0), rankRatingUnique0);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(1), rankRatingUnique1);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(2), rankRatingUnique2);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(3), rankRatingUnique3);
    QCOMPARE(testRating.ratingAtRankNoDuplicate(3), rankRatingUnique4);

}

void RatingsTest::rankedRatings_data()
{
    static_assert(Ratings::CountRatingAcencies >= 2, "You need at least 2 agencies to run the test");
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
        for (qint32 i = 0; i <Ratings::CountRatingAcencies; ++i)
            tempRating.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
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
        for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + i), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("All Ratings Split Uniques")
            << tempRating
            << Ratings::RatingValue::AAm //highestRating
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + Ratings::CountRatingAcencies - 1) //lowestRating
            << Ratings::RatingValue::AAm //rankRatingNeg1
            << Ratings::RatingValue::AAm //rankRating0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 1, 1)) //rankRating1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 1, 2)) //rankRating2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 1, 3)) //rankRating3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 1, 4)) //rankRating4
            << Ratings::RatingValue::AAm //rankRatingUniqueNeg1
            << Ratings::RatingValue::AAm //rankRatingUnique0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 1, 1)) //rankRatingUnique1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 1, 2)) //rankRatingUnique2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 1, 3)) //rankRatingUnique3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 1, 4)) //rankRatingUnique4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < 2; ++i)
            tempRating.setRating(Ratings::RatingValue::AAm, static_cast<Ratings::RatingAgency>(1 << i));
        for (qint32 i = 2; i <Ratings::CountRatingAcencies; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + i - 2), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("All Ratings Split Duplicates")
            << tempRating
            << Ratings::RatingValue::AAm //highestRating
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + Ratings::CountRatingAcencies - 3) //lowestRating
            << Ratings::RatingValue::AAm //rankRatingNeg1
            << Ratings::RatingValue::AAm //rankRating0
            << Ratings::RatingValue::AAm //rankRating1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 3, 2) - 1) //rankRating2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 3) - 1) //rankRating3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 4) - 1) //rankRating4
            << Ratings::RatingValue::AAm //rankRatingUniqueNeg1
            << Ratings::RatingValue::AAm //rankRatingUnique0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 3, 2) - 1) //rankRatingUnique1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 3) - 1) //rankRatingUnique2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 4) - 1) //rankRatingUnique3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 5) - 1) //rankRatingUnique4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < Ratings::CountRatingAcencies - 1; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + i), static_cast<Ratings::RatingAgency>(1<<i));
        QTest::newRow("One NR Split Uniques")
            << tempRating
            << Ratings::RatingValue::AAm //highestRating
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + Ratings::CountRatingAcencies - 2) //lowestRating
            << Ratings::RatingValue::AAm //rankRatingNeg1
            << Ratings::RatingValue::AAm //rankRating0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 2, 1)) //rankRating1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 2, 2)) //rankRating2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 2, 3)) //rankRating3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 2, 4)) //rankRating4
            << Ratings::RatingValue::AAm //rankRatingUniqueNeg1
            << Ratings::RatingValue::AAm //rankRatingUnique0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 2, 1)) //rankRatingUnique1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 2, 2)) //rankRatingUnique2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 2, 3)) //rankRatingUnique3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + qMin(Ratings::CountRatingAcencies - 2, 4)) //rankRatingUnique4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < 2; ++i)
            tempRating.setRating(Ratings::RatingValue::AAm, static_cast<Ratings::RatingAgency>(1 << i));
        for (qint32 i = 2; i < Ratings::CountRatingAcencies-1; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + i - 2), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("One NR Split Duplicates")
            << tempRating
            << Ratings::RatingValue::AAm //highestRating
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + Ratings::CountRatingAcencies - 4) //lowestRating
            << Ratings::RatingValue::AAm //rankRatingNeg1
            << Ratings::RatingValue::AAm //rankRating0
            << Ratings::RatingValue::AAm //rankRating1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 2) - 2) //rankRating2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 3) - 2) //rankRating3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 4) - 2) //rankRating4
            << Ratings::RatingValue::AAm //rankRatingUniqueNeg1
            << Ratings::RatingValue::AAm //rankRatingUnique0
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 2) - 2) //rankRatingUnique1
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 3) - 2) //rankRatingUnique2
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 4) - 2) //rankRatingUnique3
            << static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::Ap) + qMin(Ratings::CountRatingAcencies - 2, 5) - 2) //rankRatingUnique4
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
namespace QTest {
    template<>
    char *toString(const Ratings::RatingValue &val)
    {
        QString ba = "RatingValue: " + QString::number(static_cast<qint16>(val)) + " - " + Ratings::RatingValueString(val);
        return qstrdup(ba.toLatin1().data());
    }
}