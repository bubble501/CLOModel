#ifndef PrincipalProceeds_h__
#define PrincipalProceeds_h__
#include <QtGlobal>
#include "BackwardCompatibilityInterface.h"
class PrincipalRecip :public BackwardInterface {
protected:
	double m_Scheduled;
	double m_Prepay;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	const double& GetPrepay() const { return m_Prepay; }
	void SetPrepay(const double& val) { if (qAbs(val) >= 0.01) m_Prepay = val; }
	void AddPrepay(const double& val) { if (qAbs(val) >= 0.01) m_Prepay += val; }
	const double& GetScheduled() const { return m_Scheduled; }
	void SetScheduled(const double& val) { if (qAbs(val) >= 0.01) m_Scheduled = val; }
	void AddScheduled(const double& val) { if (qAbs(val) >= 0.01) m_Scheduled += val; }
	PrincipalRecip() :m_Scheduled(0.0), m_Prepay(0.0) {}
	PrincipalRecip(const PrincipalRecip& a) :m_Scheduled(a.m_Scheduled), m_Prepay(a.m_Prepay) {}
	PrincipalRecip& operator=(const PrincipalRecip& a) { m_Scheduled = a.m_Scheduled; m_Prepay = a.m_Prepay; return *this; }
	PrincipalRecip operator-(double a) { PrincipalRecip Result(*this); Result -= a; return Result; }
	PrincipalRecip& operator-=(double a) {
		if (qAbs(a) < 0.01) return *this;
		if (a < 0.0) return operator+=(-a);
		double Temp = qMin(a, m_Scheduled);
		m_Scheduled -= Temp;
		m_Prepay -= a - Temp;
		return *this;
	}
	PrincipalRecip& operator+=(double a) {
		if (qAbs(a) < 0.01) return *this;
		if (a < 0.0) return operator-=(-a);
		m_Scheduled += a;
		return *this;
	}
	PrincipalRecip operator+(double a) { PrincipalRecip Result(*this); Result += a; return Result; }
	bool operator<(double a) { return m_Scheduled < a || m_Prepay < a; }
	double Total() const { return m_Scheduled + m_Prepay; }
	void Erase() { m_Scheduled = m_Prepay = 0.0; }
	friend QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows);
	friend QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows);
};
QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows);
QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows);
#endif // PrincipalProceeds_h__