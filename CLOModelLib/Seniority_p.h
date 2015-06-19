#ifndef Seniority_p_h__
#define Seniority_p_h__

#include "Seniority.h"
#include "BackwardCompatibilityInterface_p.h"
class SeniorityPrivate : public BackwardInterfacePrivate
{
    Q_DECLARE_PUBLIC(Seniority)
private:
    SeniorityPrivate(const SeniorityPrivate& other);
public:
    SeniorityPrivate(Seniority* q);
    SeniorityPrivate(Seniority* q, const SeniorityPrivate& other);
    virtual SeniorityPrivate& operator=(const SeniorityPrivate& other);
protected:
    QList<quint32> m_SeniorityScale;
    QList<quint32> m_GroupScale;
    QList<quint32> m_RankScale;
    enum { SeniorityStringSeparator = '-' };
    quint32 genericGet(int level, const QList<quint32>& cont) const;
    friend QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
    friend QDataStream& operator>>(QDataStream & stream, Seniority& flows);
};
#endif // Seniority_p_h__
