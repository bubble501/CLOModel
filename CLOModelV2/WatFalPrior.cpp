#include "WatFalPrior.h"

WatFalPrior::WatFalPrior()
	:PriorityType(WaterfallStepType::wst_Excess)
{}
WatFalPrior::WatFalPrior(const WatFalPrior& a)
	: Parameters(a.Parameters)
	,PriorityType(a.PriorityType)
{}
WatFalPrior& WatFalPrior::operator=(const WatFalPrior& a){
	Parameters=a.Parameters;
	PriorityType=a.PriorityType;
	return *this;
}
QString WatFalPrior::ReadyToCalculate() const {
	QString Result = "";
	switch (PriorityType) {
	case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
	case WatFalPrior::WaterfallStepType::wst_SeniorFees:
	case WatFalPrior::WaterfallStepType::wst_juniorFees:
		if (!HasParameter(wstParameters::SourceOfFunding)) Result += "Expenses and Fees need a Source of Funding Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_Interest:
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "Interest needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "Interest needs a Seniority Group Level Parameter\n";
		if (!HasParameter(wstParameters::CouponIndex)) Result += "Interest needs a Coupon Index Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_Principal:
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "Principal needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "Principal needs a Seniority Group Level Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_OCTest:
	case WatFalPrior::WaterfallStepType::wst_OCTestPrinc:
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "OC Test needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "OC Test needs a Seniority Group Level Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup) || HasParameter(wstParameters::RedemptionGroupLevel)) {
			if (!HasParameter(wstParameters::RedemptionGroup)) Result += "OC Test needs a Redemption Group Parameter\n";
			if (!HasParameter(wstParameters::RedemptionGroupLevel)) Result += "OC Test needs a Redemption Group Level Parameter\n";
			if (!HasParameter(wstParameters::RedemptionShare)) Result += "OC Test needs a Redemption Share Parameter\n";
		}
		break;
	case WatFalPrior::WaterfallStepType::wst_ICTest:
	case WatFalPrior::WaterfallStepType::wst_ICTestPrinc:
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "IC Test needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "IC Test needs a Seniority Group Level Parameter\n";
		if (!HasParameter(wstParameters::CouponIndex)) Result += "IC Test needs a Coupon Index Parameter\n";
		break;
	case WatFalPrior::WaterfallStepType::wst_DeferredPrinc:
	case WatFalPrior::WaterfallStepType::wst_DeferredInterest:
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "Deferred Interest needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "Deferred Interest needs a Seniority Group Level Parameter\n";
		if (!HasParameter(wstParameters::CouponIndex)) Result += "Deferred Interest needs a Coupon Index Parameter\n";	
		break;
	case WatFalPrior::WaterfallStepType::wst_Excess:
		if (HasParameter(wstParameters::RedemptionGroup) || HasParameter(wstParameters::RedemptionGroupLevel)) {
			if (!HasParameter(wstParameters::RedemptionGroup)) Result += "Excess Spread needs a Redemption Group Parameter\n";
			if (!HasParameter(wstParameters::RedemptionGroupLevel)) Result += "Excess Spread needs a Redemption Group Level Parameter\n";
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal:
		if (!HasParameter(wstParameters::SourceOfFunding)) Result += "Reinvest Principal needs a Source of Funding Parameter\n";
		if (!HasParameter(wstParameters::AdditionalCollateralShare))Result += "Reinvest Principal needs a Additional Collateral Share Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_ReserveReplenish:
		if (!HasParameter(wstParameters::ReserveIndex)) Result += "Reserve Replenish needs a Reserve Index Parameter\n";
		if (!HasParameter(wstParameters::SourceOfFunding)) Result += "Reserve Replenish needs a Source of Funding Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_Turbo:
		if (!HasParameter(wstParameters::RedemptionGroup)) Result += "Turbo needs a Redemption Group Parameter\n";
		if (!HasParameter(wstParameters::RedemptionGroupLevel)) Result += "Turbo needs a Redemption Group Level Parameter\n";
		if (!HasParameter(wstParameters::RedemptionShare)) Result += "Turbo needs a Redemption Share Parameter\n";
		break;
	case WatFalPrior::WaterfallStepType::wst_PDL:
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "PDL Cure needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "PDL Cure needs a Seniority Group Level Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup) || HasParameter(wstParameters::RedemptionGroupLevel)) {
			if (!HasParameter(wstParameters::RedemptionGroup)) Result += "PDL Cure needs a Redemption Group Parameter\n";
			if (!HasParameter(wstParameters::RedemptionGroupLevel)) Result += "PDL Cure needs a Redemption Group Level Parameter\n";
			if (!HasParameter(wstParameters::RedemptionShare)) Result += "PDL Cure needs a Redemption Share Parameter\n";
		}
		break;
	case WatFalPrior::WaterfallStepType::wst_FeesFromExcess:
		if (!HasParameter(wstParameters::RedemptionShare)) Result += "Fees from Excess Spread needs a Redemption Share Parameter\n";
		if (!HasParameter(wstParameters::SourceOfFunding)) Result += "Fees from Excess Spread needs a Source of Funding Parameter\n";
		break;
	default:
		break;
	}
	return Result;
}
QDataStream& WatFalPrior::LoadOldVersion(QDataStream& stream) {
	qint32 TempInt;
	stream
		>> Parameters;
	stream >> TempInt;
	PriorityType = static_cast<WaterfallStepType>(TempInt);
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows) {
	return flows.LoadOldVersion(stream);
}
QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows) {
	stream
		<< flows.Parameters
		<< qint32(flows.PriorityType)
		;
	return stream;
}