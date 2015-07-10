#include "BloombergVector.h"
#include "Private/BloombergVector_p.h"
#include "CommonFunctions.h"
#include "FloorCapVector.h"
#include "Private/InternalItems.h"
#include <QMap>
#include <QRegExp>
#include <QSet>
#include <QStringList>
#include <boost/math/tools/toms748_solve.hpp>
#include <qmath.h>
DEFINE_PUBLIC_COMMONS(BloombergVector)
DEFINE_PUBLIC_COMMONS_COPY(BloombergVector)
BloombergVectorPrivate::BloombergVectorPrivate(BloombergVector *q,const BloombergVectorPrivate& other)
	:AbstractBbgVectPrivate(q,other)
    , m_Divisor(other.m_Divisor)
    , m_VectVal(other.m_VectVal)
{}
BloombergVectorPrivate::BloombergVectorPrivate(BloombergVector *q)
	:AbstractBbgVectPrivate(q)
    , m_Divisor(100.0)
{}
BloombergVector::BloombergVector(BloombergVectorPrivate *d, const BloombergVector& other)
    : AbstractBbgVect(d, other)
{
    RegisterAsMetaType(BloombergVector);
}
BloombergVector::BloombergVector(BloombergVectorPrivate *d)
	:AbstractBbgVect(d)
{
    RegisterAsMetaType(BloombergVector);
}
BloombergVector& BloombergVector::operator=(const BloombergVector& Vec)
{
    AbstractBbgVect::operator=(Vec);
    Q_D(BloombergVector);
    d->m_VectVal = Vec.d_func()->m_VectVal;
    d->m_Divisor = Vec.d_func()->m_Divisor;
    return *this;
}

BloombergVector::BloombergVector(const QString& Vec)
    :BloombergVector()
{
    SetVector(Vec);
}
BloombergVector::BloombergVector(const QList<QDate>& Dates, const QList<double>& Values) 
    : BloombergVector()
{
	SetVector(Dates, Values);
}
BloombergVector::BloombergVector(const QList<double>& Values, const QDate& Anchor) 
    : BloombergVector()
{
    Q_D(BloombergVector);
    d->m_VectVal = Values;
    d->m_AnchorDate = Anchor;
	RepackVector(); 
}

bool BloombergVector::SetVector(const QList<double>& Values, const QDate& Anchor)
{
    Q_D(BloombergVector);
	if (Values.isEmpty()) 
        return false;
    d->m_VectVal = Values;
    d->m_AnchorDate = Anchor;
	RepackVector();
	return true;
}
bool BloombergVector::SetVector(const QList<double>& Values) {
    Q_D(BloombergVector);
	if (Values.isEmpty()) return false;
    /*
    Negative values are allowed
    if(std::any_of(Values.constBegin(), Values.constEnd(), [](double SingleVal)->bool {return SingleVal < 0.0; }))
        return false;
    */
    d->m_VectVal = Values;
	RepackVector();
	return true;
}

bool BloombergVector::SetVector(const QList<QDate>& Dates, const QList<double>& Values) {
    Q_D(BloombergVector);
	if (
		Dates.size() != Values.size() //Each date has a value and vice versa
		|| Values.isEmpty() //The vectors are not empty
		|| Dates.toSet().size()!=Dates.size() //There are no duplicates in the dates
	) return false;
	QMap<QDate, double> SortedValues;
	for (int i = 0; i <Dates.size(); i++) {
		if (Dates.at(i).isNull())
            return false;
		if (Values.at(i)<0.0)
            return false;
		SortedValues.insert(Dates.at(i), Values.at(i));
	}
	d->m_AnchorDate = SortedValues.firstKey();
    d->m_VectVal.clear();
	int NumMonths;
	for (auto i = SortedValues.constBegin()+1; i != SortedValues.constEnd(); i++) {
		NumMonths = MonthDiff(i.key(), (i - 1).key());
		for (int j = 0; j < NumMonths; j++)
            d->m_VectVal.append((i - 1).value());
	}
    d->m_VectVal.append(SortedValues.last());

	RepackVector();
	return true;
}

BloombergVector& BloombergVector::operator=(const QString& a)
{
    AbstractBbgVect::operator=(a); 
    return *this;
}

