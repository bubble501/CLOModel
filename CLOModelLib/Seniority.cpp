#include "Seniority.h"
#include "Seniority_p.h"
#include <QStringList>

void Seniority::Clear()
{
    Q_D(Seniority);
    d->m_SeniorityScale.clear();
    d->m_GroupScale.clear();
    d->m_RankScale.clear();
}



quint32 Seniority::GetSeniority(int level) const
{
    Q_D(const Seniority);
    return d->genericGet(level, d->m_SeniorityScale);
}

quint32 Seniority::GetGroup(int level) const
{
    Q_D(const Seniority);
    return d->genericGet(level, d->m_GroupScale);
}

quint32 Seniority::GetRank(int level) const
{
    Q_D(const Seniority);
    return d->genericGet(level, d->m_RankScale);
}

bool Seniority::AddSeniorityLevel(quint32 sen, quint32 gr, quint32 rnk)
{
    Q_D(Seniority);
    if (sen == 0 || gr == 0 || rnk == 0) return false;
    d->m_SeniorityScale.append(sen);
    d->m_GroupScale.append(gr);
    d->m_RankScale.append(rnk);
    return true;
}

bool Seniority::SetSeniorityScale(const QString& a)
{
    Clear();
    if (a.isEmpty()) return false;
    const QStringList senLvls = a.split(static_cast<char>(SeniorityPrivate::SeniorityStringSeparator),QString::SkipEmptyParts);
    const QString validNumber("0*[1-9]\\d*");
    QRegExp singleLvlRX("^(" + validNumber + ")(?:G(" + validNumber + ")(?:R(" + validNumber + ")?)?)?$");
    for (auto i = senLvls.constBegin(); i!=senLvls.constEnd();++i){
        if (!singleLvlRX.exactMatch(*i)) {
            Clear();
            return false;
        }
        AddSeniorityLevel(
            singleLvlRX.cap(1).toUInt()
            , singleLvlRX.cap(2).isEmpty()  ? 1:singleLvlRX.cap(2).toUInt() 
            , singleLvlRX.cap(3).isEmpty()  ? 1:singleLvlRX.cap(3).toUInt() 
            );
    }
    return true;
}
QDataStream& operator<<(QDataStream & stream, const Seniority& flows)
{
    stream
        << flows.d_func()->m_SeniorityScale
        << flows.d_func()->m_GroupScale
        << flows.d_func()->m_RankScale
        ;
    return stream;
}
QDataStream& Seniority::LoadOldVersion(QDataStream& stream)
{
    Q_D(Seniority);
    stream
        >> d->m_SeniorityScale
        >> d->m_GroupScale
        >> d->m_RankScale
        ;
    ResetProtocolVersion();
    return stream;
}

QString Seniority::ToString() const
{
    Q_D(const Seniority);
    QString Result = "";
    for (int i = 0; i < d->m_SeniorityScale.size(); ++i) {
        if (!Result.isEmpty()) Result.append(static_cast<char>(SeniorityPrivate::SeniorityStringSeparator));
        Result += QString::number(d->m_SeniorityScale.at(i));
        if (d->m_GroupScale.at(i) > 1 || d->m_RankScale.at(i) > 1) {
            Result += 'G' + QString::number(d->m_GroupScale.at(i));
            if (d->m_RankScale.at(i) > 1)
                Result += 'R' + QString::number(d->m_RankScale.at(i));
        }
    }
    return Result;
}

Seniority::Seniority()
    :BackwardInterface(new SeniorityPrivate(this))
{}

Seniority::Seniority(const QString& a)
    :Seniority()
{
    SetSeniorityScale(a);
}

Seniority::Seniority(SeniorityPrivate* d)
    :BackwardInterface(d)
{}

Seniority::Seniority(const Seniority& a)
    : BackwardInterface(new SeniorityPrivate(this,*a.d_func()))
{}

Seniority& Seniority::operator=(const Seniority& a)
{
    Q_D(Seniority);
    d->operator=(*a.d_func());
    return *this;
}

bool Seniority::isValid() const
{
    Q_D(const Seniority);
    return !d->m_SeniorityScale.isEmpty();
}

quint32 SeniorityPrivate::genericGet(int level, const QList<quint32>& cont) const
{
    if (level < 0) return 0;
    if (level >= cont.size()) return cont.last();
    return  cont.at(level);
}

QDataStream& operator>>(QDataStream & stream, Seniority& flows)
{
    return flows.LoadOldVersion(stream);
}
SeniorityPrivate::SeniorityPrivate(Seniority* q)
    :BackwardInterfacePrivate(q)
{}

SeniorityPrivate::SeniorityPrivate(Seniority* q, const SeniorityPrivate& other)
    : BackwardInterfacePrivate(q, other)
    , m_SeniorityScale(other.m_SeniorityScale)
    , m_GroupScale(other.m_GroupScale)
    , m_RankScale(other.m_RankScale)
{}

SeniorityPrivate& SeniorityPrivate::operator=(const SeniorityPrivate& other)
{
    BackwardInterfacePrivate::operator=(other);
    m_SeniorityScale = other.m_SeniorityScale;
    m_GroupScale = other.m_GroupScale;
    m_RankScale = other.m_RankScale;
    return *this;
}
