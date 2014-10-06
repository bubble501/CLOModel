#include "IntegerVector.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
#include <qmath.h>
IntegerVector::IntegerVector(const QString& Vec)
:AbstractBbgVect(Vec)
, m_Shift(0)
{
	RegisterAsMetaType<IntegerVector>();
	if (IsValid()) UnpackVector();
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
}
IntegerVector::IntegerVector(const IntegerVector& Vec)
:m_VectVal(Vec.m_VectVal)
, m_Shift(Vec.m_Shift)
{
	m_Vector = Vec.m_Vector;
	m_AnchorDate = Vec.m_AnchorDate;
}
IntegerVector& IntegerVector::operator=(const IntegerVector& Vec) {
	m_Vector = Vec.m_Vector;
	m_AnchorDate = Vec.m_AnchorDate;
	m_VectVal = Vec.m_VectVal;
	m_Shift = Vec.m_Shift;
	return *this;
}
IntegerVector::IntegerVector(const QString& Vec, const QDate& Anchor)
:AbstractBbgVect(Vec)
, m_Shift(0)
{
	RegisterAsMetaType<IntegerVector>();
	if (IsValid()){ UnpackVector(); m_AnchorDate = Anchor;}
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
	
}
void IntegerVector::UnpackVector() {
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
		TempStr = StringParts.at(i);
		TempStr.replace(QRegExp("\\d"), "");
		if (TempStr == "S") {
			for (int j = 0; j < StepLen; j++) {
				m_VectVal.append(StringParts.at(i - 1).toInt());
			}
		}
		else {
			for (int j = 0; j < StepLen; j++) {
				m_VectVal.append((qRound(
					StringParts.at(i - 1).toDouble() + ((StringParts.at(i + 1).toDouble() - StringParts.at(i - 1).toDouble()) * static_cast<double>(j) / static_cast<double>(StepLen))
					)));
			}
		}
	}
	m_VectVal.append(StringParts.last().toInt());
}
bool IntegerVector::IsValid() const {
	return AbstractBbgVect::IsValid("-?\\d+", true);
}
int IntegerVector::GetValue(const QDate& index)const {
	QDate ValidDate(m_AnchorDate);
	if (m_AnchorDate.isNull()) { 
		ValidDate = QDate::currentDate(); 
		LOGDEBUG("Anchor defaulted to today\n"); 
	}
	if (index < m_AnchorDate) {
		LOGDEBUG("Requested date before Anchor\n"); 
		return m_VectVal.first()+m_Shift;
	}
	return GetValue(MonthDiff(index, ValidDate));
}
int IntegerVector::GetValue(int index)const {
	if (m_VectVal.isEmpty() || index < 0) return 0;
	return m_VectVal.at(qMin(index, m_VectVal.size() - 1)) + m_Shift;
}
QDataStream& operator<<(QDataStream & stream, const IntegerVector& flows) {
	stream << flows.m_Vector;
	stream << flows.m_AnchorDate;
	stream << flows.m_VectVal;
	stream << flows.m_Shift;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, IntegerVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& IntegerVector::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_Vector
		>> m_AnchorDate
		>> m_VectVal
		>> m_Shift
		;
	ResetProtocolVersion();
	return stream;
}
bool IntegerVector::IsEmpty(int Lbound, int Ubound, bool IgnoreShift) const {
	if (AbstractBbgVect::IsEmpty()) return true;
	if (Ubound < Lbound) {
		int TempV = Ubound;
		Ubound = Lbound;
		Lbound = TempV;
	}
	for (QList<int>::const_iterator i = m_VectVal.begin(); i != m_VectVal.end(); i++) {
		if (
			(*i) + (IgnoreShift? 0:m_Shift)> Ubound 
			|| (*i) + (IgnoreShift ? 0 : m_Shift) < Lbound
		) return true;
	}
	return false;
}