#include "DayCountVect.h"
#include "Private/DayCountVect_p.h"
#include <QRegularExpression>
#include <QStringList>
#include "CommonFunctions.h"
DEFINE_PUBLIC_COMMONS(DayCountVector)
DEFINE_PUBLIC_COMMONS_COPY(DayCountVector)
DayCountVectorPrivate::DayCountVectorPrivate(DayCountVector *q,const DayCountVectorPrivate& other)
	:AbstractBbgVectPrivate(q,other)
    , m_VectVal(other.m_VectVal)
{}
DayCountVectorPrivate::DayCountVectorPrivate(DayCountVector *q)
	:AbstractBbgVectPrivate(q)
{}
DayCountVector::DayCountVector(DayCountVectorPrivate *d, const DayCountVector& other)
	:AbstractBbgVect(d,other)
{}
DayCountVector::DayCountVector(DayCountVectorPrivate *d)
	:AbstractBbgVect(d)
{}

DayCountVector& DayCountVector::operator=(const DayCountVector& other)
{
    AbstractBbgVect::operator=(other);
    Q_D(DayCountVector);
    d->m_VectVal = other.d_func()->m_VectVal;
    return *this;
}

DayCountVector::DayCountVector(const QString& Vec)
    :DayCountVector()
{
    SetVector(Vec);
}

DayCountVector::DayCountVector(const QString& Vec, const QDate& Anchor)
    : DayCountVector(Vec)
{
    SetAnchorDate(Anchor);
}
DayCountVector::DayCountVector(DayCountConvention a, const QDate& index)
    : DayCountVector(QString::number(static_cast<qint16>(a)), index)
{}
void DayCountVector::UnpackVector() {
    Q_D(DayCountVector);
	d->m_VectVal.clear();
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
            d->m_VectVal.append(static_cast<DayCountConvention>(StringParts.at(i - 1).toInt()));
		}
	}
    d->m_VectVal.append(static_cast<DayCountConvention>(StringParts.last().toInt()));
}
QString DayCountVectorPrivate::getValidString() const
{
    QString Result = "(";
    for (qint16 i = 0; i <= (1 << (1 + CompoundShift));) {
        if (i > 0) Result.append('|');
        Result.append('|' + QString::number(i + DayCountConvention::FiACTACT));
        Result.append('|' + QString::number(i + DayCountConvention::FiACT360));
        Result.append('|' + QString::number(i + DayCountConvention::FiACT365));
        Result.append('|' + QString::number(i + DayCountConvention::Fi30ACT));
        Result.append('|' + QString::number(i + DayCountConvention::FiSIA30360));
        Result.append('|' + QString::number(i + DayCountConvention::Fi30365));
        Result.append('|' + QString::number(i + DayCountConvention::FiNLACT));
        Result.append('|' + QString::number(i + DayCountConvention::FiNL360));
        Result.append('|' + QString::number(i + DayCountConvention::FiNL365));
        Result.append('|' + QString::number(i + DayCountConvention::FiACTACTneom));
        Result.append('|' + QString::number(i + DayCountConvention::FiACT360neom));
        Result.append('|' + QString::number(i + DayCountConvention::FiACT365neom));
        Result.append('|' + QString::number(i + DayCountConvention::Fi30ACTneom));
        Result.append('|' + QString::number(i + DayCountConvention::FiSIA30360neom));
        Result.append('|' + QString::number(i + DayCountConvention::Fi30365neom));
        Result.append('|' + QString::number(i + DayCountConvention::FiNLACTneom));
        Result.append('|' + QString::number(i + DayCountConvention::FiNL360neom));
        Result.append('|' + QString::number(i + DayCountConvention::FiNL365neom));
        Result.append('|' + QString::number(i + DayCountConvention::FiISMA30ACT));
        Result.append('|' + QString::number(i + DayCountConvention::FiISMA30360));
        Result.append('|' + QString::number(i + DayCountConvention::FiISMA30365));
        Result.append('|' + QString::number(i + DayCountConvention::FiISMA30ACTneom));
        Result.append('|' + QString::number(i + DayCountConvention::FiISMA30360neom));
        Result.append('|' + QString::number(i + DayCountConvention::FiISMA30365neom));
        Result.append('|' + QString::number(i + DayCountConvention::FiACT364));
        //Result.append('|' + QString::number(i +  DayCountConvention::FiUSMUNI30360));
        //Result.append('|' + QString::number(i +  DayCountConvention::FiBusiness252));
        Result.append('|' + QString::number(i + DayCountConvention::FiGerman30360));
        //Result.append('|' + QString::number(i +  DayCountConvention::FiBusiness252neom));
        Result.append('|' + QString::number(i + DayCountConvention::FiGerman30360neom));
        //Result.append('|' + QString::number(i +  DayCountConvention::FiUSWITACTACTneom));
        //Result.append('|' + QString::number(i +  DayCountConvention::FiUSIWIBACT360neom));
        Result.append('|' + QString::number(i + DayCountConvention::ACTACT));
        Result.append('|' + QString::number(i + DayCountConvention::ACT360));
        Result.append('|' + QString::number(i + DayCountConvention::ACT365));
        Result.append('|' + QString::number(i + DayCountConvention::N30360));
        Result.append('|' + QString::number(i + DayCountConvention::NACTACT));
        Result.append('|' + QString::number(i + DayCountConvention::NACT360));
        Result.append('|' + QString::number(i + DayCountConvention::NACT365));
        Result.append('|' + QString::number(i + DayCountConvention::ISMA30360));
        Result.append('|' + QString::number(i + DayCountConvention::ISDAACTACT));
        Result.append('|' + QString::number(i + DayCountConvention::AFBACTACT));
        Result.append('|' + QString::number(i + DayCountConvention::NISDAACTACT));
        Result.append('|' + QString::number(i + DayCountConvention::NAFBACTACT));
        if (i == 0)
            i = (1 << CompoundShift);
        else
            i <<= 1;
    }
    Result.append(')');
    return Result;
}
bool DayCountVector::IsValid() const {
    Q_D(const DayCountVector);
    return AbstractBbgVect::IsValid(d->getValidString(), false);
}
QRegularExpressionValidator* DayCountVector::GetValidator(QObject* parent) const {
    Q_D(const DayCountVector);
    return AbstractBbgVect::GetValidator(d->getValidString(), false, parent);
}
DayCountConvention DayCountVector::GetValue(const QDate& index)const {
    Q_D(const DayCountVector);
    return d->GetValueTemplate(d->m_VectVal, index, DayCountConvention::Invalid);
}
DayCountConvention DayCountVector::GetValue(int index)const {
    Q_D(const DayCountVector);
    return d->GetValueTemplate(d->m_VectVal, index, DayCountConvention::Invalid);
}

int DayCountVector::NumElements() const
{
    Q_D(const DayCountVector);
    return d->m_VectVal.size();
}

DayCountVector& DayCountVector::operator=(const QString& a)
{
    AbstractBbgVect::operator=(a); return *this;
}

QDataStream& operator<<(QDataStream & stream, const DayCountVector& flows)
{
    stream << flows.d_func()->m_Vector;
	stream << flows.d_func()->m_AnchorDate;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, DayCountVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& DayCountVector::LoadOldVersion(QDataStream& stream) {
    Q_D(DayCountVector);
	stream
		>> d->m_Vector
        >> d->m_AnchorDate
		;
	UnpackVector();
	ResetProtocolVersion();
	return stream;
}


