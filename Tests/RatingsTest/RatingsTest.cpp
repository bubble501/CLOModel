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
     //PIC average ratings
    testRating.reset(); testRating.setRating("Aa1", Ratings::RatingAgency::Moody); testRating.setRating("AA", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1255425735") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1255427194") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("Baa2", Ratings::RatingAgency::Moody); testRating.setRating("", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1255427608") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("BBB", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("BBB", Ratings::RatingAgency::DBRS); QTest::newRow("XS1200786074") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("Aa1", Ratings::RatingAgency::Moody); testRating.setRating("AA", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1255067396") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("AA", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0736642686") << testRating << Ratings::RatingValue::AAp;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0736644203") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("AA-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0255993577") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("A3", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("A-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0255995358") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A+", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0274610855") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0292255758") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("A+", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0325178548") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("A+", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0201883914") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0217569119") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0264193284") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("AAA", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0202811039") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("Baa3", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("A-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0202812276") << testRating << Ratings::RatingValue::BBBp;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("AAA", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0202810734") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("AAA", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0228780341") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("AA", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0228780937") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("Baa3", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0228781158") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("AAA", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0228780002") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A/*-", Ratings::RatingAgency::SP); testRating.setRating("A+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0289326935") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("AA", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("AA", Ratings::RatingAgency::DBRS); QTest::newRow("XS1200785183") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("AA", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0149246554") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("Aa1", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0249478073") << testRating << Ratings::RatingValue::AAp;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0249476374") << testRating << Ratings::RatingValue::AAA;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0271277971") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0271281817") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0271278433") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0271282039") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aa1", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0271277385") << testRating << Ratings::RatingValue::AAp;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0294479778") << testRating << Ratings::RatingValue::AAA;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0294480602") << testRating << Ratings::RatingValue::AAA;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("NR", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0294485072") << testRating << Ratings::RatingValue::AAp;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("AA", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0235419800") << testRating << Ratings::RatingValue::AAp;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0235420725") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0246904238") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0246905391") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0261648850") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0261650161") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0261650674") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0272536284") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0272536524") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("AAA", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0291722931") << testRating << Ratings::RatingValue::AAp;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("AA-", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0291724630") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aaa", Ratings::RatingAgency::Moody); testRating.setRating("AAA", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0310515258") << testRating << Ratings::RatingValue::AAA;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A+", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0310521496") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("A+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1042815834") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1130121293") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("A3", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0224731231") << testRating << Ratings::RatingValue::BBBp;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("BBB", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1183250874") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("NR", Ratings::RatingAgency::Moody); testRating.setRating("A+", Ratings::RatingAgency::SP); testRating.setRating("A-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1134219598") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("NR", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1134219838") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("A+", Ratings::RatingAgency::SP); testRating.setRating("A-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1134519393") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1134523239") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("NR", Ratings::RatingAgency::Moody); testRating.setRating("A+/*-", Ratings::RatingAgency::SP); testRating.setRating("A/*-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0254833758") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("NR", Ratings::RatingAgency::Moody); testRating.setRating("BBB", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0301707096") << testRating << Ratings::RatingValue::BBBp;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("A", Ratings::RatingAgency::DBRS); QTest::newRow("XS1192475512") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0288628224") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0288628810") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0308356970") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0353589608") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("BBB-", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0353591505") << testRating << Ratings::RatingValue::BBBm;
    testRating.reset(); testRating.setRating("Aa1", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0270510224") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("AA/*-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0226131463") << testRating << Ratings::RatingValue::AAp;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0236740501") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0236742036") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0254130916") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0254131484") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("Ba2", Ratings::RatingAgency::Moody); testRating.setRating("BBB", Ratings::RatingAgency::SP); testRating.setRating("A-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0274974111") << testRating << Ratings::RatingValue::BBBm;
    testRating.reset(); testRating.setRating("Baa2", Ratings::RatingAgency::Moody); testRating.setRating("BB", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0329656366") << testRating << Ratings::RatingValue::BBBm;
    testRating.reset(); testRating.setRating("Aa1", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0217069813") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0243658670") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0183102283") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0198318411") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("NR", Ratings::RatingAgency::Moody); testRating.setRating("BB-", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0300474193") << testRating << Ratings::RatingValue::BBBp;
    testRating.reset(); testRating.setRating("NR", Ratings::RatingAgency::Moody); testRating.setRating("BB-", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0300474607") << testRating << Ratings::RatingValue::BBBp;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("AA-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0220958036") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("AA-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0220959604") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0230221250") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("AA-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0230222068") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("Aa1", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0235781159") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0257369156") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("A1", Ratings::RatingAgency::Moody); testRating.setRating("BBB-", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0257371137") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0277409446") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Baa1", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("AA-", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0277411004") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A/*-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0213177214") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("A/*-", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0213178709") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("Aa1", Ratings::RatingAgency::Moody); testRating.setRating("AA/*-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0229739916") << testRating << Ratings::RatingValue::AAp;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0259418456") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("NR", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0552555806") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("A+/*-", Ratings::RatingAgency::SP); testRating.setRating("NR", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0552555558") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("AA", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1221083410") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("A2", Ratings::RatingAgency::Moody); testRating.setRating("A+", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1221084145") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("Aa2", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0225883387") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("BBB+", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0225883973") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("A3", Ratings::RatingAgency::Moody); testRating.setRating("BBB", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0225884278") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("AAA", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0168771748") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("A+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("US38741UAF49") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("Aa3", Ratings::RatingAgency::Moody); testRating.setRating("A", Ratings::RatingAgency::SP); testRating.setRating("A+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0176411071") << testRating << Ratings::RatingValue::Ap;
    testRating.reset(); testRating.setRating("A3", Ratings::RatingAgency::Moody); testRating.setRating("AAA/*-", Ratings::RatingAgency::SP); testRating.setRating("AA+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0273149962") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("A3", Ratings::RatingAgency::Moody); testRating.setRating("AA/*-", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0273153998") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("", Ratings::RatingAgency::SP); testRating.setRating("AA", Ratings::RatingAgency::Fitch); testRating.setRating("AA", Ratings::RatingAgency::DBRS); QTest::newRow("XS0944452993") << testRating << Ratings::RatingValue::AA;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("A", Ratings::RatingAgency::DBRS); QTest::newRow("XS0944453967") << testRating << Ratings::RatingValue::A;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("BBB", Ratings::RatingAgency::DBRS); QTest::newRow("XS0944454858") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("AA+", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("AH", Ratings::RatingAgency::DBRS); QTest::newRow("XS1117708831") << testRating << Ratings::RatingValue::AAm;
    testRating.reset(); testRating.setRating("Baa1", Ratings::RatingAgency::Moody); testRating.setRating("BB+", Ratings::RatingAgency::SP); testRating.setRating("BBB+", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0276410080") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("", Ratings::RatingAgency::Moody); testRating.setRating("A-", Ratings::RatingAgency::SP); testRating.setRating("A", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0191371391") << testRating << Ratings::RatingValue::Am;
    testRating.reset(); testRating.setRating("NR", Ratings::RatingAgency::Moody); testRating.setRating("BBB", Ratings::RatingAgency::SP); testRating.setRating("BBB", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS0275279064") << testRating << Ratings::RatingValue::BBB;
    testRating.reset(); testRating.setRating("B1", Ratings::RatingAgency::Moody); testRating.setRating("CCC", Ratings::RatingAgency::SP); testRating.setRating("D", Ratings::RatingAgency::Fitch); testRating.setRating("B", Ratings::RatingAgency::DBRS); QTest::newRow("XS0305732181") << testRating << Ratings::RatingValue::CCp;
    testRating.reset(); testRating.setRating("Baa3", Ratings::RatingAgency::Moody); testRating.setRating("BBB", Ratings::RatingAgency::SP); testRating.setRating("", Ratings::RatingAgency::Fitch); testRating.setRating("", Ratings::RatingAgency::DBRS); QTest::newRow("XS1220242025") << testRating << Ratings::RatingValue::BBBm;

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
    QTest::addColumn<Ratings::RatingAgencies>("highestRating");
    QTest::addColumn<Ratings::RatingAgencies>("lowestRating");
    QTest::addColumn<Ratings::RatingAgencies>("rankRatingNeg1");
    QTest::addColumn<Ratings::RatingAgencies>("rankRating0");
    QTest::addColumn<Ratings::RatingAgencies>("rankRating1");
    QTest::addColumn<Ratings::RatingAgencies>("rankRating2");
    QTest::addColumn<Ratings::RatingAgencies>("rankRating3");
    QTest::addColumn<Ratings::RatingAgencies>("rankRating4");
    QTest::addColumn<Ratings::RatingAgencies>("rankRatingUniqueNeg1");
    QTest::addColumn<Ratings::RatingAgencies>("rankRatingUnique0");
    QTest::addColumn<Ratings::RatingAgencies>("rankRatingUnique1");
    QTest::addColumn<Ratings::RatingAgencies>("rankRatingUnique2");
    QTest::addColumn<Ratings::RatingAgencies>("rankRatingUnique3");
    QTest::addColumn<Ratings::RatingAgencies>("rankRatingUnique4");

    {
        Ratings tempRating;
        Ratings::RatingAgencies result=Ratings::RatingAgency::Invalid;
        for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i) {
            tempRating.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
            result |= static_cast<Ratings::RatingAgency>(1 << i);
        }
        QTest::newRow("All Ratings Equal")
            << tempRating
            << result //highestRating
            << result  //lowestRating
            << result  //rankRatingNeg1
            << result  //rankRating0
            << result  //rankRating1
            << result  //rankRating2
            << result  //rankRating3
            << result  //rankRating4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm)+i), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("All Ratings Split Uniques")
            << tempRating
            << static_cast<Ratings::RatingAgencies>(0x1) //highestRating
            << static_cast<Ratings::RatingAgencies>(1 << (Ratings::CountRatingAcencies-1)) //lowestRating
            << static_cast<Ratings::RatingAgencies>(0x1) //rankRatingNeg1
            << static_cast<Ratings::RatingAgencies>(0x1) //rankRating0
            << static_cast<Ratings::RatingAgencies>(qMin(0x2, 1 << (Ratings::CountRatingAcencies -3))) //rankRating1
            << static_cast<Ratings::RatingAgencies>(qMin(0x4, 1 << (Ratings::CountRatingAcencies -2))) //rankRating2
            << static_cast<Ratings::RatingAgencies>(qMin(0x8, 1 << (Ratings::CountRatingAcencies - 1))) //rankRating3
            << static_cast<Ratings::RatingAgencies>(qMin(0x10, 1 << (Ratings::CountRatingAcencies - 1))) //rankRating4
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
            << static_cast<Ratings::RatingAgencies>(0x1 | 0x2) //highestRating
            << static_cast<Ratings::RatingAgencies>(1 << (Ratings::CountRatingAcencies - 1)) //lowestRating
            << static_cast<Ratings::RatingAgencies>(0x1 | 0x2) //rankRatingNeg1
            << static_cast<Ratings::RatingAgencies>(0x1 | 0x2) //rankRating0
            << static_cast<Ratings::RatingAgencies>(1 << (Ratings::CountRatingAcencies - 2)) //rankRating1
            << static_cast<Ratings::RatingAgencies>(1 << (Ratings::CountRatingAcencies - 1)) //rankRating2
            << static_cast<Ratings::RatingAgencies>(1 << (Ratings::CountRatingAcencies - 1)) //rankRating3
            << static_cast<Ratings::RatingAgencies>(1 << (Ratings::CountRatingAcencies - 1)) //rankRating4
            ;
    }
    {
        Ratings tempRating;
        for (qint32 i = 0; i < Ratings::CountRatingAcencies - 1; ++i)
            tempRating.setRating(static_cast<Ratings::RatingValue>(static_cast<qint16>(Ratings::RatingValue::AAm) + i), static_cast<Ratings::RatingAgency>(1 << i));
        QTest::newRow("One NR Split Uniques")
            << tempRating
            << static_cast<Ratings::RatingAgencies>(0x1) //highestRating
            << static_cast<Ratings::RatingAgencies>(1 << (Ratings::CountRatingAcencies - 2)) //lowestRating
            << static_cast<Ratings::RatingAgencies>(0x1) //rankRatingNeg1
            << static_cast<Ratings::RatingAgencies>(0x1) //rankRating0
            << static_cast<Ratings::RatingAgencies>(qMin(0x2, 1 << (Ratings::CountRatingAcencies - 3))) //rankRating1
            << static_cast<Ratings::RatingAgencies>(qMin(0x4, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating2
            << static_cast<Ratings::RatingAgencies>(qMin(0x8, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating3
            << static_cast<Ratings::RatingAgencies>(qMin(0x10, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating4
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
            << static_cast<Ratings::RatingAgencies>(0x1 | 0x2) //highestRating
            << static_cast<Ratings::RatingAgencies>(1 << (Ratings::CountRatingAcencies - 2)) //lowestRating
            << static_cast<Ratings::RatingAgencies>(0x1 | 0x2) //rankRatingNeg1
            << static_cast<Ratings::RatingAgencies>(0x1 | 0x2) //rankRating0
            << static_cast<Ratings::RatingAgencies>(qMin(0x4, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating1
            << static_cast<Ratings::RatingAgencies>(qMin(0x8, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating2
            << static_cast<Ratings::RatingAgencies>(qMin(0x10, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating3
            << static_cast<Ratings::RatingAgencies>(qMin(0x20, 1 << (Ratings::CountRatingAcencies - 2))) //rankRating4
            ;
    }
    {
        Ratings::RatingAgencies result = Ratings::RatingAgency::Invalid;
        for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i) 
            result |= static_cast<Ratings::RatingAgency>(1 << i);
        QTest::newRow("No Ratings")
            << Ratings()
            << static_cast<Ratings::RatingAgencies>(result) //highestRating
            << static_cast<Ratings::RatingAgencies>(result)  //lowestRating
            << static_cast<Ratings::RatingAgencies>(result) //rankRatingNeg1
            << static_cast<Ratings::RatingAgencies>(result)  //rankRating0
            << static_cast<Ratings::RatingAgencies>(result)  //rankRating1
            << static_cast<Ratings::RatingAgencies>(result)  //rankRating2
            << static_cast<Ratings::RatingAgencies>(result)  //rankRating3
            << static_cast<Ratings::RatingAgencies>(result)  //rankRating4
            ;
    }
}

void RatingsTest::rankedAgency()
{
    QFETCH(Ratings, testRating);
    QFETCH(Ratings::RatingAgencies, highestRating);
    QFETCH(Ratings::RatingAgencies, lowestRating);
    QFETCH(Ratings::RatingAgencies, rankRatingNeg1);
    QFETCH(Ratings::RatingAgencies, rankRating0);
    QFETCH(Ratings::RatingAgencies, rankRating1);
    QFETCH(Ratings::RatingAgencies, rankRating2);
    QFETCH(Ratings::RatingAgencies, rankRating3);
    QFETCH(Ratings::RatingAgencies, rankRating4);

    QCOMPARE(testRating.highestAgency(), highestRating);
    QCOMPARE(testRating.lowestAgency(), lowestRating);
    QCOMPARE(testRating.agencyAtRank(-1), rankRatingNeg1);
    QCOMPARE(testRating.agencyAtRank(0), rankRating0);
    QCOMPARE(testRating.agencyAtRank(1), rankRating1);
    QCOMPARE(testRating.agencyAtRank(2), rankRating2);
    QCOMPARE(testRating.agencyAtRank(3), rankRating3);
    QCOMPARE(testRating.agencyAtRank(4), rankRating4);
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

void RatingsTest::testCompare()
{
    QFETCH(Ratings, testRating1);
    QFETCH(Ratings, testRating2);
    QFETCH(bool, expectedCompRating);
    QFETCH(bool, expectedCompWatch);

    QCOMPARE(testRating1.compare(testRating2, Ratings::CompareRating), expectedCompRating);
    QCOMPARE(testRating1.compare(testRating2, Ratings::CompareWatch), expectedCompWatch);
    QCOMPARE(testRating1.compare(testRating2, Ratings::CompareAll), expectedCompWatch && expectedCompRating);
    QCOMPARE(testRating1==testRating2, expectedCompWatch && expectedCompRating);
}

void RatingsTest::testCompare_data()
{
    QTest::addColumn<Ratings>("testRating1");
    QTest::addColumn<Ratings>("testRating2");
    QTest::addColumn<bool>("expectedCompRating");
    QTest::addColumn<bool>("expectedCompWatch");

    Ratings testRating1, testRating2;
    for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        testRating2.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
    }
    QTest::newRow("All Ratings, all equal, no watch")
        << testRating1 << testRating2 << true << true;
    testRating1.reset();
    testRating2.reset();
    for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        testRating2.setRating(Ratings::RatingValue::BBB, static_cast<Ratings::RatingAgency>(1 << i));
    }
    QTest::newRow("All Ratings, all different, no watch")
        << testRating1 << testRating2 << false << true;

    testRating1.reset();
    testRating2.reset();
    for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        testRating2.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i), Ratings::Positive);
    }
    QTest::newRow("All Ratings, all equal, different watch")
        << testRating1 << testRating2 << true << false;
    testRating1.reset();
    testRating2.reset();
    for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        testRating2.setRating(Ratings::RatingValue::BBB, static_cast<Ratings::RatingAgency>(1 << i), Ratings::Positive);
    }
    QTest::newRow("All Ratings, all different, different watch")
        << testRating1 << testRating2 << false << false;
    //////////////////////////////////////////////////////////////////////////
    testRating1.reset();
    testRating2.reset();
    for (qint32 i = 0; i < Ratings::CountRatingAcencies-1; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        testRating2.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
    }
    QTest::newRow("One NR, all equal, no watch")
        << testRating1 << testRating2 << true << true;
    testRating1.reset();
    testRating2.reset();
    for (qint32 i = 0; i < Ratings::CountRatingAcencies-1; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        testRating2.setRating(Ratings::RatingValue::BBB, static_cast<Ratings::RatingAgency>(1 << i));
    }
    QTest::newRow("One NR, all different, no watch")
        << testRating1 << testRating2 << false << true;

    testRating1.reset();
    testRating2.reset();
    for (qint32 i = 0; i < Ratings::CountRatingAcencies-1; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        testRating2.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i), Ratings::Negative);
    }
    QTest::newRow("One NR, all equal, different watch")
        << testRating1 << testRating2 << true << false;
    testRating1.reset();
    testRating2.reset();
    for (qint32 i = 0; i < Ratings::CountRatingAcencies-1; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        testRating2.setRating(Ratings::RatingValue::BBB, static_cast<Ratings::RatingAgency>(1 << i), Ratings::Negative);
    }
    QTest::newRow("One NR, all different, different watch")
        << testRating1 << testRating2 << false << false;
    //////////////////////////////////////////////////////////////////////////
    testRating1.reset();
    testRating2.reset();
    for (qint32 i = 0; i < Ratings::CountRatingAcencies; ++i) {
        testRating1.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
        if(i>0)
            testRating2.setRating(Ratings::RatingValue::A, static_cast<Ratings::RatingAgency>(1 << i));
    }
    QTest::newRow("One NR in one")
        << testRating1 << testRating2 << false << true;
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
    template<>
    char *toString(const Ratings::RatingAgency &val)
    {
        QString ba = "RatingAgency: " + QString::number(static_cast<qint16>(val)) + " - " + Ratings::agencyName(val);
        return qstrdup(ba.toLatin1().data());
    }
    template<>
    char *toString(const Ratings::RatingAgencies &val)
    {
        QString ba = "RatingAgencies: " + QString::number(val.operator int(),2);
        return qstrdup(ba.toLatin1().data());
    }
}