#ifndef BoolVector_p_h__
#define BoolVector_p_h__
#include "AbstractBbgVect_p.h"
#include "BoolVector.h"
class BoolVectorPrivate : public AbstractBbgVectPrivate
{
    DECLARE_PRIVATE_COMMONS(BoolVector)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(BoolVector)
public:
    QList<bool> m_VectVal;
};
#endif // BoolVector_p_h__