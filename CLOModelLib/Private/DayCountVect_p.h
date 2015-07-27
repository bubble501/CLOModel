
#ifndef DayCountVect_p_h__
#define DayCountVect_p_h__
#include "AbstractBbgVect_p.h"
#include "DayCountVect.h"
class DayCountVectorPrivate : public AbstractBbgVectPrivate
{
    DECLARE_PRIVATE_COMMONS(DayCountVector)
    DECLARE_PRIVATE_COMMONS_COPY(DayCountVector)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(DayCountVector)
public:
    QList<DayCountConvention> m_VectVal;
    QString getValidString() const;
};



#endif // DayCountVect_p_h__
