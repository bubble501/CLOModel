#include "BloombergVector.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
#include <qmath.h>
BloombergVector::BloombergVector(const QString& Vec)
	:AbstarctBbgVect(Vec)
{
	if(IsValid()) UnpackVector();
	else m_Vector="";
}
BloombergVector::BloombergVector(const BloombergVector& Vec)
	:m_VectVal(Vec.m_VectVal)
{
	m_Vector=Vec.m_Vector;
	m_AnchorDate=Vec.m_AnchorDate;
}
BloombergVector& BloombergVector::operator=(const BloombergVector& Vec)
{
	m_Vector=Vec.m_Vector;
	m_AnchorDate=Vec.m_AnchorDate;
	m_VectVal=Vec.m_VectVal;
	return *this;
}
BloombergVector BloombergVector::operator+(const BloombergVector& Vec) const{
	BloombergVector Result;
	QList<double> ResultVector;
	if(m_AnchorDate.isNull() || Vec.m_AnchorDate.isNull() || m_AnchorDate==Vec.m_AnchorDate){
		int MaxLen=qMax(m_VectVal.size(),Vec.m_VectVal.size());
		for(int i=0;i<MaxLen;i++){
			ResultVector.append(
				m_VectVal.at(qMin(m_VectVal.size()-1,i))
				+
				Vec.m_VectVal.at(qMin(Vec.m_VectVal.size()-1,i))
			);
		}
		if(!Vec.m_AnchorDate.isNull()) Result.m_AnchorDate=Vec.m_AnchorDate;
		else Result.m_AnchorDate=m_AnchorDate;
	}
	else{
		// The part of the vector up to the most recent anchor date will be discarded
		QDate MaxAnchor=qMax(m_AnchorDate,Vec.m_AnchorDate);
		QDate MaxEnd=qMax(m_AnchorDate.addMonths(m_VectVal.size()),Vec.m_AnchorDate.addMonths(Vec.m_VectVal.size()));
		for(QDate i=MaxAnchor;i<=MaxEnd;i.addMonths(1)){
			ResultVector.append(
				GetValue(i)
				+
				Vec.GetValue(i)
				);
		}
		Result.m_AnchorDate=MaxAnchor;
	}
	Result.m_VectVal=ResultVector;
	Result.RepackVector();
	return Result;
}
BloombergVector BloombergVector::operator+(double a) const{
	BloombergVector ResultVector(*this);
	for(int i=0;i<m_VectVal.size();i++){
		ResultVector.m_VectVal[i]+=a;
	}
	ResultVector.RepackVector();
	return ResultVector;
}

BloombergVector::BloombergVector(const QString& Vec,const QDate& Anchor)
	:AbstarctBbgVect(Vec)
{
	if(IsValid()) UnpackVector();
	else m_Vector="";
	m_AnchorDate=Anchor;
}
void BloombergVector::RepackVector(){
	QString NewVector("");
	if(!m_AnchorDate.isNull()) NewVector="A "+m_AnchorDate.toString("MM/dd/yyyy") + ' ';
	for(int i=0;i<m_VectVal.size();i++){
		NewVector+=QString("%1").arg(m_VectVal.at(i)*100.0);
		if(i>0) NewVector+=" 1S ";
	}
}
void BloombergVector::UnpackVector(){
	m_VectVal.clear();
	QString TempVec(m_Vector.trimmed().toUpper());
	QRegExp AnchorCheck("^A\\s+(\\d{1,2})/(\\d{1,2})/(\\d{4})\\s+(.+)",Qt::CaseInsensitive);
	if(AnchorCheck.exactMatch(TempVec)){
		QStringList dateVals=AnchorCheck.capturedTexts();
		m_AnchorDate.setDate(dateVals.at(3).toInt(),dateVals.at(1).toInt(),dateVals.at(2).toInt());
		TempVec=dateVals.at(4);
	}
	m_Vector=TempVec;
	QStringList StringParts=TempVec.trimmed().toUpper().split(QRegExp("\\s"),QString::SkipEmptyParts);
	int StepLen;
	QString TempStr;
	for (int i=1;i<StringParts.size();i+=2){
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\D"),"");
		StepLen=TempStr.toInt();
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\d"),"");
		if(TempStr=="S"){
			for (int j=0;j<StepLen;j++){
				m_VectVal.append(StringParts.at(i-1).toDouble()/100.0);
			}
		}
		else{
			for (int j=0;j<StepLen;j++){
				m_VectVal.append((
					StringParts.at(i - 1).toDouble() + ((StringParts.at(i + 1).toDouble() - StringParts.at(i - 1).toDouble()) * static_cast<double>(j) / static_cast<double>(StepLen))
					)/100.0);
			}
		}
	}
	m_VectVal.append(StringParts.last().toDouble()/100.0);
}
bool BloombergVector::IsValid() const{
	QRegExp Vigil("^(A\\s+\\d{1,2}/\\d{1,2}/\\d{4}\\s+){0,1}\\d*[,.]?\\d+(\\s+\\d+[RS]\\s+\\d*[,.]?\\d+)*$",Qt::CaseInsensitive);
	return Vigil.exactMatch(m_Vector.trimmed().toUpper());
}
double BloombergVector::GetValue(const QDate& index,int Frequency)const{
	QDate ValidDate(m_AnchorDate);
	if(m_AnchorDate.isNull()) ValidDate=QDate::currentDate();
	return GetValue(MonthDiff(index,ValidDate),Frequency);
}
double BloombergVector::GetValue(int index,int Frequency)const{
	if(m_VectVal.isEmpty() || index<0 || Frequency<1) return 0.0;
	if(Frequency==12) return m_VectVal.at(qMin(index,m_VectVal.size()-1));
	return qPow(1.0+m_VectVal.at(qMin(index,m_VectVal.size()-1)),static_cast<double>(Frequency)/12.0)-1.0;
}
QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows){
	stream	<< flows.m_Vector
			<< flows.m_AnchorDate;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BloombergVector& flows){
	stream	>> flows.m_Vector
			>> flows.m_AnchorDate;
	flows.UnpackVector();
	return stream;
}