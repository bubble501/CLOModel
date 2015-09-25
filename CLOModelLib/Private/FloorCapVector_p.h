#include "FloorCapVector.h"
#include "AbstractBbgVect_p.h"
class FloorCapVectorPrivate : public AbstractBbgVectPrivate
{
    DECLARE_PRIVATE_COMMONS(FloorCapVector)
    DECLARE_PRIVATE_COMMONS_COPY(FloorCapVector)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(FloorCapVector)
public:
    template<class T> std::shared_ptr<double> GetFloor(const T& index) const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetFloor can be used only with int or QDate");
        return GetValueTemplate(m_FloorVal, index, std::shared_ptr<double>(nullptr));
    }

    template<class T> std::shared_ptr<double> GetCap(const T& index) const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetCap can be used only with int or QDate");
        return GetValueTemplate(m_CapVal, index, std::shared_ptr<double>(nullptr));
    }
    QList<std::shared_ptr<double> > m_FloorVal;
    QList<std::shared_ptr<double> > m_CapVal;
};