#include "ChartValue.h"
ChartValue::ChartValue(const QDate& a, double b)
	:XValue(a)
	,YValue(b)
	,IsNullValue(false)
{}
ChartValue::ChartValue(const QDate& a)
	:XValue(a)
	,YValue(0)
	,IsNullValue(true)
{}
ChartValue::ChartValue(const ChartValue& a)
	:XValue(a.XValue)
	,YValue(a.YValue)
	,IsNullValue(a.IsNullValue)
{}
ChartValue::ChartValue()
	:YValue(0)
	,IsNullValue(false)
{}
ChartValue& ChartValue::operator=(const ChartValue& a){
	XValue=a.XValue;
	YValue=a.YValue;
	IsNullValue=a.IsNullValue;
	return *this;
}
bool ChartValue::operator<(const ChartValue& a) const {return XValue<a.XValue;}
bool ChartValue::operator>(const ChartValue& a) const {return XValue>a.XValue;}
bool ChartValue::operator==(const ChartValue& a) const {return XValue==a.XValue && YValue==a.YValue;}
bool ChartValue::operator<=(const ChartValue& a) const {return XValue<=a.XValue;}
bool ChartValue::operator>=(const ChartValue& a) const {return XValue>=a.XValue;}