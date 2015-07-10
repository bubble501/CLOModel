#ifndef MtgCashFlow_h__
#define MtgCashFlow_h__
#include <QDate>
#include <QMetaType>
#include "GenericCashFlow.h"
#include "BloombergVector.h"
class CLOMODELLIB_EXPORT MtgCashFlow : public GenericCashFlow
{
public:
#ifdef _DEBUG
	double GetCoupTimeOut(int index)const { 
        return GenericCashFlow::GetFlow(index, MtgFlowType::WACouponFlow);
    }
	double GetCoupTimeOut(const QDate& index) const { 
        return GenericCashFlow::GetFlow(index, MtgFlowType::WACouponFlow);
    }
#endif
	enum MtgFlowType : qint32 {
		InterestFlow = 1 << MaximumInterestsTypes,
		PrincipalFlow = 1,
		PrepaymentFlow = 2,
		AmountOutstandingFlow = 3,
		WACouponFlow = 4,
		AccruedInterestFlow = 5,
		LossOnInterestFlow = 6,
		PrincipalDefault = 7,
		PrincipalRecovered = 8,
		InterestRecovered = 9,
		LossFlow = 10,
		WAPrepayMult = 129,
		WALossMult = 130,
		WAPrice = 131,
		WALlevel=133,
		DelinquentOutstanding=106,
		OutstandingForOC=103,
		PrepaymentFees=102,
		WAPrepayFees=132
	};
	virtual MtgCashFlow ApplyScenario(BloombergVector CPRv, BloombergVector CDRv, BloombergVector LSv) const;
	virtual MtgCashFlow ApplyScenario(const QString& CPRv, const QString& CDRv, const QString& LSv) const;
	template<class T> double GetDelinquentShare(const T& index)const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetDelinquentShare can be used only with int or QDate");
		double CurAmtout = GetAmountOut(index);
		if (CurAmtout < 0.01) return 0.0;
		return GetDelinquent(index) / CurAmtout;
	}
	template<class T> double GetWAL(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetWAL can be used only with int or QDate");
		if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
		return GetFlow(index, MtgFlowType::WALlevel) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
	}
	template<class T> double GetInterest(const T& index) const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetInterest can be used only with int or QDate"); 
        return GenericCashFlow::GetFlow(index, MtgFlowType::InterestFlow); 
    }
	template<class T> double GetScheduled(const T& index) const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetScheduled can be used only with int or QDate");
        return GenericCashFlow::GetFlow(index, MtgFlowType::PrincipalFlow); 
    }
	template<class T> double GetPrepay(const T& index) const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetPrepay can be used only with int or QDate");
        return GenericCashFlow::GetFlow(index, MtgFlowType::PrepaymentFlow);
    }
	template<class T> double GetPrincipal(const T& index) const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetPrincipal can be used only with int or QDate"); 
        return GetScheduled(index) + GetPrepay(index); 
    }
	template<class T> double GetLoss(const T& index) const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetLoss can be used only with int or QDate");	
        return GenericCashFlow::GetFlow(index, MtgFlowType::LossFlow); 
    }
	template<class T> double GetAmountOut(const T& index) const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetAmountOut can be used only with int or QDate"); 
        return GenericCashFlow::GetFlow(index, MtgFlowType::AmountOutstandingFlow); 
    }
	template<class T> double GetAccruedInterest(const T& index) const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetAccruedInterest can be used only with int or QDate");
        return GenericCashFlow::GetFlow(index, MtgFlowType::AccruedInterestFlow);
    }
	template<class T> double GetLossOnInterest(const T& index) const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetLossOnInterest can be used only with int or QDate"); 
        return GenericCashFlow::GetFlow(index, MtgFlowType::LossOnInterestFlow); 
    }
	template<class T> double GetDelinquent(const T& index) const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetDelinquent can be used only with int or QDate"); 
        return GenericCashFlow::GetFlow(index, MtgFlowType::DelinquentOutstanding); 
    }
	template<class T> double GetPrepayFees(const T& index) const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetPrepayFees can be used only with int or QDate"); 
        return GenericCashFlow::GetFlow(index, MtgFlowType::PrepaymentFees); 
    }
	template<class T> double GetWAcoupon(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetWAcoupon can be used only with int or QDate");
		if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
		return GetFlow(index, MtgFlowType::WACouponFlow) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
	}
	template<class T> double GetWAprepayMult(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetWAprepayMult can be used only with int or QDate");
		if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
		return GetFlow(index, MtgFlowType::WAPrepayMult) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
	}
	template<class T> double GetWAlossMult(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetWAlossMult can be used only with int or QDate");
		if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
		return GetFlow(index, MtgFlowType::WALossMult) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
	}
	template<class T> double GetWAPrice(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetWAPrice can be used only with int or QDate");
		if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
		return GetFlow(index, MtgFlowType::WAPrice) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
	}
	template<class T> double GetWAPrepayFees(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetWAPrepayFees can be used only with int or QDate");
		if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
		return GetFlow(index, MtgFlowType::WAPrepayFees) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
	}
	template<class T> double GetDefaults(const T& index) const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetDefaults can be used only with int or QDate");
        return GenericCashFlow::GetFlow(index, MtgFlowType::PrincipalDefault);
    }
	template<class T> double GetRecoveries(const T& index) const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetRecoveries can be used only with int or QDate"); 
        return GenericCashFlow::GetFlow(index, MtgFlowType::PrincipalRecovered); 
    }
	template<class T> double GetInterestRecoveries(const T& index) const { 
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetInterestRecoveries can be used only with int or QDate"); 
        return GenericCashFlow::GetFlow(index, MtgFlowType::InterestRecovered); 
    }
	template<class T> double GetOutstandingForOC(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetOutstandingForOC can be used only with int or QDate");
		if (HasFlowType(MtgFlowType::OutstandingForOC))
			return GenericCashFlow::GetFlow(index, MtgFlowType::OutstandingForOC);
		return GetAmountOut(index);
	}
	
	virtual MtgCashFlow ScaledCashFlows(double OriginalRefSize, double ResultSize)const;
	virtual double GetTotalFlow(int index)const override;
	virtual double GetTotalFlow(const QDate& a)const override;
	virtual void AddFlow(const QDate& Dte, double Amt, MtgFlowType FlowTpe) { 
        GenericCashFlow::AddFlow(Dte, Amt, static_cast<qint32>(FlowTpe));
    }
	virtual void SetFlow(const QDate& Dte, double Amt, MtgFlowType FlowTpe) { 
        GenericCashFlow::SetFlow(Dte, Amt, static_cast<qint32>(FlowTpe));
    }
	template<class T>  double GetFlow(const T& index, MtgFlowType FlowTpe)const {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GetFlow can be used only with int or QDate"); 
        return GenericCashFlow::GetFlow(index, static_cast<qint32>(FlowTpe));
    }
	using GenericCashFlow::AddFlow;
	using GenericCashFlow::SetFlow;
	using GenericCashFlow::GetFlow; 
	MtgCashFlow(const MtgCashFlow& a);
	MtgCashFlow();
};
Q_DECLARE_METATYPE(MtgCashFlow)
#endif // MtgCashFlow_h__