#ifndef BoolVector_p_h__
#define BoolVector_p_h__
#include "AbstractBbgVect_p.h"
#include "BoolVector.h"
class BoolVectorPrivate : public AbstractBbgVectPrivate
{
    Q_DECLARE_PUBLIC(BoolVector)
private:
    BoolVectorPrivate(const BoolVectorPrivate& other);
public:
    BoolVectorPrivate(BoolVector* q);
    BoolVectorPrivate(BoolVector* q, const BoolVectorPrivate& other);
    virtual BoolVectorPrivate& operator=(const BoolVectorPrivate& other);
    QList<bool> m_VectVal;
    friend QDataStream& operator<<(QDataStream & stream, const BoolVector& flows);
    friend QDataStream& operator>>(QDataStream & stream, BoolVector& flows);
};
#endif // BoolVector_p_h__