#ifndef BaseRateVect_p_h__
#define BaseRateVect_p_h__
#include <type_traits>
#include "AbstractBbgVect_p.h"
#include "BaseRateVect.h"
#include <QStringList>
class BaseRateVectorPrivate : public AbstractBbgVectPrivate
{
    Q_DECLARE_PUBLIC(BaseRateVector)
private:
    BaseRateVectorPrivate(const BaseRateVectorPrivate& other);
public:
    BaseRateVectorPrivate(BaseRateVector* q);
    BaseRateVectorPrivate(BaseRateVector* q, const BaseRateVectorPrivate& other);
    virtual BaseRateVectorPrivate& operator=(const BaseRateVectorPrivate& other);
protected:
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
    friend QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows);
    friend QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows);
};
#endif // BaseRateVect_p_h__