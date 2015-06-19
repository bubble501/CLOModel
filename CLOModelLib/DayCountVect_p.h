
#ifndef DayCountVect_p_h__
#define DayCountVect_p_h__
#include "AbstractBbgVect_p.h"
#include "DayCountVect.h"
class DayCountVectorPrivate : public AbstractBbgVectPrivate
{
    Q_DECLARE_PUBLIC(DayCountVector)
private:
    DayCountVectorPrivate(const DayCountVectorPrivate& other);
public:
    DayCountVectorPrivate(DayCountVector* q);
    DayCountVectorPrivate(DayCountVector* q, const DayCountVectorPrivate& other);
    virtual DayCountVectorPrivate& operator=(const DayCountVectorPrivate& other);
    QList<DayCountConvention> m_VectVal;
    friend QDataStream& operator<<(QDataStream & stream, const DayCountVector& flows);
    friend QDataStream& operator>>(QDataStream & stream, DayCountVector& flows);
};
#endif // DayCountVect_p_h__
