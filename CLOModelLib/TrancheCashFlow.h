#ifndef TrancheCashFlow_h__
#define TrancheCashFlow_h__
#include "GenericCashFlow.h"
#include "CommonFunctions.h"
#include <QList>
#include <QHash>
#include <QDate>
namespace QBbgLib { class QBbgAbstractResponse; }
class TrancheCashFlowPrivate;
class CLOMODELLIB_EXPORT TrancheCashFlow : public GenericCashFlow
{
    DECLARE_PUBLIC_COMMONS(TrancheCashFlow)
    DECLARE_PUBLIC_COMMONS_COPY(TrancheCashFlow)
public:
	enum class TrancheFlowType : qint32{
		InterestFlow = 1 << MaximumInterestsTypes,
		PrincipalFlow = 1,
		AmountOutstandingFlow = 3,
		OCFlow = 11,
		ICFlow = 12,
		OCTarget = 211,
		ICTarget = 212,
		DeferredFlow = 1 << (MaximumInterestsTypes + 1),
		AccruedFlow = 1 << (MaximumInterestsTypes + 2),
		PDLOutstanding = 201,
		PDLCured = 202
	};
	TrancheCashFlow(double ThrancheOutstanding);
	virtual double GetTotalFlow(int index)const override;
	virtual double GetTotalFlow(const QDate& a)const override;
	virtual QString GetLabel(qint32 FlowTpe, const QString& DefaultLab = QString()) const override;
	template<class T> double GetTotalInterest(const T& index)const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetInterest can be used only with int or QDate");
		double Result = 0;
        for (qint32 i = 0; i < (1 << MaximumInterestsTypes); ++i) {
            Result += GetFlow(index, static_cast<qint32>(TrancheFlowType::InterestFlow) | i);
		}
		return Result;
	}
	template<class T> double GetTotalDeferred(const T& index)const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetDeferred can be used only with int or QDate");
		double Result = 0;
		for (qint32 i = 0; i < (1 << MaximumInterestsTypes) ; ++i) {
            Result += GetFlow(index, static_cast<qint32>(TrancheFlowType::DeferredFlow) | i);
		}
		return Result;
	}
	template<class T> double GetTotalAccrued(const T& index)const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetAccrued can be used only with int or QDate");
		double Result = 0;
        for (qint32 i = 0; i < (1 << MaximumInterestsTypes); ++i) {
            Result += GetFlow(index, static_cast<qint32>(TrancheFlowType::AccruedFlow) | i);
		}
		return Result;
	}
	template<class T> double GetPrincipal(const T& a)const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetPrincipal can be used only with int or QDate"); 
        return GetFlow(a, static_cast<qint32>(TrancheFlowType::PrincipalFlow)); 
    }
	template<class T> double GetOCTest(const T& a)const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetOCTest can be used only with int or QDate"); 
        return GetFlow(a, static_cast<qint32>(TrancheFlowType::OCFlow)); 
    }
	template<class T> double GetICTest(const T& a)const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetICTest can be used only with int or QDate");
        return GetFlow(a, static_cast<qint32>(TrancheFlowType::ICFlow)); 
    }
	template<class T> double GetOCTarget(const T& a)const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetOCTarget can be used only with int or QDate");
        return GetFlow(a, static_cast<qint32>(TrancheFlowType::OCTarget)); 
    }
	template<class T> double GetICTarget(const T& a)const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetICTarget can be used only with int or QDate"); 
        return GetFlow(a, static_cast<qint32>(TrancheFlowType::ICTarget));
    }
	template<class T> double GetOCNumerator(const T& a)const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetOCNumerator can be used only with int or QDate"); 
        return GetFlow(a, static_cast<qint32>(TrancheFlowType::OCNumerator)); 
    }
	template<class T> double GetPDLOutstanding(const T& a)const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetPDLOutstanding can be used only with int or QDate");
        return GetFlow(a, static_cast<qint32>(TrancheFlowType::PDLOutstanding)); 
    }
	template<class T> double GetPDLCured(const T& a)const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetPDLCured can be used only with int or QDate"); 
        return GetFlow(a, static_cast<qint32>(TrancheFlowType::PDLCured)); 
    }
	template<class T> double GetAmountOutstanding(const T& a)const { 
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetAmountOutstanding can be used only with int or QDate"); 
		if (HasFlowType(static_cast<qint32>(TrancheFlowType::AmountOutstandingFlow)))
			return GetFlow(a, static_cast<qint32>(TrancheFlowType::AmountOutstandingFlow));
		else
			return GetInitialOutstanding();
	}
	template<class T> double GetDeferred(const T& a, qint32 CouponIdx/*=0*/)const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetDeferred can be used only with int or QDate");
		if (CouponIdx < 0 || CouponIdx >= (1 << MaximumInterestsTypes)) return 0.0;
		if (HasFlowType(static_cast<qint32>(TrancheFlowType::DeferredFlow) | CouponIdx))
			return GetFlow(a, static_cast<qint32>(TrancheFlowType::DeferredFlow) | CouponIdx);
		return GetStartingDeferredInterest(CouponIdx);
	}
	template<class T> double GetAccrued(const T& a, qint32 CouponIdx/*=0*/)const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetAccrued can be used only with int or QDate");
		if (CouponIdx < 0 || CouponIdx >= (1 << MaximumInterestsTypes)) return 0.0;
		return GetFlow(a, static_cast<qint32>(TrancheFlowType::AccruedFlow) | CouponIdx);
	}
	template<class T> double GetInterest(const T& a, qint32 CouponIdx/*=0*/)const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetInterest can be used only with int or QDate");
		if (CouponIdx < 0 || CouponIdx >= (1 << MaximumInterestsTypes)) return 0.0;
		return GetFlow(a, static_cast<qint32>(TrancheFlowType::InterestFlow) | CouponIdx);
	}
	double GetInitialOutstanding()const;
	void SetInitialOutstanding(double a);
	virtual void AddFlow(QDate Dte, double Amt, qint32 FlwTpe) override;
	virtual void AddFlow(const QDate& Dte, double Amt, TrancheFlowType FlwTpe);
	virtual void SetFlow(QDate Dte, double Amt, qint32 FlwTpe) override;
	virtual void SetFlow(const QDate& Dte, double Amt, TrancheFlowType FlwTpe);
	using GenericCashFlow::AddFlow;
	using GenericCashFlow::SetFlow;
	QDate GetLastFlowDate(bool IncludeDeferred = false) const;
	int GetLastFlowIndex(bool IncludeDeferred = false) const;
	double GetStartingDeferredInterest(qint32 CoupIdx/*=0*/) const;
	void SetStartingDeferredInterest(const double& val, qint32 CoupIdx/*=0*/);
	void ResetStartingDeferredInterest();
	virtual bool HasFlowType(qint32 FlowTpe)const override;
    virtual bool HasInterest() const;
    virtual bool HasDeferred() const;
    virtual bool HasAccrued() const;
	virtual void LoadFromXML(const QString& Source) override;
	virtual bool GetCashFlowsDatabase(const QString& TrancheID);
    virtual bool GetCashFlowsBloomberg(const QBbgLib::QBbgAbstractResponse* a);
	virtual TrancheCashFlow ScaledCashFlows(double OldSize, double NewSize)const;
	virtual TrancheCashFlow ScaledCashFlows(double NewSize) const;
protected:
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    bool HasAnyInterest(TrancheFlowType base) const;
	friend QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows);
};
QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows);
#endif // TrancheCashFlow_h__