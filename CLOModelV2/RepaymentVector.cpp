#include "RepaymentVector.h"
#include "CommonFunctions.h"
#include <QRegExp>
#include <QStringList>
RepaymentVector::RepaymentVector(const QString& Vec)
:AbstarctBbgVect(Vec) {
	if (IsValid()) UnpackVector();
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
}
RepaymentVector::RepaymentVector(const QString& Vec, const QDate& Anchor)
: AbstarctBbgVect(Vec) {
	if (IsValid()) {
		UnpackVector();
		m_AnchorDate = Anchor;
	}
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
}
RepaymentVector::RepaymentVector(const RepaymentVector& Vec) {
	m_Vector = Vec.m_Vector;
	m_AnchorDate = Vec.m_AnchorDate;
	m_VectVal = Vec.m_VectVal;
}
void RepaymentVector::UnpackVector() {
	m_VectVal.clear();
	if (m_Vector.isEmpty()) return;
	ExtractAnchorDate();
	QString TempVec(m_Vector.trimmed().toUpper());
	QStringList StringParts = TempVec.trimmed().toUpper().split(QRegExp("\\s"), QString::SkipEmptyParts);
	int StepLen;
	QString TempStr;
	for (int i = 1; i < StringParts.size(); i += 2) {
		TempStr = StringParts.at(i);
		TempStr.replace(QRegExp("\\D"), "");
		StepLen = TempStr.toInt();
		for (int j = 0; j < StepLen; j++) {
			m_VectVal.append(ConvertRepayment(StringParts.at(i - 1).at(0)));
		}
	}
	m_VectVal.append(ConvertRepayment(StringParts.last().at(0)));
}
bool RepaymentVector::IsValid() const {
	QString PossibleRatesPattern = "[YNP";
	QString TempPart;
	for (int i = 1; true; i<<=1) {
		TempPart = RepaymentMethodsToString(i);
		if (TempPart.isEmpty()) break;
		PossibleRatesPattern += TempPart;
	}
	PossibleRatesPattern += ']';
	return AbstarctBbgVect::IsValid(PossibleRatesPattern, true);
}

RepaymentVector::RepaymentMethods RepaymentVector::GetValue(const QDate& index)const {
	QDate ValidDate(m_AnchorDate);
	if (m_AnchorDate.isNull()) ValidDate = QDate::currentDate();
	return GetValue(MonthDiff(index, ValidDate));
}
RepaymentVector::RepaymentMethods RepaymentVector::GetValue(int index)const {
	if (m_VectVal.isEmpty() || index < 0) return Invalid;
	return m_VectVal.at(qMin(index, m_VectVal.size() - 1));
}
QDataStream& operator<<(QDataStream & stream, const RepaymentVector& flows) {
	stream << flows.m_Vector;
	stream << flows.m_AnchorDate;
	return stream;
}
QDataStream& RepaymentVector::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_Vector
		>> m_AnchorDate
		;
	UnpackVector();
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator>>(QDataStream & stream, RepaymentVector& flows) { return flows.LoadOldVersion(stream); }
RepaymentVector::RepaymentMethods RepaymentVector::ConvertRepayment(QChar a) {
	switch (a.toUpper().toLatin1()) {
		case 'Y': //For legacy vectors accepting Y and N
		case 'A':
			return RepaymentMethods::Annuity;
		case 'I': 
		case 'N': //For legacy vectors accepting Y and N
			return RepaymentMethods::InterestOnly;
		case 'L': return RepaymentMethods::Linear;
		case 'C': 
		case 'P': //Stands for PIK
			return RepaymentMethods::Capitalization;
		default: return RepaymentMethods::Invalid;
	}
}
QString RepaymentVector::RepaymentMethodsToString(int a) const {
	/*
	To amend paste the enum value and then find and replace.
	Find pattern: :b*{.+},
	Replace Pattern: case \1: return \"\1\";
	*/
	switch (a) {
	case InterestOnly: return "I";
	case Annuity: return "A";
	case Capitalization: return "C";
	case Linear: return "L";
	default: return "";
	}
}
RepaymentVector::RepaymentMethods RepaymentVector::StringToRepaymentMethods(const QString& a) const {
	return ConvertRepayment(a.at(0));
}