#include "Seniority.h"
#include <QStringList>

void Seniority::Clear()
{
    m_SeniorityScale.clear();
    m_GroupScale.clear();
    m_RankScale.clear();
}



bool Seniority::AddSeniorityLevel(quint32 sen, quint32 gr, quint32 rnk)
{
    if (sen == 0 || gr == 0 || rnk == 0) return false;
    m_SeniorityScale.append(sen);
    m_GroupScale.append(gr);
    m_RankScale.append(rnk);
    return true;
}

bool Seniority::SetSeniorityScale(const QString& a)
{
    Clear();
    if (a.isEmpty()) return false;
    const QString validNumber("0*[1-9]\\d*");
    QRegExp seniorityRX('(' + validNumber + "(?:G" + validNumber + "(?:R" + validNumber + ")?)?)(?:"
        + static_cast<char>(SeniorityStringSeparator)
        +'(' + validNumber + "(?:G" + validNumber + "(?:R" + validNumber + ")?)?))*");
    QRegExp singleLvlRX('(' + validNumber + ")(?:G(" + validNumber + ")(?:R(" + validNumber + ")?)?");
    if (!seniorityRX.exactMatch(a.trimmed())) return false;
    for (int i = 1; i < seniorityRX.captureCount(); ++i) {
        singleLvlRX.indexIn(seniorityRX.cap(i));
        AddSeniorityLevel(
            singleLvlRX.cap(1).toUInt()
            , singleLvlRX.captureCount() > 2 ? singleLvlRX.cap(2).toUInt() : 1
            , singleLvlRX.captureCount() > 3 ? singleLvlRX.cap(3).toUInt() : 1
            );
    }
    return true;
}
QDataStream& operator<<(QDataStream & stream, const Seniority& flows)
{
    stream
        << flows.m_SeniorityScale
        << flows.m_GroupScale
        << flows.m_RankScale
        ;
    return stream;
}
QDataStream& Seniority::LoadOldVersion(QDataStream& stream)
{
    stream
        >> m_SeniorityScale
        >> m_GroupScale
        >> m_RankScale
        ;
    ResetProtocolVersion();
    return stream;
}

QString Seniority::ToString() const
{
    QString Result = "";
    for (int i = 0; i < m_SeniorityScale.size(); ++i) {
        if (!Result.isEmpty()) Result.append(static_cast<char>(SeniorityStringSeparator));
        Result += QString::number(m_SeniorityScale.at(i));
        if (m_GroupScale.at(i) > 1) {
            Result += 'G' + QString::number(m_GroupScale.at(i));
            if (m_RankScale.at(i) > 1)
                Result += 'R' + QString::number(m_RankScale.at(i));
        }
    }
    return Result;
}

quint32 Seniority::genericGet(int level, const QList<quint32>& cont) const
{
    if (level < 0) return 0;
    if (level >= cont.size()) return cont.last();
    return  cont.at(level);
}

QDataStream& operator>>(QDataStream & stream, Seniority& flows)
{
    return flows.LoadOldVersion(stream);
}