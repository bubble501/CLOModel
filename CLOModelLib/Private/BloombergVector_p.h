#ifndef BloombergVector_p_h__
#define BloombergVector_p_h__
#include "BloombergVector.h"
#include "AbstractBbgVect_p.h"
class BloombergVectorPrivate : public AbstractBbgVectPrivate
{
    DECLARE_PRIVATE_COMMONS(BloombergVector)
    DECLARE_PRIVATE_COMMONS_COPY(BloombergVector)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(BloombergVector)
public:
    QList<double> m_VectVal;
    double m_Divisor;
};



#endif // BloombergVector_p_h__