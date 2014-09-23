#include "DayCountVect.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
DayCountVector::DayCountVector(const QString& Vec)
	:AbstarctBbgVect(Vec) {
	if (IsValid()) UnpackVector();
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
}
DayCountVector::DayCountVector(const DayCountVector& Vec)
	:m_VectVal(Vec.m_VectVal) {
	m_Vector = Vec.m_Vector;
	m_AnchorDate = Vec.m_AnchorDate;
}
DayCountVector& DayCountVector::operator=(const DayCountVector& Vec) {
	m_Vector = Vec.m_Vector;
	m_AnchorDate = Vec.m_AnchorDate;
	m_VectVal = Vec.m_VectVal;
	return *this;
}
DayCountVector::DayCountVector(const QString& Vec, const QDate& Anchor)
	:AbstarctBbgVect(Vec) {
	if (IsValid()) { UnpackVector(); m_AnchorDate = Anchor; }
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}

}
DayCountVector::DayCountVector(DayCountConvention a, const QDate& index) {
	m_Vector = QString::number(static_cast<qint16>(a));
	m_VectVal.append(a);
	m_AnchorDate = index;
}
void DayCountVector::UnpackVector() {
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
		for (int j = 0; j < StepLen; j++) {
			m_VectVal.append(static_cast<DayCountConvention>(StringParts.at(i - 1).toInt()));
		}
	}
	m_VectVal.append(static_cast<DayCountConvention>(StringParts.last().toInt()));
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
	return AbstarctBbgVect::IsValid(Result, false);
}
DayCountConvention DayCountVector::GetValue(const QDate& index)const {
	QDate ValidDate(m_AnchorDate);
	if (m_AnchorDate.isNull()) ValidDate = QDate::currentDate();
	return GetValue(MonthDiff(index, ValidDate));
}
DayCountConvention DayCountVector::GetValue(int index)const {
	if (m_VectVal.isEmpty() || index < 0) return DayCountConvention::Invalid;
	return m_VectVal.at(qMin(index, m_VectVal.size() - 1));
}
QDataStream& operator<<(QDataStream & stream, const DayCountVector& flows) {
	stream << flows.m_Vector;
	stream << flows.m_AnchorDate;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, DayCountVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& DayCountVector::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_Vector
		>> m_AnchorDate
		;
	UnpackVector();
	ResetProtocolVersion();
	return stream;
}
