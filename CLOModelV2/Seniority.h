#include "BackwardCompatibilityInterface.h"
#include <QList>
class Seniority : public BackwardInterface
{
    QList<quint32> m_SeniorityScale;
    QList<quint32> m_GroupScale;
    QList<quint32> m_RankScale;
public:
    void Clear();
    quint32 GetSeniority(int level) const { return genericGet(level, m_SeniorityScale); }
    quint32 GetGroup(int level) const { return genericGet(level, m_GroupScale); }
    quint32 GetRank(int level) const { return genericGet(level, m_RankScale); }
    bool AddSeniorityLevel(quint32 sen, quint32 gr = 1, quint32 rnk = 1);
    bool SetSeniorityScale(const QString& a);
    QString ToString() const;
    Seniority() {}
    Seniority(const QString& a) { SetSeniorityScale(a); }
    bool isValid() const { return !m_SeniorityScale.isEmpty(); }
protected:
    enum { SeniorityStringSeparator = '-' };
    quint32 genericGet(int level, const QList<quint32>& cont) const;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    friend QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
    friend QDataStream& operator>>(QDataStream & stream, Seniority& flows);
};
QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
QDataStream& operator>>(QDataStream & stream, Seniority& flows);

