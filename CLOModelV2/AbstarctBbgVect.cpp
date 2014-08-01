#include "AbstarctBbgVect.h"
#include <QRegExp>
#include <QStringList>
#include <QDate>
bool AbstarctBbgVect::SetVector(const QString& Vec){
	QString OldVec(m_Vector);
	m_Vector=Vec;
	if(IsValid()){
		UnpackVector();
		return true;
	}
	m_Vector=OldVec;
	return false;
}
AbstarctBbgVect::AbstarctBbgVect(const QString& Vec)
	:m_Vector(Vec)
{}
QString AbstarctBbgVect::GetVector() const{
	if(m_AnchorDate.isNull())
		return m_Vector;
	return "A "+m_AnchorDate.toString("MM/dd/yyyy")+' ' +m_Vector;
}
bool AbstarctBbgVect::ValidAnchorDate() const {
	QRegExp AnchorCheck("^A\\s+(\\d{1,2})/(\\d{1,2})/(\\d{4})\\s+.+", Qt::CaseInsensitive);
	if (AnchorCheck.exactMatch(m_Vector.trimmed().toUpper())) {
		QStringList dateVals = AnchorCheck.capturedTexts();
		return QDate::isValid(dateVals.at(3).toInt(), dateVals.at(1).toInt(), dateVals.at(2).toInt());
	}
	return true;
}
bool AbstarctBbgVect::IsValid(const QString& ValidInputs, bool AllowRamps) const {
	QString PatternString = "^(A\\s+\\d{1,2}/\\d{1,2}/\\d{4}\\s+){0,1}" + ValidInputs + "(\\s+[1-9][0-9]*";
	if (AllowRamps) PatternString += "[RS]";
	else PatternString += 'S';
	PatternString += "\\s+" + ValidInputs + ")*$";
	QRegExp Vigil(PatternString, Qt::CaseInsensitive);
	return (Vigil.exactMatch(m_Vector.trimmed().toUpper()) && ValidAnchorDate()) || m_Vector.isEmpty();
}

bool AbstarctBbgVect::ExtractAnchorDate() {
	if (m_Vector.isEmpty()) return false;
	QString TempVec(m_Vector.trimmed().toUpper());
	QRegExp AnchorCheck("^A\\s+(\\d{1,2})/(\\d{1,2})/(\\d{4})\\s+(.+)", Qt::CaseInsensitive);
	if (AnchorCheck.exactMatch(TempVec)) {
		QStringList dateVals = AnchorCheck.capturedTexts();
		m_AnchorDate.setDate(dateVals.at(3).toInt(), dateVals.at(1).toInt(), dateVals.at(2).toInt());
		TempVec = dateVals.at(4);
		m_Vector = TempVec;
		return true;
	}
	m_AnchorDate = QDate();
	return false;
}
void AbstarctBbgVect::Clear() {
	m_AnchorDate = QDate();
	m_Vector = "";
	UnpackVector();
}
