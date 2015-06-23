
#include <QtCore/QCoreApplication>
#include <Tranche.h>
#include <QDebug>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Tranche testTranche;
    testTranche.SetTrancheName("MAGEL 3 A");
    testTranche.SetISIN("XS0222684655");
    testTranche.SetBloombergExtension(" Mtge");
    testTranche.GetDataFromBloomberg();
    testTranche.GetCashFlowsFromBloomberg();
    qDebug() << testTranche.GetDiscountMargin(100);
    return a.exec();
}
