#ifndef AbstractBbgVect_p_h__
#define AbstractBbgVect_p_h__

#define VectorAnchorDateFormat "(\\d{1,2})/(\\d{1,2})/(\\d{4})"
#include "AbstractBbgVect.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QDate>
class AbstractBbgVectPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(AbstractBbgVect)
public:
    QString m_Vector;
    QDate m_AnchorDate;
    template<class T> T GetValueTemplate(const QList<T>& VecVal, const QDate& index, const T& DefaultValue) const
    {
        QDate ValidDate(m_AnchorDate);
        if (m_AnchorDate.isNull()) {
            ValidDate = QDate::currentDate();
            //LOGDEBUG("Anchor defaulted to today\n");
        }
        if (index < m_AnchorDate) {
            //LOGDEBUG("Requested date before Anchor\n");
            return VecVal.first();
        }
        return GetValueTemplate(VecVal, MonthDiff(index, ValidDate), DefaultValue);
    }
    template<class T> T GetValueTemplate(const QList<T>& VecVal, int index, const T& DefaultValue)const
    {
        if (VecVal.isEmpty() || index < 0) {
            //LOGDEBUG("Requested value with index negative or empty vector\n");
            return DefaultValue;
        }
        return VecVal.at(qMin(index, VecVal.size() - 1));
    }
};



#endif // AbstractBbgVect_p_h__



