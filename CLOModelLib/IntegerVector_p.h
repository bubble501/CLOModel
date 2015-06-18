#include "AbstractBbgVect_p.h"
#include "IntegerVector.h"
class IntegerVectorPrivate : public AbstractBbgVectPrivate
{
    Q_DECLARE_PUBLIC(IntegerVector)
private:
    IntegerVectorPrivate(const IntegerVectorPrivate& other);
public:
    IntegerVectorPrivate(IntegerVector* q);
    IntegerVectorPrivate(IntegerVector* q, const IntegerVectorPrivate& other);
    virtual IntegerVectorPrivate& operator=(const IntegerVectorPrivate& other);
protected:
    QList<int> m_VectVal;
    qint32 m_Shift;
    friend QDataStream& operator<<(QDataStream & stream, const IntegerVector& flows);
    friend QDataStream& operator>>(QDataStream & stream, IntegerVector& flows);
};