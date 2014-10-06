#include "BoolVector.h"
#include "CommonFunctions.h"
#include <QRegExp>
#include <QStringList>
BoolVector::BoolVector(const QString& Vec)
	:AbstractBbgVect(Vec)
{
	if(IsValid()) UnpackVector();
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
}
BoolVector::BoolVector(const QString& Vec,const QDate& Anchor)
	:AbstractBbgVect(Vec)
{
	if (IsValid()) {
		UnpackVector();
		m_AnchorDate = Anchor;
	}
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
}
BoolVector::BoolVector(const BoolVector& Vec)
{
	m_Vector=Vec.m_Vector;
	m_AnchorDate=Vec.m_AnchorDate;
	m_VectVal=Vec.m_VectVal;
}
void BoolVector::UnpackVector(){
	m_VectVal.clear();
	if(m_Vector.isEmpty()) return;
	ExtractAnchorDate();
	QString TempVec(m_Vector.trimmed().toUpper());
	QStringList StringParts=TempVec.trimmed().toUpper().split(QRegExp("\\s"),QString::SkipEmptyParts);
	int StepLen;
	QString TempStr;
	for (int i=1;i<StringParts.size();i+=2){
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\D"),"");
		StepLen=TempStr.toInt();
		for (int j=0;j<StepLen;j++){
			m_VectVal.append(StringParts.at(i - 1).at(0) == 'T' || StringParts.at(i - 1).at(0) == 'Y');
		}
	}
	m_VectVal.append(StringParts.last().at(0).toLatin1());
}
bool BoolVector::IsValid() const{
	return AbstractBbgVect::IsValid("[TFNY]", false);
}

bool BoolVector::GetValue(const QDate& index)const{
	QDate ValidDate(m_AnchorDate);
	if (m_AnchorDate.isNull()) { 
		ValidDate = QDate::currentDate(); 
		LOGDEBUG("Anchor defaulted to today\n"); 
	}
	if (index < m_AnchorDate) { 
		LOGDEBUG("Requested date before Anchor\n"); 
		return m_VectVal.first(); 
	}
	return GetValue(MonthDiff(index,ValidDate));
}
bool BoolVector::GetValue(int index)const{
	if(m_VectVal.isEmpty() || index<0) return 0;
	return m_VectVal.at(qMin(index,m_VectVal.size()-1));
}
QDataStream& operator<<(QDataStream & stream, const BoolVector& flows){
	stream
		<< flows.m_Vector
		<< flows.m_AnchorDate
		<< flows.m_VectVal
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BoolVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BoolVector::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_Vector
		>> m_AnchorDate
		>> m_VectVal
		;
	ResetProtocolVersion();
	return stream;
}