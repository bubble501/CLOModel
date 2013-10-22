#ifndef Value_h__
#define Value_h__

#include <QDate>
class ChartValue{
private:
	QDate XValue;
	double YValue;
	bool IsNullValue;
public:
	ChartValue(const QDate& a, double b);
	ChartValue(const ChartValue& a);
	ChartValue(const QDate& a);
	ChartValue();
	bool IsNull() const {return IsNullValue;} 
	bool operator<(const ChartValue& a) const;
	bool operator>(const ChartValue& a) const;
	bool operator==(const ChartValue& a) const;
	bool operator<=(const ChartValue& a) const;
	bool operator>=(const ChartValue& a) const;
	ChartValue& operator=(const ChartValue& a);
	const QDate& GetXValue() const {return XValue;}
	double GetYValue() const {return YValue;}
	void SetXValue(const QDate& a){XValue=a;}
	void SetYValue(double a){YValue=a; IsNullValue=false;}
	void SetNullValue() {YValue=0; IsNullValue=true;}
};
#endif // Value_h__
