#ifndef AbstractCashFlow_h__
#define AbstractCashFlow_h__
#include "BackwardCompatibilityInterface.h"
#include <QHash>
#include <QMap>
#include <QSet>
#include <QDate>
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
	virtual bool operator==(const GenericCashFlow& a) const;
	virtual void AddFlow(const QDate& Dte, double Amt, qint32 FlowTpe);
	virtual void AddFlow(const GenericCashFlow& a);
	virtual QDate GetDate(int index) const;
	virtual double GetFlow(int index, qint32 FlowTpe) const;
	virtual double GetFlow(const QDate& index, qint32 FlowTpe) const;
	virtual double GetPreviousFlow(int index, qint32 FlowTpe) const;
	virtual double GetPreviousFlow(const QDate& index, qint32 FlowTpe) const;
	virtual void Clear();
	virtual void RemoveAllFlows() { Clear(); }
	virtual void ResetFlows() { Clear(); }
	virtual int GetPaymentFrequency() const;
	virtual QDate MaturityDate() const;
	virtual int Count() const { return m_CashFlows.size(); }
	virtual int FindDate(const QDate& a) const;
	virtual void ReplaceDate(const QDate& OriginalDate, const QDate& NewDate);
	virtual void Aggregate(CashFlowAggregation Freq);

protected:
	static bool SamePeriod(const QDate& a, const QDate& b, CashFlowAggregation Freq);
	QSet<qint32> m_UsedFlowsTypes;
	QMap<QDate, QHash<qint32, double>*	> m_CashFlows;
	CashFlowAggregation m_AggregationLevel;
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
	friend QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
};
QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
#endif // AbstractCashFlow_h__

