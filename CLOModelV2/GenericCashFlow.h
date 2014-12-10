#ifndef AbstractCashFlow_h__
#define AbstractCashFlow_h__
#include "BackwardCompatibilityInterface.h"
#include "IntegerVector.h"
#include <QHash>
#include <QSet>
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
	virtual void SetFlow(const GenericCashFlow& a);
	virtual QDate GetDate(int index) const;
	virtual double GetFlow(int index, qint32 FlowTpe) const;
	virtual double GetFlow(const QDate& index, qint32 FlowTpe) const;
#ifdef _DEBUG
	virtual double GetPreviousFlow(int index, qint32 FlowTpe) const { return 0.0; }
	virtual double GetPreviousFlow(const QDate& index, qint32 FlowTpe) const { return 0.0; }
#endif // _DEBUG
	virtual void Clear();
	virtual void RemoveFlow(qint32 FlowTpe);
	virtual void RemoveAllFlows() { Clear(); }
	virtual void ResetFlows() { Clear(); }
	virtual QDate MaturityDate() const;
	virtual int Count() const { return m_CashFlows.size(); }
	virtual int FindDate(const QDate& a) const;
	virtual void ReplaceDate(const QDate& OriginalDate, const QDate& NewDate);
	virtual void Aggregate(CashFlowAggregation Freq);
	virtual GenericCashFlow SingleFlow(qint32 FlowTpe) const;
	virtual GenericCashFlow SingleDate(const QDate& a) const;
	virtual bool HasFlowType(qint32 FlowTpe)const;
	virtual QList<qint32> AvailableFlows(const QDate& a=QDate()) const;
	virtual bool GetAdjustHolidays() const { return m_AdjustHolidays; }
	virtual void SetAdjustHolidays(bool val);
	virtual double GetTotalFlow(const QDate& a, const QList<qint32>& Groups) const;
	virtual double GetTotalFlow(int index, const QList<qint32>& Groups) const;
	virtual double GetTotalFlow(const QDate& a) const { return GetTotalFlow(a, QList<qint32>()); }
	virtual double GetTotalFlow(int index) const { return GetTotalFlow(index, QList<qint32>()); }
	virtual bool IsEmpty() const;
	virtual QString ToString() const;
	virtual GenericCashFlow AggregateRange(const QDate& StartDate, const QDate& EndDate)const;
	virtual void SetLabel(qint32 FlowTpe, const QString& Lab);
	virtual QString GetLabel(qint32 FlowTpe) const { return m_CashFlowLabels.value(FlowTpe, QString()); }
	virtual void RemoveLabel(qint32 FlowTp) { m_CashFlowLabels.remove(FlowTp); }
	virtual void ClearLabels() { m_CashFlowLabels.clear(); }
	virtual GenericCashFlow ScaledCashFlows(double OriginalRefSize, double ResultSize, const QList<qint32>& Groups = QList<qint32>(), const QList<qint32>& ExcludeGroups = QList<qint32>()) const;
	virtual QString ToPlainText(bool UseHeaders = true)const;
	virtual QString ToXML()const;
	virtual void LoadFromXML(const QString& Source);
	virtual void SetStock(qint32 a) { m_Stocks.insert(a); }
	virtual void RemoveStock(qint32 a) { m_Stocks.remove(a); }
	virtual void ClearStocks() { m_Stocks.clear(); }
	virtual bool IsStock(qint32 a) const { return m_Stocks.contains(a); }
	virtual const QSet<qint32>& GetStocks() const { return m_Stocks; }
protected:
	static bool SamePeriod(const QDate& a, const QDate& b, CashFlowAggregation Freq);
	QMap<QDate, QHash<qint32, double>*	> m_CashFlows;
	QHash<qint32, QString> m_CashFlowLabels;
	QSet<qint32> m_Stocks;
	CashFlowAggregation m_AggregationLevel;
	bool m_AdjustHolidays;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
};
QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
#endif // AbstractCashFlow_h__

