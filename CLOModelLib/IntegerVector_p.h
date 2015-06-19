#ifndef IntegerVector_p_h__
#define IntegerVector_p_h__

#include "AbstractBbgVect_p.h"
#include "IntegerVector.h"
class IntegerVectorPrivate : public AbstractBbgVectPrivate
{
    DECLARE_PRIVATE_COMMONS(IntegerVector)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(IntegerVector)
public:
    QList<int> m_VectVal;
    qint32 m_Shift;
};
#endif // IntegerVector_p_h__