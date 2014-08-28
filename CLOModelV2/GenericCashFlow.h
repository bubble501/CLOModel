#ifndef AbstractCashFlow_h__
#define AbstractCashFlow_h__
#include "BackwardCompatibilityInterface.h"
#include "IntegerVector.h"
#include <QHash>
#include <QMap>
#include <QDate>
#include <QList>
#include <QSet>
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
	virtual void AddFlow(const QDate& Dte, double Amt, qint32 FlowTpe);
	virtual void AddFlow(const GenericCashFlow& a);
	virtual void AddStack(const QDate& Dte, double Amt, qint32 FlowTpe);
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
	virtual void SetStock(qint32 FlowTpe, bool IsStock = true);
	virtual bool IsStock(qint32 FlowTpe) const {  return m_Stocks.contains(FlowTpe); }
	virtual GenericCashFlow SingleFlow(qint32 FlowTpe) const;
	virtual bool HasFlowType(qint32 FlowTpe)const;
#ifdef _DEBUG
	virtual QString ToString() const;
#endif // _DEBUG

protected:
	static bool SamePeriod(const QDate& a, const QDate& b, CashFlowAggregation Freq);
	QMap<QDate, QHash<qint32, double>*	> m_CashFlows;
	QSet<qint32> m_Stocks;
	CashFlowAggregation m_AggregationLevel;
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
	friend QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
};
QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
#endif // AbstractCashFlow_h__

