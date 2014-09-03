#ifndef RepaymentVector_h__
#define RepaymentVector_h__
#include <QList>
#include "AbstarctBbgVect.h"
class RepaymentVector : public AbstarctBbgVect {
public:
	enum RepaymentMethods {
		Invalid=-1
		, InterestOnly = 0x1
		, Annuity = 0x2
		, Linear = 0x4
		, Capitalization = 0x8
	};
protected:
	QList<RepaymentMethods> m_VectVal;
	QString RepaymentMethodsToString(int a) const;
	QString RepaymentMethodsToString(RepaymentMethods a) const { return RepaymentMethodsToString(static_cast<int>(a)); }
	RepaymentMethods StringToRepaymentMethods(const QString& a) const;
	virtual void UnpackVector();
	virtual bool IsValid() const;
	static RepaymentMethods ConvertRepayment(QChar a);
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	RepaymentVector() {}
	RepaymentVector(const RepaymentVector& Vec);
	RepaymentVector(const QString& Vec);
	RepaymentVector(const QString& Vec, const QDate& Anchor);
	RepaymentMethods GetValue(const QDate& index) const;
	RepaymentMethods GetValue(int index) const;
	QString GetValueString(const QDate& index) const { return RepaymentMethodsToString(GetValue(index)); }
	QString GetValueString(int index) const { return RepaymentMethodsToString(GetValue(index)); }
	int NumElements() const { return m_VectVal.size(); }
	RepaymentVector& operator=(const QString& a) { AbstarctBbgVect::operator=(a); return *this; }
	friend QDataStream& operator<<(QDataStream & stream, const RepaymentVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, RepaymentVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const RepaymentVector& flows);
QDataStream& operator>>(QDataStream & stream, RepaymentVector& flows);
#endif // RepaymentVector_h__