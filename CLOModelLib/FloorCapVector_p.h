#include "FloorCapVector.h"
#include "AbstractBbgVect_p.h"
class FloorCapVectorPrivate : public AbstractBbgVectPrivate
{
    Q_DECLARE_PUBLIC(FloorCapVector)
private:
    FloorCapVectorPrivate(const FloorCapVectorPrivate& other);
public:
    FloorCapVectorPrivate(FloorCapVector* q);
    FloorCapVectorPrivate(FloorCapVector* q, const FloorCapVectorPrivate& other);
    virtual FloorCapVectorPrivate& operator=(const FloorCapVectorPrivate& other);
    template<class T> QSharedPointer<double> GetFloor(const T& index) const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetFloor can be used only with int or QDate");
        return GetValueTemplate(m_FloorVal, index, QSharedPointer<double>(NULL));
    }

    template<class T> QSharedPointer<double> GetCap(const T& index) const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetCap can be used only with int or QDate");
        return GetValueTemplate(m_CapVal, index, QSharedPointer<double>(NULL));
    }
    QList<QSharedPointer<double> > m_FloorVal;
    QList<QSharedPointer<double> > m_CapVal;
    friend QDataStream& operator<<(QDataStream & stream, const FloorCapVector& flows);
    friend QDataStream& operator>>(QDataStream & stream, FloorCapVector& flows);
};