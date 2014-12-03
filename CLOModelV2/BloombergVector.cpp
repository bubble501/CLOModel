#include "BloombergVector.h"
#include <QRegExp>
#include <QStringList>
#include <QMap>
#include <QSet>
#include "CommonFunctions.h"
#include <qmath.h>
#include <boost/math/tools/toms748_solve.hpp>
BloombergVector::BloombergVector(const QString& Vec)
	:AbstractBbgVect(Vec)
	,m_Divisor(100.0)
{
	RegisterAsMetaType<BloombergVector>();
	if(IsValid()) 
		UnpackVector();
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}

}
BloombergVector::BloombergVector(const QList<QDate>& Dates, const QList<double>& Values) 
	:m_Divisor(100.0)
{
	SetVector(Dates, Values);
	RegisterAsMetaType<BloombergVector>();
}
BloombergVector::BloombergVector(const QList<double>& Values, const QDate& Anchor) 
	: m_VectVal(Values) 
	, m_Divisor(100.0)
{
	RegisterAsMetaType<BloombergVector>();
	foreach(double SingleVal, Values) {
		if (SingleVal < 0.0) {
			m_VectVal.clear();
			m_Vector = "";
			return;
		}
	}
	m_AnchorDate = Anchor;
	RepackVector(); 
}
bool BloombergVector::SetVector(const QList<double>& Values, const QDate& Anchor) {
	if (Values.isEmpty()) return false;
	foreach(double SingleVal, Values) {
		if (SingleVal < 0.0) return false;
	}
	m_VectVal = Values;
	m_AnchorDate = Anchor;
	RepackVector();
	return true;
}
bool BloombergVector::SetVector(const QList<double>& Values) {
	if (Values.isEmpty()) return false;
	foreach(double SingleVal, Values) {
		if (SingleVal < 0.0) return false;
	}
	m_VectVal = Values;
	RepackVector();
	return true;
}

bool BloombergVector::SetVector(const QList<QDate>& Dates, const QList<double>& Values) {
	if (
		Dates.size() != Values.size() //Each date has a value and vice versa
		|| Values.isEmpty() //The vectors are not empty
		|| Dates.toSet().size()!=Dates.size() //There are no duplicates in the dates
	) return false;
	QMap<QDate, double> SortedValues;
	for (int i = 0; i <Dates.size(); i++) {
		if (Dates.at(i).isNull()) return false;
		if (Values.at(i)<0.0) return false;
		SortedValues.insert(Dates.at(i), Values.at(i));
	}
	m_AnchorDate = SortedValues.firstKey();
	m_VectVal.clear();
	int NumMonths;
	for (QMap<QDate, double>::const_iterator i = SortedValues.constBegin()+1; i != SortedValues.constEnd(); i++) {
		NumMonths = MonthDiff(i.key(), (i - 1).key());
		for (int j = 0; j < NumMonths; j++)
			m_VectVal.append((i - 1).value());
	}
	m_VectVal.append(SortedValues.last());

	RepackVector();
	return true;
}

