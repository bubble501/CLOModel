#ifndef Seniority_h__
#define Seniority_h__

#include "BackwardCompatibilityInterface.h"
#include <QList>
class SeniorityPrivate;
class CLOMODELLIB_EXPORT Seniority : public BackwardInterface
{
    Q_DECLARE_PRIVATE(Seniority)
public:
    void Clear();
    quint32 GetSeniority(int level) const;
    quint32 GetGroup(int level) const;
    quint32 GetRank(int level) const;
    bool AddSeniorityLevel(quint32 sen, quint32 gr = 1, quint32 rnk = 1);
    bool SetSeniorityScale(const QString& a);
    QString ToString() const;
    Seniority();
    Seniority(const QString& a);
    Seniority(const Seniority& a);
    Seniority& operator=(const Seniority& a);
    bool isValid() const;
protected:
    Seniority(SeniorityPrivate* d);
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    friend QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
    friend QDataStream& operator>>(QDataStream & stream, Seniority& flows);
};
QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
QDataStream& operator>>(QDataStream & stream, Seniority& flows);

#endif // Seniority_h__
