#include "AbstractBbgVect.h"
#include <QRegExp>
#include <QStringList>
#include <QDate>
#include <QSettings>
bool AbstractBbgVect::SetVector(const QString& Vec){
	/*QString OldVec(m_Vector);
	m_Vector=Vec;
	if(IsValid()){
		UnpackVector();
		return true;
	}
	m_Vector=OldVec;
	return false;*/
	m_Vector = Vec;
	if (IsValid()) {
		UnpackVector();
		return true;
	}
	m_Vector = "";
	return false;
}
AbstractBbgVect::AbstractBbgVect(const QString& Vec)
	:m_Vector(Vec)
{}
QString AbstractBbgVect::GetVector() const{
	if(m_AnchorDate.isNull())
		return m_Vector;
	return "A "+m_AnchorDate.toString("MM/dd/yyyy")+' ' +m_Vector;
}
bool AbstractBbgVect::ValidAnchorDate() const {
	QRegExp AnchorCheck(QString("^A\\s+") + VectorAnchorDateFormat + "\\s+.+", Qt::CaseInsensitive);
	if (AnchorCheck.exactMatch(m_Vector.trimmed())) {
		QStringList dateVals = AnchorCheck.capturedTexts();
		return QDate::isValid(dateVals.at(3).toInt(), dateVals.at(1).toInt(), dateVals.at(2).toInt());
	}
	return true;
}
bool AbstractBbgVect::IsValid(const QString& ValidInputs, bool AllowRamps) const {
	QString PatternString = QString("^(?:A\\s+") + VectorAnchorDateFormat + "\\s+){0,1}" + ValidInputs + "(?:\\s+[1-9][0-9]*";
	if (AllowRamps) PatternString += "[RS]";
	else PatternString += 'S';
	PatternString += "\\s+" + ValidInputs + ")*$";
	QRegExp Vigil(PatternString, Qt::CaseInsensitive);
	return (Vigil.exactMatch(m_Vector.trimmed()) && ValidAnchorDate()) || m_Vector.isEmpty();
}
QRegExpValidator* AbstractBbgVect::GetValidator(const QString& ValidInputs, bool AllowRamps, QObject* parent) const {
	QString PatternString = QString("(^$|^(?:A\\s+") + VectorAnchorDateFormat + "\\s+){0,1}" + ValidInputs + "(?:\\s+[1-9][0-9]*";
	if (AllowRamps) PatternString += "[RS]";
	else PatternString += 'S';
	PatternString += "\\s+" + ValidInputs + ")*$)";
	QRegExp ValidReg(PatternString,Qt::CaseInsensitive);
	return new QRegExpValidator(ValidReg, parent);
}
bool AbstractBbgVect::ExtractAnchorDate() {
	QString TempVec(m_Vector.trimmed().toUpper());
	QRegExp AnchorCheck(QString("^A\\s+") + VectorAnchorDateFormat + "\\s+(.+)", Qt::CaseInsensitive);
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
void AbstractBbgVect::Clear() {
	m_AnchorDate = QDate();
	m_Vector = "";
	UnpackVector();
}




