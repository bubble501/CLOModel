#ifndef AbstractCashFlow_h__
#define AbstractCashFlow_h__
#include "BackwardCompatibilityInterface.h"
#include <QDate>
#include <QList>
class GenericCashFlowPrivate;
class GenericCashFlow;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const GenericCashFlow& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, GenericCashFlow& flows);
class CLOMODELLIB_EXPORT GenericCashFlow : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(GenericCashFlow)
    DECLARE_PUBLIC_COMMONS_COPY(GenericCashFlow)
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
	virtual GenericCashFlow& operator+=(const GenericCashFlow& a);
	virtual GenericCashFlow operator+(const GenericCashFlow& a) const;
	virtual bool operator==(const GenericCashFlow& a) const;
	virtual bool operator!=(const GenericCashFlow& a) const;
	virtual void AddFlow(QDate Dte, double Amt, qint32 FlowTpe);
	virtual void AddFlow(const GenericCashFlow& a);
	virtual void SetFlow(QDate Dte, double Amt, qint32 FlowTpe);
	virtual void SetFlow(const GenericCashFlow& a);
	virtual QDate GetDate(int index) const;
	virtual double GetFlow(int index, qint32 FlowTpe) const;
	virtual double GetFlow(const QDate& index, qint32 FlowTpe) const;
	virtual void Clear();
	virtual void RemoveFlow(qint32 FlowTpe);
    virtual void RemoveFlowsAt(qint32 index);
    virtual void RemoveFlowsAt(const QDate& a);
	virtual void RemoveAllFlows();
	virtual void ResetFlows();
	virtual QDate MaturityDate() const;
	virtual int Count() const;
	virtual int FindDate(const QDate& a) const;
	virtual void ReplaceDate(const QDate& OriginalDate, const QDate& NewDate);
	virtual void Aggregate(CashFlowAggregation Freq);
	virtual GenericCashFlow SingleFlow(qint32 FlowTpe) const;
	virtual GenericCashFlow SingleDate(const QDate& a) const;
	virtual bool HasFlowType(qint32 FlowTpe)const;
	virtual QList<qint32> AvailableFlows(const QDate& a=QDate()) const;
	virtual bool GetAdjustHolidays() const;
	virtual void SetAdjustHolidays(bool val);
	virtual double GetTotalFlow(const QDate& a, const QList<qint32>& Groups) const;
	virtual double GetTotalFlow(int index, const QList<qint32>& Groups) const;
	virtual double GetTotalFlow(const QDate& a) const;
	virtual double GetTotalFlow(int index) const;
	virtual bool IsEmpty() const;
	virtual QString ToString() const;
	virtual GenericCashFlow AggregateRange(const QDate& StartDate, const QDate& EndDate)const;
	virtual void SetLabel(qint32 FlowTpe, const QString& Lab);
	virtual QString GetLabel(qint32 FlowTpe, const QString& DefaultLab = QString()) const;
	virtual void RemoveLabel(qint32 FlowTp);
	virtual void ClearLabels();
	virtual GenericCashFlow ScaledCashFlows(double OriginalRefSize, double ResultSize, const QList<qint32>& Groups = QList<qint32>(), const QList<qint32>& ExcludeGroups = QList<qint32>()) const;
	virtual QString ToPlainText(bool UseHeaders = true)const;
	virtual QString ToXML()const;
	virtual void LoadFromXML(const QString& Source);
	virtual void SetStock(qint32 a);
	virtual void RemoveStock(qint32 a);
	virtual void ClearStocks();
	virtual bool IsStock(qint32 a) const;
	virtual const QSet<qint32>& GetStocks() const;
    virtual QDate nextFlowDate(const QDate& currDt) const;
    virtual QDate prevFlowDate(const QDate& currDt) const;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
};

#endif // AbstractCashFlow_h__

