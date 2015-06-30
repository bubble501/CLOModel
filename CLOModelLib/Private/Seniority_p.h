#ifndef Seniority_p_h__
#define Seniority_p_h__

#include "Seniority.h"
#include <QList>
#include "Private/BackwardCompatibilityInterface_p.h"
class SeniorityPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(Seniority)
    DECLARE_PRIVATE_COMMONS_COPY(Seniority)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(Seniority)
public:
    QList<quint32> m_SeniorityScale;
    QList<quint32> m_GroupScale;
    QList<quint32> m_RankScale;
    enum { SeniorityStringSeparator = '-' };
    quint32 genericGet(int level, const QList<quint32>& cont) const;
    friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
    friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Seniority& flows);
};
#endif // Seniority_p_h__
