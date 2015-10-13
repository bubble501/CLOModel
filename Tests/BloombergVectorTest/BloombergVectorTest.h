#ifndef BLOOMBERGVECTORTEST_H
#define BLOOMBERGVECTORTEST_H

#include <QObject>

class BloombergVectorTest : public QObject
{
    Q_OBJECT
private slots:
    void testSum();
    void testSum_data();
    
};

#endif // BLOOMBERGVECTORTEST_H