BloombergVector::BloombergVector(const BloombergVector& Vec)
	:m_VectVal(Vec.m_VectVal)
	,m_Divisor(Vec.m_Divisor)
{
	m_LoadProtocolVersion = Vec.m_LoadProtocolVersion;
	m_Vector=Vec.m_Vector;
	m_AnchorDate=Vec.m_AnchorDate;
}
BloombergVector& BloombergVector::operator=(const BloombergVector& Vec)
{
	m_Vector=Vec.m_Vector;
	m_AnchorDate=Vec.m_AnchorDate;
	m_VectVal=Vec.m_VectVal;
	m_Divisor = Vec.m_Divisor;
	m_LoadProtocolVersion = Vec.m_LoadProtocolVersion;
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
		for(QDate i=MaxAnchor;i<=MaxEnd;i=i.addMonths(1)){
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
	:AbstractBbgVect(Vec)
	, m_Divisor(100.0)
{
	RegisterAsMetaType<BloombergVector>();
	if (IsValid()) {UnpackVector(); m_AnchorDate = Anchor;}
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
	
}

BloombergVector::BloombergVector() : m_Divisor(100.0) {
	RegisterAsMetaType<BloombergVector>();
}

void BloombergVector::RepackVector(){
	if (m_VectVal.isEmpty()) {
		m_Vector = "";
		return;
	}
	QString NewVector("");
	int StepSize = 1;
	int RampSize = 0;
	double PrevVal;
	double predictedRamp;
	boost::math::tools::eps_tolerance<double> ToleranceMonitor(std::numeric_limits<double>::digits / 2);
	PrevVal=m_VectVal.at(0);
	NewVector += QString("%1").arg(PrevVal*m_Divisor);
	for(int i=1;i<m_VectVal.size();i++){
		if (ToleranceMonitor(m_VectVal.at(i) , PrevVal) && RampSize == 0) StepSize++;
		else {
			if (StepSize == 1) {
				if (RampSize == 0) {
					RampSize++;
					predictedRamp = m_VectVal.at(i) - PrevVal;
				}
				else {
					if (ToleranceMonitor(m_VectVal.at(i) - m_VectVal.at(i - 1), predictedRamp)) {
						RampSize++;
					}
					else { //The Ramp is Over
						PrevVal=m_VectVal.at(--i);
						NewVector += QString(" %1%2 %3").arg(RampSize).arg(RampSize>1 ? 'R':'S').arg(PrevVal*m_Divisor);
						RampSize = 0;
					}
				}
			}
			else {
				PrevVal=m_VectVal.at(i);
				NewVector += QString(" %1S %2").arg(StepSize).arg(PrevVal*m_Divisor);
				StepSize = 1;
			}
		}
	}
	if (RampSize>0) NewVector += QString(" %1%2 %3").arg(RampSize).arg(RampSize>1 ? 'R' : 'S').arg(m_VectVal.last()*m_Divisor);
	m_Vector=NewVector;
}

void BloombergVector::UnpackVector(){
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
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\d"),"");
		if(TempStr=="S"){
			for (int j=0;j<StepLen;j++){
				m_VectVal.append(StringParts.at(i - 1).toDouble() / m_Divisor);
			}
		}
		else{
			for (int j=0;j<StepLen;j++){
				m_VectVal.append((
					StringParts.at(i - 1).toDouble() + ((StringParts.at(i + 1).toDouble() - StringParts.at(i - 1).toDouble()) * static_cast<double>(j) / static_cast<double>(StepLen))
					) / m_Divisor);
			}
		}
	}
	m_VectVal.append(StringParts.last().toDouble() / m_Divisor);
}
bool BloombergVector::IsValid() const{
	return AbstractBbgVect::IsValid("-?\\d*\\.?\\d+",true);
}
QRegExpValidator* BloombergVector::GetValidator(QObject* parent) const {
	return AbstractBbgVect::GetValidator("-?\\d*\\.?\\d+", true, parent);
}
double BloombergVector::GetValue(const QDate& index,int Frequency)const{
	if (Frequency<1) return 0.0;
	double Result = GetValueTemplate(m_VectVal, index, 0.0);
	if (Frequency != 12)
		return qPow(1.0 + Result, static_cast<double>(Frequency) / 12.0) - 1.0;
	return Result;
}

double BloombergVector::GetValue(int index,int Frequency)const{
	if (Frequency<1) return 0.0;
	double Result = GetValueTemplate(m_VectVal, index, 0.0);
	if(Frequency!=12) 
		return qPow(1.0 + Result, static_cast<double>(Frequency) / 12.0) - 1.0;
	return Result;
}
double BloombergVector::GetSMM(const QDate& index, int Frequency)const {
	if (Frequency < 1) return 0.0;
	double Result = GetValueTemplate(m_VectVal, index, 0.0);
	if (Frequency != 12)
		return 1.0 - qPow(1.0 - Result, static_cast<double>(Frequency) / 12.0);
	return Result;
}
double BloombergVector::GetSMM(int index, int Frequency)const {
	if (Frequency < 1) return 0.0;
	double Result = GetValueTemplate(m_VectVal, index, 0.0);
	if (Frequency != 12) 
		return 1.0 - qPow(1.0 - Result, static_cast<double>(Frequency) / 12.0);
	return Result;
}
QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows){
	stream << flows.m_Vector
		<< flows.m_AnchorDate
		<< flows.m_VectVal
		<< flows.m_Divisor
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BloombergVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BloombergVector::LoadOldVersion(QDataStream& stream) {
	stream 
		>> m_Vector
		>> m_AnchorDate
		>> m_VectVal
		>> m_Divisor
		;
	ResetProtocolVersion();
	return stream;
}
BloombergVector BloombergVector::Combine(const BloombergVector& StartVect, const BloombergVector& SwitchVect,quint32 Periods) {
	BloombergVector Result;
	if (Periods == 0) return SwitchVect;
	QDate CurrDate;
	Result.SetAnchorDate(StartVect.GetAnchorDate());
	if (StartVect.m_Divisor == SwitchVect.m_Divisor) Result.m_Divisor = StartVect.m_Divisor;
	if (!StartVect.GetAnchorDate().isNull() && !SwitchVect.GetAnchorDate().isNull()) {
		for (int i = 0; i < Periods; i++)
			Result.m_VectVal.append(StartVect.GetValue(i));
		for (CurrDate = StartVect.GetAnchorDate().addMonths(Periods); CurrDate < SwitchVect.GetAnchorDate(); CurrDate = CurrDate.addMonths(1))
			Result.m_VectVal.append(SwitchVect.GetValue(0));
		for (; CurrDate <= SwitchVect.GetAnchorDate().addMonths(SwitchVect.NumElements()); CurrDate = CurrDate.addMonths(1))
			Result.m_VectVal.append(SwitchVect.GetValue(CurrDate));
	}
	else {
		if (!SwitchVect.GetAnchorDate().isNull())
			Result.SetAnchorDate(SwitchVect.GetAnchorDate());
		for (int i = 0; i < Periods; i++) 
			Result.m_VectVal.append(StartVect.GetValue(i));
		if (Periods >= SwitchVect.NumElements())
			Result.m_VectVal.append(SwitchVect.GetValue(Periods));
		else {
			for (int i = Periods; i < SwitchVect.NumElements(); i++) 
				Result.m_VectVal.append(SwitchVect.GetValue(i));
		}
		
	}
	Result.RepackVector();
	Result.UnpackVector();
	return Result;
}

bool BloombergVector::IsEmpty(double Lbound, double Ubound) const{
	if (AbstractBbgVect::IsEmpty()) return true;
	if (Ubound < Lbound) {
		double TempV = Ubound;
		Ubound = Lbound;
		Lbound = TempV;
	}
	for (QList<double>::const_iterator i = m_VectVal.begin(); i != m_VectVal.end(); i++) {
		if ((*i)>Ubound || (*i) < Lbound) return true;
	}
	return false;
}

QString BloombergVector::BloombergSafeVector(QDate CurrentDate) const {
	if (m_AnchorDate.isNull()) return m_Vector;
	if (m_AnchorDate > CurrentDate) return "";
	BloombergVector Shorter(*this);
	Shorter.m_VectVal.erase(Shorter.m_VectVal.begin(), Shorter.m_VectVal.begin() + MonthDiff(CurrentDate, m_AnchorDate));
	Shorter.RepackVector();
	return Shorter.m_Vector;
}

BloombergVector BloombergVector::operator*(double a) const {
	if (a == 0.0) return BloombergVector("0"); 
	BloombergVector Result(*this);
	Result.SetDivisor(m_Divisor / a); 
	return Result;
}

BloombergVector BloombergVector::operator/(double a) const {
	if (a == 0.0) return BloombergVector(); 
	BloombergVector Result(*this);
	Result.SetDivisor(1.0 / (a * m_Divisor)); 
	return Result;
}

BloombergVector& BloombergVector::operator*=(double a) {
	if (a == 0.0) {
		QDate CurrentAnchor = GetAnchorDate();
		SetVector("0");
		SetAnchorDate(CurrentAnchor);
	}
	else SetDivisor(m_Divisor / a);
	return *this;
}

BloombergVector& BloombergVector::operator/=(double a) {
	if (a == 0.0) {
		operator=(BloombergVector());
	}
	else SetDivisor(1.0 / (a * m_Divisor));
	return *this;
}