BloombergVector BloombergVector::operator+(const BloombergVector& Vec) const
{
    Q_D(const BloombergVector);
	BloombergVector Result;
	QList<double> ResultVector;
    if (d->m_AnchorDate.isNull() || Vec.d_func()->m_AnchorDate.isNull() || d->m_AnchorDate == Vec.d_func()->m_AnchorDate) {
        int MaxLen = qMax(d->m_VectVal.size(), Vec.d_func()->m_VectVal.size());
		for(int i=0;i<MaxLen;i++){
			ResultVector.append(
				d->m_VectVal.at(qMin(d->m_VectVal.size()-1,i))
				+
                Vec.d_func()->m_VectVal.at(qMin(Vec.d_func()->m_VectVal.size() - 1, i))
			);
		}
        if (!Vec.d_func()->m_AnchorDate.isNull()) Result.d_func()->m_AnchorDate = Vec.d_func()->m_AnchorDate;
        else Result.d_func()->m_AnchorDate = d->m_AnchorDate;
	}
	else{
		// The part of the vector up to the most recent anchor date will be discarded
        QDate MaxAnchor = qMax(d->m_AnchorDate, Vec.d_func()->m_AnchorDate);
        QDate MaxEnd = qMax(d->m_AnchorDate.addMonths(d->m_VectVal.size()), Vec.d_func()->m_AnchorDate.addMonths(Vec.d_func()->m_VectVal.size()));
		for(QDate i=MaxAnchor;i<=MaxEnd;i=i.addMonths(1)){
			ResultVector.append(
				GetValue(i)
				+
				Vec.GetValue(i)
				);
		}
        Result.d_func()->m_AnchorDate = MaxAnchor;
	}
	Result.d_func()->m_VectVal=ResultVector;
	Result.RepackVector();
	return Result;
}

BloombergVector BloombergVector::operator+(double a) const{
    Q_D(const BloombergVector);
	BloombergVector ResultVector(*this);
    for (int i = 0; i<d->m_VectVal.size(); i++) {
        ResultVector.d_func()->m_VectVal[i] += a;
	}
	ResultVector.RepackVector();
	return ResultVector;
}

BloombergVector::BloombergVector(const QString& Vec,const QDate& Anchor)
	:BloombergVector()
{
    SetVector(Vec);
    SetAnchorDate(Anchor);
}

void BloombergVector::RepackVector(){
    Q_D(BloombergVector);
    if (d->m_VectVal.isEmpty()) {
        d->m_Vector = "";
		return;
	}
	QString NewVector("");
	int StepSize = 1;
	int RampSize = 0;
	double PrevVal;
	double predictedRamp;
	boost::math::tools::eps_tolerance<double> ToleranceMonitor(std::numeric_limits<double>::digits / 2);
    PrevVal = d->m_VectVal.at(0);
    NewVector += QString("%1").arg(PrevVal*d->m_Divisor);
    for (int i = 1; i<d->m_VectVal.size(); i++) {
        if (ToleranceMonitor(d->m_VectVal.at(i), PrevVal) && RampSize == 0) {
            StepSize++;
        }
		else {
			if (StepSize == 1) {
				if (RampSize == 0) {
					RampSize++;
                    predictedRamp = d->m_VectVal.at(i) - PrevVal;
				}
				else {
                    if (ToleranceMonitor(d->m_VectVal.at(i) - d->m_VectVal.at(i - 1), predictedRamp)) {
						RampSize++;
					}
					else { //The Ramp is Over
                        PrevVal = d->m_VectVal.at(--i);
                        NewVector += QString(" %1%2 %3").arg(RampSize).arg(RampSize>1 ? 'R' : 'S').arg(PrevVal*d->m_Divisor);
						RampSize = 0;
					}
				}
			}
			else {
                PrevVal = d->m_VectVal.at(i);
                NewVector += QString(" %1S %2").arg(StepSize).arg(PrevVal*d->m_Divisor);
				StepSize = 1;
			}
		}
	}
    if (RampSize>0) 
        NewVector += QString(" %1%2 %3").arg(RampSize).arg(RampSize>1 ? 'R' : 'S').arg(d->m_VectVal.last()*d->m_Divisor);
    d->m_Vector = NewVector;
}

void BloombergVector::UnpackVector(){
    Q_D(BloombergVector);
	d->m_VectVal.clear();
    if (d->m_Vector.isEmpty()) 
        return;
	ExtractAnchorDate();
    QString TempVec(d->m_Vector.trimmed().toUpper());
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
                d->m_VectVal.append(StringParts.at(i - 1).toDouble() / d->m_Divisor);
			}
		}
		else{
			for (int j=0;j<StepLen;j++){
                d->m_VectVal.append((
					StringParts.at(i - 1).toDouble() + ((StringParts.at(i + 1).toDouble() - StringParts.at(i - 1).toDouble()) * static_cast<double>(j) / static_cast<double>(StepLen))
                    ) / d->m_Divisor);
			}
		}
	}
    d->m_VectVal.append(StringParts.last().toDouble() / d->m_Divisor);
}
bool BloombergVector::IsValid() const{
	return AbstractBbgVect::IsValid("-?\\d*\\.?\\d+",true);
}
QRegExpValidator* BloombergVector::GetValidator(QObject* parent) const {
	return AbstractBbgVect::GetValidator("-?\\d*\\.?\\d+", true, parent);
}
double BloombergVector::GetValue(const QDate& index,int Frequency)const{
    Q_D(const BloombergVector);
	if (Frequency<1) 
        return 0.0;
    double Result = d->GetValueTemplate(d->m_VectVal, index, 0.0);
	if (Frequency != 12)
		return qPow(1.0 + Result, static_cast<double>(Frequency) / 12.0) - 1.0;
	return Result;
}

