#include "FloorCapVector.h"
#include "FloorCapVector_p.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
FloorCapVector::FloorCapVector()
    :AbstractBbgVect(new FloorCapVectorPrivate(this))
{}
FloorCapVector::FloorCapVector(const FloorCapVector& other)
    : AbstractBbgVect(new FloorCapVectorPrivate(this,*other.d_func()))
{}
FloorCapVector::FloorCapVector(const QString& Vec)
    :FloorCapVector()
{
    SetVector(Vec);
}

QSharedPointer<double> FloorCapVector::GetFloor(const QDate& index) const
{
    Q_D(const FloorCapVector);
    return d->GetFloor(index);
}
QSharedPointer<double> FloorCapVector::GetFloor(int index) const
{
    Q_D(const FloorCapVector);
    return d->GetFloor(index);
}
QSharedPointer<double> FloorCapVector::GetCap(const QDate& index) const
{
    Q_D(const FloorCapVector);
    return d->GetCap(index);
}
QSharedPointer<double> FloorCapVector::GetCap(int index) const
{
    Q_D(const FloorCapVector);
    return d->GetCap(index);
}

int FloorCapVector::NumElements() const
{
    Q_D(const FloorCapVector);
    return d->m_FloorVal.size();
}

FloorCapVector& FloorCapVector::operator=(const QString& a)
{
    AbstractBbgVect::operator=(a); return *this;
}

FloorCapVector& FloorCapVector::operator=(const FloorCapVector& Vec)
{
    Q_D(FloorCapVector);
    d->operator=(*Vec.d_func());
    return *this;

}
FloorCapVector::FloorCapVector(const QString& Vec, const QDate& Anchor)
    :FloorCapVector()
{
    SetVector(Vec);
    SetAnchorDate(Anchor);
}
void FloorCapVector::UnpackVector()
{
    Q_D(FloorCapVector);
    d->m_FloorVal.clear();
    d->m_CapVal.clear();
    if (d->m_Vector.isEmpty()) return;
    ExtractAnchorDate();
    QString TempVec(d->m_Vector.trimmed().toUpper());
    QStringList StringParts = TempVec.trimmed().toUpper().split(QRegExp("\\s"), QString::SkipEmptyParts);
    int StepLen;
    QString TempStr;
    for (int i = 1; i < StringParts.size(); i += 2) {
        TempStr = StringParts.at(i);
        TempStr.replace(QRegExp("\\D"), "");
        StepLen = TempStr.toInt();
        TempStr = StringParts.at(i);
        TempStr.replace(QRegExp("\\d"), "");
        for (int j = 0; j < StepLen; j++) {
            QString RawVal = StringParts.at(i - 1);
            RawVal.replace("[", "");
            RawVal.replace("]", "");
            auto CapFloor = RawVal.split(',', QString::KeepEmptyParts);
            if (CapFloor.size() > 0) {
                if (CapFloor.at(0).isEmpty()) d->m_FloorVal.append(QSharedPointer<double>(NULL));
                else d->m_FloorVal.append(QSharedPointer<double>(new double(CapFloor.at(0).toDouble())));
            }
            else d->m_FloorVal.append(QSharedPointer<double>(NULL));
            if (CapFloor.size() > 1) {
                if (CapFloor.at(1).isEmpty()) d->m_CapVal.append(QSharedPointer<double>(NULL));
                else d->m_CapVal.append(QSharedPointer<double>(new double(CapFloor.at(1).toDouble())));
            }
            else d->m_CapVal.append(QSharedPointer<double>(NULL));
        }
    }
    {
        QString RawVal = StringParts.last();
        RawVal.replace("[", "");
        RawVal.replace("]", "");
        auto CapFloor = RawVal.split(',', QString::KeepEmptyParts);
        if (CapFloor.size() > 0) {
            if (CapFloor.at(0).isEmpty()) d->m_FloorVal.append(QSharedPointer<double>(NULL));
            else d->m_FloorVal.append(QSharedPointer<double>(new double(CapFloor.at(0).toDouble())));
        }
        else d->m_FloorVal.append(QSharedPointer<double>(NULL));
        if (CapFloor.size() > 1) {
            if (CapFloor.at(1).isEmpty()) d->m_CapVal.append(QSharedPointer<double>(NULL));
            else d->m_CapVal.append(QSharedPointer<double>(new double(CapFloor.at(1).toDouble())));
        }
        else d->m_CapVal.append(QSharedPointer<double>(NULL));
    }
}
bool FloorCapVector::IsValid() const
{
    Q_D(const FloorCapVector);
    if (!AbstractBbgVect::IsValid(R"**(\[(?:(?:-?\d*\.?\d+)|(?:(?:-?\d*\.?\d+)?(?:,-?\d*\.?\d+)))\])**", false)) return false;
    QRegExp rx("\\[(-?\\d*\\.?\\d+),(-?\\d*\\.?\\d+)\\]");
    for (int pos = 0; (pos = rx.indexIn(d->m_Vector, pos)) >= 0; pos += rx.matchedLength()) {
        if (rx.cap(1).toDouble() > rx.cap(2).toDouble()) return false;
    }
    return true;
}
QRegExpValidator* FloorCapVector::GetValidator(QObject* parent) const
{
    return AbstractBbgVect::GetValidator("\\[(?:-?\\d*\\.?\\d+)?(?:,-?\\d*\\.?\\d+)?\\]", false, parent);
}

