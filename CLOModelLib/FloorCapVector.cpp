#include "FloorCapVector.h"
#include "Private/FloorCapVector_p.h"
#include <QRegularExpression>
#include <QStringList>
#include "CommonFunctions.h"
DEFINE_PUBLIC_COMMONS(FloorCapVector)
DEFINE_PUBLIC_COMMONS_COPY(FloorCapVector)
FloorCapVectorPrivate::FloorCapVectorPrivate(FloorCapVector *q,const FloorCapVectorPrivate& other)
	:AbstractBbgVectPrivate(q,other)
{}
FloorCapVectorPrivate::FloorCapVectorPrivate(FloorCapVector *q)
	:AbstractBbgVectPrivate(q)
{}
FloorCapVector::FloorCapVector(FloorCapVectorPrivate *d, const FloorCapVector& other)
	:AbstractBbgVect(d,other)
{
    operator=(other);
}
FloorCapVector::FloorCapVector(FloorCapVectorPrivate *d)
	:AbstractBbgVect(d)
{}

FloorCapVector& FloorCapVector::operator=(const FloorCapVector& other)
{
    Q_D(FloorCapVector);
    AbstractBbgVect::operator=(other);
    d->m_FloorVal.clear();
    d->m_CapVal.clear();
    for (auto SingleFloor = other.d_func()->m_FloorVal.constBegin(); SingleFloor != other.d_func()->m_FloorVal.constEnd(); ++SingleFloor)
    {
        if (*SingleFloor)
            d->m_FloorVal.append(std::make_shared<double>(**SingleFloor));
        else 
            d->m_FloorVal.append(std::shared_ptr<double>(nullptr));
    }
    for (auto SingleCap = other.d_func()->m_CapVal.constBegin(); SingleCap != other.d_func()->m_CapVal.constEnd(); ++SingleCap)
    {
        if (*SingleCap) 
            d->m_CapVal.append(std::make_shared<double>(**SingleCap));
        else  
            d->m_CapVal.append(std::shared_ptr<double>(nullptr));
    }
    return *this;
}

FloorCapVector::FloorCapVector(const QString& Vec)
    :FloorCapVector()
{
    SetVector(Vec);
}

std::shared_ptr<double> FloorCapVector::GetFloor(const QDate& index) const
{
    Q_D(const FloorCapVector);
    return d->GetFloor(index);
}
std::shared_ptr<double> FloorCapVector::GetFloor(int index) const
{
    Q_D(const FloorCapVector);
    return d->GetFloor(index);
}
std::shared_ptr<double> FloorCapVector::GetCap(const QDate& index) const
{
    Q_D(const FloorCapVector);
    return d->GetCap(index);
}
std::shared_ptr<double> FloorCapVector::GetCap(int index) const
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

FloorCapVector::FloorCapVector(const QString& Vec, const QDate& Anchor)
    :FloorCapVector(Vec)
{
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
    QStringList StringParts = TempVec.trimmed().toUpper().split(QRegularExpression("\\s"), QString::SkipEmptyParts);
    int StepLen;
    QString TempStr;
    for (int i = 1; i < StringParts.size(); i += 2) {
        TempStr = StringParts.at(i);
        TempStr.replace(QRegularExpression("\\D"), "");
        StepLen = TempStr.toInt();
        TempStr = StringParts.at(i);
        TempStr.replace(QRegularExpression("\\d"), "");
        for (int j = 0; j < StepLen; j++) {
            QString RawVal = StringParts.at(i - 1);
            RawVal.replace("[", "");
            RawVal.replace("]", "");
            auto CapFloor = RawVal.split(',', QString::KeepEmptyParts);
            if (CapFloor.size() > 0) {
                if (CapFloor.at(0).isEmpty()) d->m_FloorVal.append(std::shared_ptr<double>(nullptr));
                else d->m_FloorVal.append(std::make_shared<double>(CapFloor.at(0).toDouble()));
            }
            else d->m_FloorVal.append(std::shared_ptr<double>(nullptr));
            if (CapFloor.size() > 1) {
                if (CapFloor.at(1).isEmpty()) d->m_CapVal.append(std::shared_ptr<double>(nullptr));
                else d->m_CapVal.append(std::make_shared<double>(CapFloor.at(1).toDouble()));
            }
            else d->m_CapVal.append(std::shared_ptr<double>(nullptr));
        }
    }
    {
        QString RawVal = StringParts.last();
        RawVal.replace("[", "");
        RawVal.replace("]", "");
        auto CapFloor = RawVal.split(',', QString::KeepEmptyParts);
        if (CapFloor.size() > 0) {
            if (CapFloor.at(0).isEmpty()) d->m_FloorVal.append(std::shared_ptr<double>(nullptr));
            else d->m_FloorVal.append(std::make_shared<double>(CapFloor.at(0).toDouble()));
        }
        else d->m_FloorVal.append(std::shared_ptr<double>(nullptr));
        if (CapFloor.size() > 1) {
            if (CapFloor.at(1).isEmpty()) d->m_CapVal.append(std::shared_ptr<double>(nullptr));
            else d->m_CapVal.append(std::make_shared<double>(CapFloor.at(1).toDouble()));
        }
        else d->m_CapVal.append(std::shared_ptr<double>(nullptr));
    }
}
bool FloorCapVector::IsValid() const
{
    Q_D(const FloorCapVector);
    if (!AbstractBbgVect::IsValid(R"**(\[(?:(?:-?\d*\.?\d+)|(?:(?:-?\d*\.?\d+)?(?:,-?\d*\.?\d+)))\])**", false)) return false;
    QRegularExpression rx("\\[(-?\\d*\\.?\\d+),(-?\\d*\\.?\\d+)\\]");
    for (auto i = rx.globalMatch(d->m_Vector); i.hasNext();) {
        const auto singleMatch = i.next();
        if (singleMatch.captured(1).toDouble() > singleMatch.captured(2).toDouble())
            return false;
    }
    return true;
}
QRegularExpressionValidator* FloorCapVector::GetValidator(QObject* parent) const
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