double BloombergVector::GetValue(int index,int Frequency)const{
    Q_D(const BloombergVector);
	if (Frequency<1)
        return 0.0;
    double Result = d->GetValueTemplate(d->m_VectVal, index, 0.0);
	if(Frequency!=12) 
		return qPow(1.0 + Result, static_cast<double>(Frequency) / 12.0) - 1.0;
	return Result;
}
double BloombergVector::GetSMM(const QDate& index, int Frequency)const {
    Q_D(const BloombergVector);
	if (Frequency < 1)
        return 0.0;
    double Result = d->GetValueTemplate(d->m_VectVal, index, 0.0);
	if (Frequency != 12)
		return 1.0 - qPow(1.0 - Result, static_cast<double>(Frequency) / 12.0);
	return Result;
}
double BloombergVector::GetSMM(int index, int Frequency)const {
    Q_D(const BloombergVector);
	if (Frequency < 1)
        return 0.0;
    double Result = d->GetValueTemplate(d->m_VectVal, index, 0.0);
	if (Frequency != 12) 
		return 1.0 - qPow(1.0 - Result, static_cast<double>(Frequency) / 12.0);
	return Result;
}
QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows){
    stream << flows.d_func()->m_Vector
        << flows.d_func()->m_AnchorDate
        << flows.d_func()->m_VectVal
        << flows.d_func()->m_Divisor
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BloombergVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BloombergVector::LoadOldVersion(QDataStream& stream) {
    Q_D(BloombergVector);
	stream 
		>> d->m_Vector
        >> d->m_AnchorDate
        >> d->m_VectVal
        >> d->m_Divisor
		;
	ResetProtocolVersion();
	return stream;
}
BloombergVector BloombergVector::Combine(const BloombergVector& StartVect, const BloombergVector& SwitchVect,quint32 Periods) {
	BloombergVector Result;
	if (Periods == 0) 
        return SwitchVect;
	QDate CurrDate;
	Result.SetAnchorDate(StartVect.GetAnchorDate());
    if (StartVect.d_func()->m_Divisor == SwitchVect.d_func()->m_Divisor) Result.d_func()->m_Divisor = StartVect.d_func()->m_Divisor;
	if (!StartVect.GetAnchorDate().isNull() && !SwitchVect.GetAnchorDate().isNull()) {
		for (decltype(Periods) i = 0; i < Periods; i++)
            Result.d_func()->m_VectVal.append(StartVect.GetValue(i));
		for (CurrDate = StartVect.GetAnchorDate().addMonths(Periods); CurrDate < SwitchVect.GetAnchorDate(); CurrDate = CurrDate.addMonths(1))
            Result.d_func()->m_VectVal.append(SwitchVect.GetValue(0));
		for (; CurrDate <= SwitchVect.GetAnchorDate().addMonths(SwitchVect.NumElements()); CurrDate = CurrDate.addMonths(1))
            Result.d_func()->m_VectVal.append(SwitchVect.GetValue(CurrDate));
	}
	else {
		if (!SwitchVect.GetAnchorDate().isNull())
			Result.SetAnchorDate(SwitchVect.GetAnchorDate());
        for (decltype(Periods) i = 0; i < Periods; i++)
            Result.d_func()->m_VectVal.append(StartVect.GetValue(i));
        if (Periods >= static_cast<decltype(Periods)>(SwitchVect.NumElements()))
            Result.d_func()->m_VectVal.append(SwitchVect.GetValue(Periods));
		else {
			for (int i = Periods; i < SwitchVect.NumElements(); i++) 
                Result.d_func()->m_VectVal.append(SwitchVect.GetValue(i));
		}
		
	}
	Result.RepackVector();
	Result.UnpackVector();
	return Result;
}

void BloombergVector::Combine(const BloombergVector& SwitchVect, quint32 Periods)
{
    operator=(Combine(*this, SwitchVect, Periods));
}

bool BloombergVector::IsEmpty(double Lbound, double Ubound) const
{
    Q_D(const BloombergVector);
	if (AbstractBbgVect::IsEmpty()) 
        return true;
	if (Ubound < Lbound)
        std::swap(Lbound, Ubound);
    return std::any_of(d->m_VectVal.constBegin(), d->m_VectVal.constEnd(), [&Lbound, &Ubound](double val)-> bool {return val<Lbound || val>Ubound; });
	/*for (QList<double>::const_iterator i = d->m_VectVal.begin(); i != d->m_VectVal.end(); i++) {
		if ((*i)>Ubound || (*i) < Lbound) return true;
	}
	return false;*/
}

QString BloombergVector::BloombergSafeVector(QDate CurrentDate) const {
    Q_D(const BloombergVector);
    if (d->m_AnchorDate.isNull())
        return d->m_Vector;
    if (d->m_AnchorDate > CurrentDate) 
        return "";
	BloombergVector Shorter(*this);
    Shorter.d_func()->m_VectVal.erase(Shorter.d_func()->m_VectVal.begin(), Shorter.d_func()->m_VectVal.begin() + MonthDiff(CurrentDate, d->m_AnchorDate));
	Shorter.RepackVector();
    return Shorter.d_func()->m_Vector;
}

BloombergVector BloombergVector::operator*(double a) const {
    Q_D(const BloombergVector);
	if (a == 0.0) return BloombergVector("0"); 
	BloombergVector Result(*this);
    Result.SetDivisor(d->m_Divisor / a);
	return Result;
}

BloombergVector BloombergVector::operator/(double a) const {
    Q_D(const BloombergVector);
	if (a == 0.0) return BloombergVector(); 
	BloombergVector Result(*this);
	Result.SetDivisor(1.0 / (a * d->m_Divisor)); 
	return Result;
}

BloombergVector& BloombergVector::operator-=(double a)
{
    return operator+=(-a);
}

BloombergVector BloombergVector::operator-(double a) const
{
    return operator+(-a);
}

BloombergVector& BloombergVector::operator+=(double a)
{
    return operator=(operator+(a));
}

BloombergVector& BloombergVector::operator+=(const BloombergVector& a)
{
    return operator=(operator+(a));
}

BloombergVector& BloombergVector::operator*=(double a)
{
    Q_D(BloombergVector);
	if (a == 0.0) {
		QDate CurrentAnchor = GetAnchorDate();
		SetVector("0");
		SetAnchorDate(CurrentAnchor);
	}
	else SetDivisor(d->m_Divisor / a);
	return *this;
}

BloombergVector& BloombergVector::operator/=(double a) {
    Q_D(BloombergVector);
	if (a == 0.0) {
		operator=(BloombergVector());
	}
	else SetDivisor(1.0 / (a * d->m_Divisor));
	return *this;
}

void BloombergVector::ApplyFloorCap(const FloorCapVector& fc) {
    Q_D(BloombergVector);
	if (fc.IsEmpty()) return;
	QList<double> NewVec;
	QDate NewAnchor;
	if (!GetAnchorDate().isNull() && !fc.GetAnchorDate().isNull()) {
		NewAnchor = GetAnchorDate();
        for (QDate i = GetAnchorDate(); i<qMax(GetAnchorDate(), fc.GetAnchorDate()); i = i.addMonths(1)) 
            NewVec.append(d->GetValueTemplate(d->m_VectVal, i, 0.0));
		for (
			QDate i = qMax(GetAnchorDate(), fc.GetAnchorDate());
			i <= qMax(GetAnchorDate().addMonths(NumElements()), fc.GetAnchorDate().addMonths(fc.NumElements()));
			i = i.addMonths(1)
		) {
            NewVec.append(d->GetValueTemplate(d->m_VectVal, i, 0.0));
            if (fc.GetFloor(i)) NewVec.last() = qMax(*fc.GetFloor(i) / d->m_Divisor, NewVec.last());
            if (fc.GetCap(i)) NewVec.last() = qMin(*fc.GetCap(i) / d->m_Divisor, NewVec.last());
		}
	}
	else {
		if (!GetAnchorDate().isNull()) NewAnchor = GetAnchorDate();
		if (!fc.GetAnchorDate().isNull())NewAnchor = fc.GetAnchorDate();
		for (int i = 0; i < qMax(fc.NumElements(), NumElements()); ++i) {
            NewVec.append(d->GetValueTemplate(d->m_VectVal, i, 0.0));
            if (fc.GetFloor(i)) NewVec.last() = qMax(*fc.GetFloor(i) / d->m_Divisor, NewVec.last());
            if (fc.GetCap(i)) NewVec.last() = qMin(*fc.GetCap(i) / d->m_Divisor, NewVec.last());
		}
	}
    if (NewVec == d->m_VectVal) 
        return;
    d->m_VectVal = NewVec;
	RepackVector();
	d->m_AnchorDate = NewAnchor;
}

int BloombergVector::NumElements() const
{
    Q_D(const BloombergVector);
    return d->m_VectVal.size();
}

void BloombergVector::SetDivisor(double a)
{
    Q_D(BloombergVector);
    if (a != 0.0) 
        d->m_Divisor = a;
    UnpackVector();
}

double BloombergVector::GetDivisor() const
{
    Q_D(const BloombergVector);
    return d->m_Divisor;
}

