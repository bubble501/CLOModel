#ifndef BloombergVector_p_h__
#define BloombergVector_p_h__
#include "BloombergVector.h"
#include "AbstractBbgVect_p.h"
class BloombergVectorPrivate : public AbstractBbgVectPrivate
{
    Q_DECLARE_PUBLIC(BloombergVector)
private:
    BloombergVectorPrivate(const BloombergVectorPrivate& other);
public:
    BloombergVectorPrivate(BloombergVector* q);
    BloombergVectorPrivate(BloombergVector* q, const BloombergVectorPrivate& other);
    virtual BloombergVectorPrivate& operator=(const BloombergVectorPrivate& other);
protected:
    QList<double> m_VectVal;
    double m_Divisor;
    friend QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
    friend QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
};



#endif // BloombergVector_p_h__