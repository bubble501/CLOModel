#include "AnnuityVector.h"
#include "CommonFunctions.h"
#include <QRegExp>
#include <QStringList>
AnnuityVector::AnnuityVector(const QString& Vec)
	:AbstarctBbgVect(Vec)
{
	if(IsValid()) UnpackVector();
	else m_Vector="";
}
AnnuityVector::AnnuityVector(const QString& Vec,const QDate& Anchor)
	:AbstarctBbgVect(Vec)
{
	if(IsValid()) UnpackVector();
	else m_Vector="";
	m_AnchorDate=Anchor;
}
AnnuityVector::AnnuityVector(const AnnuityVector& Vec)
{
	m_Vector=Vec.m_Vector;
	m_AnchorDate=Vec.m_AnchorDate;
	m_VectVal=Vec.m_VectVal;
}
void AnnuityVector::UnpackVector(){
	m_VectVal.clear();
	QString TempVec(m_Vector.trimmed().toUpper());
	QRegExp AnchorCheck("^A\\s+(\\d{1,2})/(\\d{1,2})/(\\d{4})\\s+(.+)",Qt::CaseInsensitive);
	if(AnchorCheck.exactMatch(TempVec)){
		QStringList dateVals=AnchorCheck.capturedTexts();
		m_AnchorDate.setDate(dateVals.at(3).toInt(),dateVals.at(1).toInt(),dateVals.at(2).toInt());
		TempVec=dateVals.at(4);
	}
	QStringList StringParts=TempVec.trimmed().toUpper().split(QRegExp("\\s"),QString::SkipEmptyParts);
	int StepLen;
	QString TempStr;
	for (int i=1;i<StringParts.size();i+=2){
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\D"),"");
		StepLen=TempStr.toInt();
		for (int j=0;j<StepLen;j++){
			m_VectVal.append(StringParts.at(i-1).at(0).toAscii());
		}
	}
	m_VectVal.append(StringParts.last().at(0).toAscii());
}
bool AnnuityVector::IsValid() const{
	QRegExp Vigil("^(A\\s+\\d{1,2}/\\d{1,2}/\\d{4}\\s+){0,1}[YN](\\s+\\d+S\\s+[YN])*$",Qt::CaseInsensitive);
	return Vigil.exactMatch(m_Vector.trimmed().toUpper());
}

char AnnuityVector::GetValue(const QDate& index)const{
	QDate ValidDate(m_AnchorDate);
	if(m_AnchorDate.isNull()) ValidDate=QDate::currentDate();
	return GetValue(MonthDiff(index,ValidDate));
}
char AnnuityVector::GetValue(int index)const{
	if(m_VectVal.isEmpty() || index<0) return 0;
	return m_VectVal.at(qMin(index,m_VectVal.size()-1));
}
QDataStream& operator<<(QDataStream & stream, const AnnuityVector& flows){
	stream	<< flows.m_Vector;
	stream	<< flows.m_AnchorDate;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, AnnuityVector& flows){
	stream	>> flows.m_Vector;
	stream	>> flows.m_AnchorDate;
	flows.UnpackVector();
	return stream;
}