
#include <QCoreApplication>
#include <QTest>
#include "RatingsTest.h"
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv); 
    RatingsTest ratingsTest;
    return QTest::qExec(&ratingsTest, argc, argv);
}
#include "RatingsTest.moc"
