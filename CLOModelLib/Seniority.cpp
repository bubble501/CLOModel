#include "Seniority.h"
#include "Private/Seniority_p.h"
#include <QStringList>
#include <QRegularExpression>
DEFINE_PUBLIC_COMMONS(Seniority)
DEFINE_PUBLIC_COMMONS_COPY(Seniority)
SeniorityPrivate::SeniorityPrivate(Seniority *q,const SeniorityPrivate& other)
	:BackwardInterfacePrivate(q,other)
    , m_SeniorityScale(other.m_SeniorityScale)
    , m_GroupScale(other.m_GroupScale)
    , m_RankScale(other.m_RankScale)
{}
SeniorityPrivate::SeniorityPrivate(Seniority *q)
	:BackwardInterfacePrivate(q)
{}
Seniority::Seniority(SeniorityPrivate *d, const Seniority& other)
	:BackwardInterface(d,other)
{}
Seniority::Seniority(SeniorityPrivate *d)
	:BackwardInterface(d)
{}
Seniority& Seniority::operator=(const Seniority& other)
{
    BackwardInterface::operator=(other);
    Q_D(Seniority);
    d->m_SeniorityScale = other.d_func()->m_SeniorityScale;
    d->m_GroupScale = other.d_func()->m_GroupScale;
    d->m_RankScale = other.d_func()->m_RankScale;
    return *this;
}
void Seniority::Clear()
{
    Q_D(Seniority);
    d->m_SeniorityScale.clear();
    d->m_GroupScale.clear();
    d->m_RankScale.clear();
}

qint32 Seniority::GetSeniority(int level) const
{
    Q_D(const Seniority);
    return d->genericGet(level, d->m_SeniorityScale);
}

qint32 Seniority::GetGroup(int level) const
{
    Q_D(const Seniority);
    return d->genericGet(level, d->m_GroupScale);
}

qint32 Seniority::GetRank(int level) const
{
    Q_D(const Seniority);
    return d->genericGet(level, d->m_RankScale);
}

bool Seniority::AddSeniorityLevel(qint32 sen, qint32 gr, qint32 rnk)
{
    Q_D(Seniority);
    if (sen <= 0 || gr <= 0 || rnk <= 0)
        return false;
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
    QRegularExpression singleLvlRX("^(" + validNumber + ")(?:G(" + validNumber + ")(?:R(" + validNumber + ")?)?)?$");
    Q_ASSERT(singleLvlRX.isValid());
    for (auto i = senLvls.constBegin(); i!=senLvls.constEnd();++i){
        const auto singleLvlRXMatch = singleLvlRX.match(*i);
        if (!singleLvlRXMatch.hasMatch()) {
            Clear();
            return false;
        }
        AddSeniorityLevel(
            singleLvlRXMatch.captured(1).toUInt()
            , singleLvlRXMatch.captured(2).isEmpty() ? 1 : singleLvlRXMatch.captured(2).toUInt()
            , singleLvlRXMatch.captured(3).isEmpty() ? 1 : singleLvlRXMatch.captured(3).toUInt()
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



Seniority::Seniority(const QString& a)
    :Seniority()
{
    SetSeniorityScale(a);
}

bool Seniority::isValid() const
{
    Q_D(const Seniority);
    return !d->m_SeniorityScale.isEmpty();
}

qint32 SeniorityPrivate::genericGet(int level, const QList<qint32>& cont) const
{
    if (level < 0) return 0;
    if (level >= cont.size()) return cont.last();
    return  cont.at(level);
}

QDataStream& operator>>(QDataStream & stream, Seniority& flows)
{
    return flows.LoadOldVersion(stream);
}

