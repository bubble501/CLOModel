#include "BaseRateVect.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
#include "BloombergWorker.h"
QString BaseRateVector::AvailableRatesToString(int a) const{
	/*
	To amend paste the enum value and then find and replace.
	Find pattern: :b*{.+},
	Replace Pattern: case \1: return \"\1\";
	*/
	switch(a){
		case EUR001W: return "EUR001W";
		case EUR002W: return "EUR002W";
		case EUR003W: return "EUR003W";
		case EUR001M: return "EUR001M";
		case EUR002M: return "EUR002M";
		case EUR003M: return "EUR003M";
		case EUR004M: return "EUR004M";
		case EUR005M: return "EUR005M";
		case EUR006M: return "EUR006M";
		case EUR007M: return "EUR007M";
		case EUR008M: return "EUR008M";
		case EUR009M: return "EUR009M";
		case EUR010M: return "EUR010M";
		case EUR011M: return "EUR011M";
		case EUR012M: return "EUR012M";
		case BP00ON: return "BP00ON";
		case BP0001W: return "BP0001W";
		case BP0001M: return "BP0001M";
		case BP0002M: return "BP0002M";
		case BP0003M: return "BP0003M";
		case BP0006M: return "BP0006M";
		case BP0012M: return "BP0012M";
		case US00ON: return "US00ON";
		case US0001W: return "US0001W";
		case US0001M: return "US0001M";
		case US0002M: return "US0002M";
		case US0003M: return "US0003M";
		case US0006M: return "US0006M";
		case US0012M: return "US0012M";
		case LIBOR01W: return "LIBOR01W";
		case LIBOR01M: return "LIBOR01M";
		case LIBOR02M: return "LIBOR02M";
		case LIBOR03M: return "LIBOR03M";
		case LIBOR06M: return "LIBOR06M";
		case LIBOR12M: return "LIBOR12M";
		default: return "";
	}
}
BaseRateVector::AvailableRates BaseRateVector::StringToAvailableRates(const QString& a) const{
	/*
	To amend paste the enum value and then find and replace.
	Find pattern: :b*{.+},
	Replace Pattern: else if(a==\"\1\") return \1;
	*/
	if(a=="EUR001W") return EUR001W;
	else if(a=="EUR002W") return EUR002W;
	else if(a=="EUR003W") return EUR003W;
	else if(a=="EUR001M") return EUR001M;
	else if(a=="EUR002M") return EUR002M;
	else if(a=="EUR003M") return EUR003M;
	else if(a=="EUR004M") return EUR004M;
	else if(a=="EUR005M") return EUR005M;
	else if(a=="EUR006M") return EUR006M;
	else if(a=="EUR007M") return EUR007M;
	else if(a=="EUR008M") return EUR008M;
	else if(a=="EUR009M") return EUR009M;
	else if(a=="EUR010M") return EUR010M;
	else if(a=="EUR011M") return EUR011M;
	else if(a=="EUR012M") return EUR012M;
	else if(a=="BP00ON") return BP00ON;
	else if(a=="BP0001W") return BP0001W;
	else if(a=="BP0001M") return BP0001M;
	else if(a=="BP0002M") return BP0002M;
	else if(a=="BP0003M") return BP0003M;
	else if(a=="BP0006M") return BP0006M;
	else if(a=="BP0012M") return BP0012M;
	else if(a=="US00ON") return US00ON;
	else if(a=="US0001W") return US0001W;
	else if(a=="US0001M") return US0001M;
	else if(a=="US0002M") return US0002M;
	else if(a=="US0003M") return US0003M;
	else if(a=="US0006M") return US0006M;
	else if(a=="US0012M") return US0012M;
	else if(a=="LIBOR01W") return LIBOR01W;
	else if(a=="LIBOR01M") return LIBOR01M;
	else if(a=="LIBOR02M") return LIBOR02M;
	else if(a=="LIBOR03M") return LIBOR03M;
	else if(a=="LIBOR06M") return LIBOR06M;
	else if(a=="LIBOR12M") return LIBOR12M;
	else return Invalid;
}
BaseRateVector::BaseRateVector(const QString& Vec)
	:AbstarctBbgVect(Vec)
{
	if(IsValid()) UnpackVector();
	else m_Vector="";
}
BaseRateVector::BaseRateVector(const BaseRateVector& Vec)
{
	m_Vector=Vec.m_Vector;
	m_AnchorDate=Vec.m_AnchorDate;
	m_VectVal=Vec.m_VectVal;
}
BaseRateVector::BaseRateVector(const QString& Vec,const QDate& Anchor)
	:AbstarctBbgVect(Vec)
{
	if(IsValid()) UnpackVector();
	else m_Vector="";
	m_AnchorDate=Anchor;
}
bool BaseRateVector::IsValid() const{
	QString PossibleRatesPattern="(";
	QString TempPart;
	for(int i=0;true;i++){
		TempPart=AvailableRatesToString(i);
		if(TempPart.isEmpty()) break;
		if(i>0) PossibleRatesPattern+='|';
		PossibleRatesPattern+=TempPart;
	}
	PossibleRatesPattern+=')';
	QRegExp Vigil(
		"^(A\\s+\\d{1,2}/\\d{1,2}/\\d{4}\\s+){0,1}"
		+ PossibleRatesPattern +
		"(\\s+\\d+S\\s+"+PossibleRatesPattern+"+)*$",Qt::CaseInsensitive);
	return Vigil.exactMatch(m_Vector.trimmed().toUpper()) || m_Vector.isEmpty();
}
void BaseRateVector::UnpackVector(){
	m_VectVal.clear();
	if(m_Vector.isEmpty()) return;
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
		for (int j=0;j<StepLen;j++){
			m_VectVal.append(StringToAvailableRates(StringParts.at(i-1)));
		}
	}
	m_VectVal.append(StringToAvailableRates(StringParts.last()));
}
QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows){
	stream	<< flows.m_Vector
		<< flows.m_AnchorDate;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows){
	stream	>> flows.m_Vector
		>> flows.m_AnchorDate;
	flows.UnpackVector();
	return stream;
}
BaseRateVector::AvailableRates BaseRateVector::GetValue(const QDate& index)const{
	QDate ValidDate(m_AnchorDate);
	if(m_AnchorDate.isNull()) ValidDate=QDate::currentDate();
	return GetValue(MonthDiff(index,ValidDate));
}
BaseRateVector::AvailableRates BaseRateVector::GetValue(int index)const{
	if(m_VectVal.isEmpty() || index<0) return Invalid;
	return m_VectVal.at(qMin(index,m_VectVal.size()-1));
}
BloombergVector BaseRateVector::CompileReferenceRateValue(const QHash<QString,double>& Values) const{
	if(IsEmpty())return BloombergVector();
	QString ResultingVector("");
	int StepCounter;
	double PreviousVal,TempValue;
	for(int i=0;i<m_VectVal.size();i++){
		if(!Values.contains(GetValueString(i))) return GetRefRateValueFromBloomberg();
		if(i>0){
			TempValue=Values.value(GetValueString(i));
			if(TempValue==PreviousVal) StepCounter++;
			else{
				ResultingVector+=QString(" %1S %2").arg(StepCounter).arg(100.0*TempValue);
				PreviousVal=TempValue;
				StepCounter=1;
			}
			
		}
		else{
			PreviousVal=Values.value(GetValueString(i));
			ResultingVector+=QString("%1").arg(100.0*PreviousVal);
			StepCounter=1;
		}
		
	}
	return BloombergVector(ResultingVector,m_AnchorDate);
}
BloombergVector BaseRateVector::GetRefRateValueFromBloomberg()const{
	if(IsEmpty())return BloombergVector();
	QStringList RatesToDownload;
	for(int i=0;i<m_VectVal.size();i++){
		if(!RatesToDownload.contains(GetValueString(i))) RatesToDownload.append(GetValueString(i));
	}
	BloombergWorker Bee;
	foreach(const QString& SingleRate,RatesToDownload)
		Bee.AddSecurity(SingleRate,"Index");
	Bee.AddField("PX_LAST");
	QHash<QString, QHash<QString,QString> > ReturnedValues=Bee.StartRequest();
	QString ResultingVector;
	int StepCounter;
	AvailableRates PreviousVal;
	for(int i=0;i<m_VectVal.size();i++){
		if(i>0){
			if(PreviousVal==m_VectVal.at(i)) StepCounter++;
			else{
				PreviousVal=m_VectVal.at(i);
				ResultingVector+=QString(" %1S ").arg(StepCounter) + ReturnedValues.value(GetValueString(i)).value("PX_LAST");
				StepCounter=1;
			}
		}
		else{
			PreviousVal=m_VectVal.at(i);
			ResultingVector+=ReturnedValues.value(GetValueString(i)).value("PX_LAST");
			StepCounter=1;
		}
	}
	return BloombergVector(ResultingVector,m_AnchorDate);
}