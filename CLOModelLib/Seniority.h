#ifndef Seniority_h__
#define Seniority_h__

#include "BackwardCompatibilityInterface.h"
class SeniorityPrivate;
class CLOMODELLIB_EXPORT Seniority : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(Seniority)
    DECLARE_PUBLIC_COMMONS_COPY(Seniority)
public:
    void Clear();
    quint32 GetSeniority(int level) const;
    quint32 GetGroup(int level) const;
    quint32 GetRank(int level) const;
    bool AddSeniorityLevel(quint32 sen, quint32 gr = 1, quint32 rnk = 1);
    bool SetSeniorityScale(const QString& a);
    QString ToString() const;
    Seniority(const QString& a);
    bool isValid() const;
protected:
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
    friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Seniority& flows);
};
CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Seniority& flows);

#endif // Seniority_h__