QDataStream& operator<<(QDataStream & stream, const FloorCapVector& flows)
{
    stream << flows.d_func()->m_Vector;
    stream << flows.d_func()->m_AnchorDate;
    return stream;
}
QDataStream& operator>>(QDataStream & stream, FloorCapVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& FloorCapVector::LoadOldVersion(QDataStream& stream)
{
    Q_D(FloorCapVector);
    stream
        >> d->m_Vector
        >> d->m_AnchorDate
        ;
    UnpackVector();
    ResetProtocolVersion();
    return stream;
}


FloorCapVectorPrivate::FloorCapVectorPrivate(FloorCapVector* q, const FloorCapVectorPrivate& other)
    :AbstractBbgVectPrivate(q, other)
{
    m_FloorVal.clear();
    m_CapVal.clear();
    foreach(const QSharedPointer<double>& SingleFloor, other.m_FloorVal)
    {
        if (SingleFloor) m_FloorVal.append(QSharedPointer<double>(new double(*SingleFloor)));
        else m_FloorVal.append(QSharedPointer<double>(NULL));
    }
    foreach(const QSharedPointer<double>& SingleCap, other.m_CapVal)
    {
        if (SingleCap) m_CapVal.append(QSharedPointer<double>(new double(*SingleCap)));
        else  m_CapVal.append(QSharedPointer<double>(NULL));
    }
}

FloorCapVectorPrivate::FloorCapVectorPrivate(FloorCapVector* q)
    :AbstractBbgVectPrivate(q)
{}

FloorCapVectorPrivate& FloorCapVectorPrivate::operator=(const FloorCapVectorPrivate& other)
{
    AbstractBbgVectPrivate::operator=(other);
    m_FloorVal.clear();
    m_CapVal.clear();
    foreach(const QSharedPointer<double>& SingleFloor, other.m_FloorVal)
    {
        if (SingleFloor) m_FloorVal.append(QSharedPointer<double>(new double(*SingleFloor)));
        else m_FloorVal.append(QSharedPointer<double>(NULL));
    }
    foreach(const QSharedPointer<double>& SingleCap, other.m_CapVal)
    {
        if (SingleCap) m_CapVal.append(QSharedPointer<double>(new double(*SingleCap)));
        else  m_CapVal.append(QSharedPointer<double>(NULL));
    }
    return *this;
}

