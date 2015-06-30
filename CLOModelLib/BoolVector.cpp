#include "BoolVector.h"
#include "Private/BoolVector_p.h"
#include "CommonFunctions.h"
#include <QRegExp>
#include <QStringList>
DEFINE_PUBLIC_COMMONS(BoolVector)
DEFINE_PUBLIC_COMMONS_COPY(BoolVector)
BoolVectorPrivate::BoolVectorPrivate(BoolVector *q,const BoolVectorPrivate& other)
	:AbstractBbgVectPrivate(q,other)
    , m_VectVal(other.m_VectVal)
{}
BoolVectorPrivate::BoolVectorPrivate(BoolVector *q)
	:AbstractBbgVectPrivate(q)
{}
BoolVector::BoolVector(BoolVectorPrivate *d, const BoolVector& other)
    : AbstractBbgVect(d, other)
{}
BoolVector::BoolVector(BoolVectorPrivate *d)
	:AbstractBbgVect(d)
{}
BoolVector& BoolVector::operator=(const BoolVector& other)
{
    AbstractBbgVect::operator=(other);
    Q_D(BoolVector);
    d->m_VectVal = other.d_func()->m_VectVal;
    return *this;
}

BoolVector::BoolVector(const QString& Vec)
    :BoolVector()
{
    SetVector(Vec);
}
BoolVector::BoolVector(const QString& Vec,const QDate& Anchor)
    : BoolVector(Vec)
{
    SetAnchorDate(Anchor);
}
void BoolVector::UnpackVector(){
    Q_D(BoolVector);
	d->m_VectVal.clear();
    if (d->m_Vector.isEmpty()) return;
	ExtractAnchorDate();
    QString TempVec(d->m_Vector.trimmed().toUpper());
	QStringList StringParts=TempVec.trimmed().toUpper().split(QRegExp("\\s"),QString::SkipEmptyParts);
	int StepLen;
	QString TempStr;
	for (int i=1;i<StringParts.size();i+=2){
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\D"),"");
		StepLen=TempStr.toInt();
		for (int j=0;j<StepLen;j++){
            d->m_VectVal.append(StringParts.at(i - 1).at(0) == 'T' || StringParts.at(i - 1).at(0) == 'Y');
		}
	}
    d->m_VectVal.append(StringParts.last().at(0).toLatin1());
}
bool BoolVector::IsValid() const{
	return AbstractBbgVect::IsValid("[TFNY]", false);
}
QRegExpValidator* BoolVector::GetValidator(QObject* parent) const {
	return AbstractBbgVect::GetValidator("[TFNY]", false, parent);
}
bool BoolVector::GetValue(const QDate& index)const{
    Q_D(const BoolVector);
    return d->GetValueTemplate(d->m_VectVal, index, false);
}
bool BoolVector::GetValue(int index)const{
    Q_D(const BoolVector);
    return d->GetValueTemplate(d->m_VectVal, index, false);
}

int BoolVector::NumElements() const
{
    Q_D(const BoolVector);
    return d->m_VectVal.size();
}

BoolVector& BoolVector::operator=(const QString& a)
{
    AbstractBbgVect::operator=(a); return *this;
}

QDataStream& operator<<(QDataStream & stream, const BoolVector& flows)
{
	stream
		<< flows.d_func()->m_Vector
        << flows.d_func()->m_AnchorDate
        << flows.d_func()->m_VectVal
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BoolVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BoolVector::LoadOldVersion(QDataStream& stream) {
    Q_D(BoolVector);
	stream
		>> d->m_Vector
        >> d->m_AnchorDate
        >> d->m_VectVal
		;
	ResetProtocolVersion();
	return stream;
}


