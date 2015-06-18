#include "DayCountVect.h"
#include "DayCountVect_p.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
DayCountVector::DayCountVector(const QString& Vec)
    :DayCountVector()
{
    SetVector(Vec);
}
DayCountVector::DayCountVector(const DayCountVector& Vec)
    : AbstractBbgVect(new DayCountVectorPrivate(this, *Vec.d_func()))
{}

DayCountVector::DayCountVector()
    : AbstractBbgVect(new DayCountVectorPrivate(this))
{}

DayCountVector& DayCountVector::operator=(const DayCountVector& Vec)
{
    Q_D(DayCountVector);
    d->operator=(*Vec.d_func());
	return *this;
}
DayCountVector::DayCountVector(const QString& Vec, const QDate& Anchor)
    :DayCountVector()
{
    SetVector(Vec);
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
            d->m_VectVal.append(static_cast<DayCountConvention>(StringParts.at(i - 1).toInt()));
		}
	}
    d->m_VectVal.append(static_cast<DayCountConvention>(StringParts.last().toInt()));
}
bool DayCountVector::IsValid() const {
	QString Result = "(";
	qint16 i = 0;
	while (i <= (1 << (1 + CompoundShift))) {
		if (i>0) Result.append('|');
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ACT360)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ACT365)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::N30360)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NACT360)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NACT365)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ISMA30360)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ISDAACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::AFBACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NISDAACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NAFBACTACT)));
		if (i == 0) i = (1 << CompoundShift);
		else i <<= 1;
	}
	Result.append(')');
	return AbstractBbgVect::IsValid(Result, false);
}
QRegExpValidator* DayCountVector::GetValidator(QObject* parent) const {
	QString Result = "(";
	qint16 i = 0;
	while (i <= (1 << (1 + CompoundShift))) {
		if (i > 0) Result.append('|');
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ACT360)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ACT365)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::N30360)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NACT360)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NACT365)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ISMA30360)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::ISDAACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::AFBACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NISDAACTACT)));
		Result.append('|' + QString::number(i + static_cast<qint16>(DayCountConvention::NAFBACTACT)));
		if (i == 0) i = (1 << CompoundShift);
		else i <<= 1;
	}
	Result.append(')');
	return AbstractBbgVect::GetValidator(Result, false, parent);
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

DayCountVectorPrivate::DayCountVectorPrivate(DayCountVector* q, const DayCountVectorPrivate& other)
    :AbstractBbgVectPrivate(q, other)
    , m_VectVal(other.m_VectVal)
{}

DayCountVectorPrivate::DayCountVectorPrivate(DayCountVector* q)
    : AbstractBbgVectPrivate(q)
{}

DayCountVectorPrivate& DayCountVectorPrivate::operator=(const DayCountVectorPrivate& other)
{
    AbstractBbgVectPrivate::operator=(other);
    m_VectVal = other.m_VectVal;
    return *this;
}
