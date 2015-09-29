#include "BloombergVectorTest.h"
#include <BloombergVector.h>
#include <QtTest>
void BloombergVectorTest::testSum()
{
    QFETCH(BloombergVector, testBase);
    QFETCH(BloombergVector, testSummed);
    QFETCH(BloombergVector, result);
    QCOMPARE(testBase + testSummed, result);
}

void BloombergVectorTest::testSum_data()
{
    QTest::addColumn<BloombergVector>("testBase");
    QTest::addColumn<BloombergVector>("testSummed");
    QTest::addColumn<BloombergVector>("result");
    

    QTest::newRow("Single value no anchor same divisor")
        << BloombergVector("5")
        << BloombergVector("7")
        << BloombergVector("12")
        ;
    QTest::newRow("Single value one anchor same divisor")
        << BloombergVector("A 05/30/2015 5")
        << BloombergVector("7")
        << BloombergVector("A 05/30/2015 12")
        ;
    QTest::newRow("Single value same anchor same divisor")
        << BloombergVector("A 05/30/2015 5")
        << BloombergVector("A 05/30/2015 7")
        << BloombergVector("A 05/30/2015 12")
        ;
    QTest::newRow("Single value different anchor same divisor")
        << BloombergVector("A 05/30/2015 5")
        << BloombergVector("A 05/30/2014 7")
        << BloombergVector("A 05/30/2015 12")
        ;

    //Steps
    QTest::newRow("Steps no anchor same divisor")
        << BloombergVector("5 12S 3")
        << BloombergVector("7 6S 1")
        << BloombergVector("12 6S 6 6S 4")
        ;
    QTest::newRow("Steps one anchor same divisor")
        << BloombergVector("A 05/30/2015 5 12S 3")
        << BloombergVector("7 6S 1")
        << BloombergVector("A 05/30/2015 12 6S 6 6S 4")
        ;
    QTest::newRow("Steps same anchor same divisor")
        << BloombergVector("A 05/30/2015 5 12S 3")
        << BloombergVector("A 05/30/2015 7 6S 1")
        << BloombergVector("A 05/30/2015 12 6S 6 6S 4")
        ;
    QTest::newRow("Steps different anchor same divisor")
        << BloombergVector("A 05/30/2015 5 12S 3")
        << BloombergVector("A 05/30/2014 7 6S 1")
        << BloombergVector("A 05/30/2015 6 12S 4")
        ;
    
    // Ramps
    QTest::newRow("Ramps no anchor same divisor")
        << BloombergVector("5 2R 3")
        << BloombergVector("7 6S 1")
        << BloombergVector("12 2R 10 4S 4")
        ;
    QTest::newRow("Steps one anchor same divisor")
        << BloombergVector("A 05/30/2015 5 2R 3")
        << BloombergVector("7 6S 1")
        << BloombergVector("A 05/30/2015 12 2R 10 4S 4")
        ;
    QTest::newRow("Steps same anchor same divisor")
        << BloombergVector("A 05/30/2015 5 2R 3")
        << BloombergVector("A 05/30/2015 7 6S 1")
        << BloombergVector("A 05/30/2015 12 2R 10 4S 4")
        ;
    QTest::newRow("Steps different anchor same divisor")
        << BloombergVector("A 05/30/2015 5 2R 3")
        << BloombergVector("A 05/30/2014 7 6S 1")
        << BloombergVector("A 05/30/2015 6 2R 4")
        ;
    {
        BloombergVector tempVector;
        tempVector.SetDivisor(10.0);
        tempVector.SetVector("5");
        QTest::newRow("Single value no anchor different divisor")
            << tempVector
            << BloombergVector("7")
            << BloombergVector("57")
            ;
        tempVector.SetVector("A 05/30/2015 5");
        QTest::newRow("Single value one anchor different divisor")
            << tempVector
            << BloombergVector("7")
            << BloombergVector("A 05/30/2015 57")
            ;
        QTest::newRow("Single value same anchor different divisor")
            << tempVector
            << BloombergVector("A 05/30/2015 7")
            << BloombergVector("A 05/30/2015 57")
            ;
        QTest::newRow("Single value different anchor different divisor")
            << tempVector
            << BloombergVector("A 05/30/2014 7")
            << BloombergVector("A 05/30/2015 57")
            ;
    }

    
}
namespace QTest {
    char *toString(const BloombergVector &val)
    {
        return qstrdup(val.GetVector().toLatin1().data());
    }
}