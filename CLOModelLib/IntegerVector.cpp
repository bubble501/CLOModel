#include "IntegerVector.h"
#include <QRegularExpression>
#include <QStringList>
#include "CommonFunctions.h"
#include <qmath.h>
#include "Private/IntegerVector_p.h"
#include "Private/InternalItems.h"
DEFINE_PUBLIC_COMMONS(IntegerVector)
DEFINE_PUBLIC_COMMONS_COPY(IntegerVector)
IntegerVector::~IntegerVector()
{

}
IntegerVectorPrivate::~IntegerVectorPrivate()
{

}
IntegerVectorPrivate::IntegerVectorPrivate(IntegerVector *q,const IntegerVectorPrivate& other)
	:AbstractBbgVectPrivate(q,other)
    , m_VectVal(other.m_VectVal)
    , m_Shift(other.m_Shift)
{}
IntegerVectorPrivate::IntegerVectorPrivate(IntegerVector *q)
	:AbstractBbgVectPrivate(q)
    , m_Shift(0)
{}
IntegerVector::IntegerVector(IntegerVectorPrivate *d, const IntegerVector& other)
	:AbstractBbgVect(d,other)
{
    RegisterAsMetaType(IntegerVector);
}
IntegerVector::IntegerVector(IntegerVectorPrivate *d)
	:AbstractBbgVect(d)
{
    RegisterAsMetaType(IntegerVector);
}

IntegerVector& IntegerVector::operator=(const IntegerVector& other)
{
    Q_D(IntegerVector);
    AbstractBbgVect::operator=(other);
    d->m_VectVal = other.d_func()->m_VectVal;
    d->m_Shift = other.d_func()->m_Shift;
    return *this;
}
IntegerVector::IntegerVector(const QString& Vec)
    :IntegerVector()
{
    SetVector(Vec);
}


IntegerVector::IntegerVector(const QString& Vec, const QDate& Anchor)
    :IntegerVector(Vec)
{
    SetAnchorDate(Anchor);	
}
void IntegerVector::UnpackVector() {
    Q_D(IntegerVector);
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
		if (TempStr == "S") {
			for (int j = 0; j < StepLen; j++) {
                d->m_VectVal.append(StringParts.at(i - 1).toInt());
			}
		}
		else {
			for (int j = 0; j < StepLen; j++) {
                d->m_VectVal.append((qRound(
					StringParts.at(i - 1).toDouble() + ((StringParts.at(i + 1).toDouble() - StringParts.at(i - 1).toDouble()) * static_cast<double>(j) / static_cast<double>(StepLen))
					)));
			}
		}
	}
    d->m_VectVal.append(StringParts.last().toInt());
}
bool IntegerVector::IsValid() const {
	return AbstractBbgVect::IsValid("-?\\d+", true);
}
QRegularExpressionValidator* IntegerVector::GetValidator(QObject* parent) const {
	return AbstractBbgVect::GetValidator("-?\\d+", true, parent);
}
int IntegerVector::GetValue(const QDate& index)const {
    Q_D(const IntegerVector);
    return d->GetValueTemplate(d->m_VectVal, index, 0) + d->m_Shift;
}
int IntegerVector::GetValue(int index)const {
    Q_D(const IntegerVector);
    return d->GetValueTemplate(d->m_VectVal, index, 0) + d->m_Shift;
}

int IntegerVector::NumElements() const
{
    Q_D(const IntegerVector);
    return d->m_VectVal.size();
}

int IntegerVector::GetShift() const
{
    Q_D(const IntegerVector);
    return d->m_Shift;
}

void IntegerVector::SetShift(int val)
{
    Q_D(IntegerVector);
    d->m_Shift = val;
}

IntegerVector& IntegerVector::operator=(const QString& a)
{
    AbstractBbgVect::operator=(a); 
    return *this;
}

QDataStream& operator<<(QDataStream & stream, const IntegerVector& flows)
{
    stream << flows.d_func()->m_Vector;
    stream << flows.d_func()->m_AnchorDate;
    stream << flows.d_func()->m_VectVal;
	stream << flows.d_func()->m_Shift;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, IntegerVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& IntegerVector::LoadOldVersion(QDataStream& stream) {
    Q_D(IntegerVector);
	stream
        >> d->m_Vector
        >> d->m_AnchorDate
        >> d->m_VectVal
        >> d->m_Shift
		;
	ResetProtocolVersion();
	return stream;
}
bool IntegerVector::IsEmpty(int Lbound, int Ubound, bool IgnoreShift) const {
    Q_D(const IntegerVector);
	if (AbstractBbgVect::IsEmpty()) return true;
	if (Ubound < Lbound) 
        std::swap(Lbound, Ubound);
    return std::any_of(
        d->m_VectVal.constBegin(),
        d->m_VectVal.constEnd(),
        [&Lbound, &Ubound, IgnoreShift, d](double val) ->bool {return (val + (IgnoreShift ? 0 : d->m_Shift) > Ubound) || (val + (IgnoreShift ? 0 : d->m_Shift) < Lbound); }
    );
/*
    for (QList<int>::const_iterator i = d->m_VectVal.begin(); i != d->m_VectVal.end(); i++) {
		if (
            (*i) + (IgnoreShift ? 0 : d->m_Shift)> Ubound
            || (*i) + (IgnoreShift ? 0 : d->m_Shift) < Lbound
		) return true;
	}
	return false;*/
}
