#ifndef BaseRateVect_p_h__
#define BaseRateVect_p_h__
#include <type_traits>
#include "AbstractBbgVect_p.h"
#include "BaseRateVect.h"
#include <QStringList>
class BaseRateVectorPrivate : public AbstractBbgVectPrivate
{
    DECLARE_PRIVATE_COMMONS(BaseRateVector)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(BaseRateVector)
public:
    QStringList m_VectVal;
    template<class T> QString GetValue(const T& index) const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetValue can be used only with int or QDate");
        QString RawVal = GetValueTemplate(m_VectVal, index, QString()).trimmed();
        RawVal.replace(QRegExp("\\[(?:-?\\d*\\.?\\d+)?(?:,-?\\d*\\.?\\d+)?\\]"), "");
        return RawVal;
    }
    template<class T> QString GetFloor(const T& index) const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetFloor can be used only with int or QDate");
        QString RawVal = GetValueTemplate(m_VectVal, index, QString()).trimmed();
        QRegExp CaptureFloor("\\[(-?\\d*\\.?\\d+)?(?:,-?\\d*\\.?\\d+)?\\]");
        if (CaptureFloor.indexIn(RawVal) < 0) return QString();
        return CaptureFloor.cap(1);
    }
    template<class T> QString GetCap(const T& index) const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetCap can be used only with int or QDate");
        QString RawVal = GetValueTemplate(m_VectVal, index, QString()).trimmed();
        QRegExp CaptureFloor("\\[(?:-?\\d*\\.?\\d+)?,(-?\\d*\\.?\\d+)?\\]");
        if (CaptureFloor.indexIn(RawVal) < 0) return QString();
        return CaptureFloor.cap(1);
    }
};
#endif // BaseRateVect_p_h__