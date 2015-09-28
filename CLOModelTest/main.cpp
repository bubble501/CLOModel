
#include <QCoreApplication>
#include <QTest>
#include "RatingsTest.h"
#include "BloombergVectorTest.h"
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv); 
    RatingsTest ratingsTest;
    BloombergVectorTest bloombergVectorTest;
    return 
       QTest::qExec(&ratingsTest, argc, argv) |
       QTest::qExec(&bloombergVectorTest, argc, argv)
        ;
}
