#ifndef AbstractCashFlow_h__
#define AbstractCashFlow_h__
#include "BackwardCompatibilityInterface.h"
#include "IntegerVector.h"
#include <QHash>
#include <QMap>
#include <QDate>
#include <QList>
class GenericCashFlow : public BackwardInterface {
public:
	enum CashFlowAggregation {
		NoAggregation
		, Weekly
		, Monthly
		, Quarterly
		, SemiAnnually
		, Annually
		, TotalAggragate
	};
	virtual ~GenericCashFlow();
	GenericCashFlow();
	GenericCashFlow(const GenericCashFlow& a);
	virtual GenericCashFlow& operator=(const GenericCashFlow& a);
	virtual GenericCashFlow& operator+=(const GenericCashFlow& a) { AddFlow(a); return *this; }
	virtual GenericCashFlow operator+(const GenericCashFlow& a) const;
	virtual bool operator==(const GenericCashFlow& a) const;
	virtual bool operator!=(const GenericCashFlow& a) const { return !(this->operator==(a)); }
	virtual void AddFlow(QDate Dte, double Amt, qint32 FlowTpe);
	virtual void AddFlow(const GenericCashFlow& a);
	virtual void SetFlow(QDate Dte, double Amt, qint32 FlowTpe);
	virtual QDate GetDate(int index) const;
	virtual double GetFlow(int index, qint32 FlowTpe) const;
	virtual double GetFlow(const QDate& index, qint32 FlowTpe) const;
	virtual double GetPreviousFlow(int index, qint32 FlowTpe) const;
	virtual double GetPreviousFlow(const QDate& index, qint32 FlowTpe) const;
	virtual void Clear();
	virtual void RemoveAllFlows() { Clear(); }
	virtual void ResetFlows() { Clear(); }
	virtual QDate MaturityDate() const;
	virtual int Count() const { return m_CashFlows.size(); }
	virtual int FindDate(const QDate& a) const;
	virtual void ReplaceDate(const QDate& OriginalDate, const QDate& NewDate);
	virtual void Aggregate(CashFlowAggregation Freq);
	virtual GenericCashFlow SingleFlow(qint32 FlowTpe) const;
	virtual bool HasFlowType(qint32 FlowTpe)const;
	virtual QList<qint32> AvailableFlows(const QDate& a) const;
	virtual bool GetAdjustHolidays() const { return m_AdjustHolidays; }
	virtual void SetAdjustHolidays(bool val);
	virtual double GetTotalFlow(const QDate& a, const QList<qint32>& Groups) const;
	virtual double GetTotalFlow(int index, const QList<qint32>& Groups) const;
	virtual double GetTotalFlow(const QDate& a) const { return GetTotalFlow(a, QList<qint32>()); }
	virtual double GetTotalFlow(int index) const { return GetTotalFlow(index, QList<qint32>()); }
#ifdef _DEBUG
	virtual QString ToString() const;
#endif // _DEBUG

protected:
	static bool SamePeriod(const QDate& a, const QDate& b, CashFlowAggregation Freq);
	QMap<QDate, QHash<qint32, double>*	> m_CashFlows;
	CashFlowAggregation m_AggregationLevel;
	bool m_AdjustHolidays;
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
	friend QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
};
QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
#endif // AbstractCashFlow_h__

