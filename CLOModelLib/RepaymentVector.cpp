#include "RepaymentVector.h"
#include "RepaymentVector_p.h"
#include "CommonFunctions.h"
#include <QRegExp>
#include <QStringList>
RepaymentVector::RepaymentVector(const QString& Vec)
    :RepaymentVector()
{
    SetVector(Vec);
}
RepaymentVector::RepaymentVector(const QString& Vec, const QDate& Anchor)
    : RepaymentVector(Vec)
{
    SetAnchorDate(Anchor);
}
RepaymentVector::RepaymentVector()
    : AbstractBbgVect(new RepaymentVectorPrivate(this))
{}
RepaymentVector::RepaymentVector(const RepaymentVector& Vec) 
    :AbstractBbgVect(new RepaymentVectorPrivate(this,*Vec.d_func()))
{}
void RepaymentVector::UnpackVector() {
    Q_D(RepaymentVector);
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
		for (int j = 0; j < StepLen; j++) {
            d->m_VectVal.append(d->ConvertRepayment(StringParts.at(i - 1).at(0)));
		}
	}
    d->m_VectVal.append(d->ConvertRepayment(StringParts.last().at(0)));
}
bool RepaymentVector::IsValid() const {
    Q_D(const RepaymentVector);
	QString PossibleRatesPattern = "[YNP";
	QString TempPart;
	for (int i = 1; true; i<<=1) {
        TempPart = d->RepaymentMethodsToString(i);
		if (TempPart.isEmpty()) break;
		PossibleRatesPattern += TempPart;
	}
	PossibleRatesPattern += ']';
	return AbstractBbgVect::IsValid(PossibleRatesPattern, false);
}
QRegExpValidator* RepaymentVector::GetValidator(QObject* parent) const {
    Q_D(const RepaymentVector);
	QString PossibleRatesPattern = "[YNP";
	QString TempPart;
	for (int i = 1; true; i <<= 1) {
        TempPart = d->RepaymentMethodsToString(i);
		if (TempPart.isEmpty()) break;
		PossibleRatesPattern += TempPart;
	}
	PossibleRatesPattern += ']';
	return AbstractBbgVect::GetValidator(PossibleRatesPattern, false, parent);
}
RepaymentVector::RepaymentMethods RepaymentVector::GetValue(const QDate& index)const {
    Q_D(const RepaymentVector);
    return d->GetValueTemplate(d->m_VectVal, index, RepaymentMethods::Invalid);
}
RepaymentVector::RepaymentMethods RepaymentVector::GetValue(int index)const {
    Q_D(const RepaymentVector);
    return d->GetValueTemplate(d->m_VectVal, index, RepaymentMethods::Invalid);
}

QString RepaymentVector::GetValueString(const QDate& index) const
{
    Q_D(const RepaymentVector);
    return d->RepaymentMethodsToString(GetValue(index));
}

QString RepaymentVector::GetValueString(int index) const
{
    Q_D(const RepaymentVector);
    return d->RepaymentMethodsToString(GetValue(index));
}

int RepaymentVector::NumElements() const
{
    Q_D(const RepaymentVector);
    return d->m_VectVal.size();
}

RepaymentVector& RepaymentVector::operator=(const RepaymentVector& a)
{
    Q_D(RepaymentVector);
    d->operator=(*a.d_func());
    return *this;
}

RepaymentVector& RepaymentVector::operator=(const QString& a)
{
    AbstractBbgVect::operator=(a); 
    return *this;
}

QDataStream& operator<<(QDataStream & stream, const RepaymentVector& flows)
{
	stream << flows.d_func()->m_Vector;
    stream << flows.d_func()->m_AnchorDate;
	return stream;
}
QDataStream& RepaymentVector::LoadOldVersion(QDataStream& stream) {
    Q_D(RepaymentVector);
	stream
		>> d->m_Vector
        >> d->m_AnchorDate
		;
	UnpackVector();
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator>>(QDataStream & stream, RepaymentVector& flows) { return flows.LoadOldVersion(stream); }
RepaymentVector::RepaymentMethods RepaymentVectorPrivate::ConvertRepayment(QChar a) {
	switch (a.toUpper().toLatin1()) {
		case 'Y': //For legacy vectors accepting Y and N
		case 'A':
            return RepaymentVector::RepaymentMethods::Annuity;
		case 'I': 
		case 'N': //For legacy vectors accepting Y and N
            return RepaymentVector::RepaymentMethods::InterestOnly;
        case 'L': return RepaymentVector::RepaymentMethods::Linear;
		case 'C': 
		case 'P': //Stands for PIK
            return RepaymentVector::RepaymentMethods::Capitalization;
        default: return RepaymentVector::RepaymentMethods::Invalid;
	}
}
QString RepaymentVectorPrivate::RepaymentMethodsToString(int a) const
{
	/*
	To amend paste the enum value and then find and replace.
	Find pattern: :b*{.+},
	Replace Pattern: case \1: return \"\1\";
	*/
	switch (a) {
    case RepaymentVector::InterestOnly: return "I";
    case RepaymentVector::Annuity: return "A";
    case RepaymentVector::Capitalization: return "C";
    case RepaymentVector::Linear: return "L";
	default: return "";
	}
}
RepaymentVector::RepaymentMethods RepaymentVectorPrivate::StringToRepaymentMethods(const QString& a) const
{
	return ConvertRepayment(a.at(0));
}

QString RepaymentVectorPrivate::RepaymentMethodsToString(RepaymentVector::RepaymentMethods a) const { 
    return RepaymentMethodsToString(static_cast<int>(a)); 
}
RepaymentVectorPrivate::RepaymentVectorPrivate(RepaymentVector* q, const RepaymentVectorPrivate& other)
    :AbstractBbgVectPrivate(q, other)
    , m_VectVal(other.m_VectVal)
{}

RepaymentVectorPrivate::RepaymentVectorPrivate(RepaymentVector* q)
    : AbstractBbgVectPrivate(q)
{}

RepaymentVectorPrivate& RepaymentVectorPrivate::operator=(const RepaymentVectorPrivate& other)
{
    AbstractBbgVectPrivate::operator=(other);
    m_VectVal = other.m_VectVal;
    return *this;
}