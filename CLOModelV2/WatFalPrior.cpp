#include "WatFalPrior.h"
#include "IntegerVector.h"
#include "BloombergVector.h"
#include <QMetaType>
WatFalPrior::WatFalPrior()
	:PriorityType(WaterfallStepType::wst_Excess)
{
}
WatFalPrior::WatFalPrior(const WatFalPrior& a)
	:PriorityType(a.PriorityType)
	, TriggerStruc(a.TriggerStruc)
{
	for (auto i = a.IntParameters.constBegin(); i != a.IntParameters.constEnd(); ++i)
		IntParameters.insert(i.key(), new IntegerVector(*(i.value())));
	for (auto i = a.DoubleParameters.constBegin(); i != a.DoubleParameters.constEnd(); ++i)
		DoubleParameters.insert(i.key(), new BloombergVector(*(i.value())));
}
WatFalPrior& WatFalPrior::operator=(const WatFalPrior& a){
	TriggerStruc = a.TriggerStruc;
	for (auto i = a.IntParameters.constBegin(); i != a.IntParameters.constEnd(); ++i)
		IntParameters.insert(i.key(), new IntegerVector(*(i.value())));
	for (auto i = a.DoubleParameters.constBegin(); i != a.DoubleParameters.constEnd(); ++i)
		DoubleParameters.insert(i.key(), new BloombergVector(*(i.value())));
	PriorityType=a.PriorityType;
	return *this;
}
QString WatFalPrior::ReadyToCalculate() const {
	QString Result = "";
	switch (PriorityType) {
	case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
	case WatFalPrior::WaterfallStepType::wst_SeniorFees:
	case WatFalPrior::WaterfallStepType::wst_juniorFees:
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1,2)) Result += "Expenses and Fees Source of Funding Parameter\n";
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
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "OC Test needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "OC Test needs a Seniority Group Level Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup)) {
			if (!HasParameter(wstParameters::RedemptionGroupLevel)) Result += "OC Test needs a Redemption Group Level Parameter\n";
			if (!HasParameter(wstParameters::RedemptionShare)) Result += "OC Test needs a Redemption Share Parameter\n";
		}
		break;
	case WatFalPrior::WaterfallStepType::wst_ICTest:
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "IC Test needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "IC Test needs a Seniority Group Level Parameter\n";
		if (!HasParameter(wstParameters::CouponIndex)) Result += "IC Test needs a Coupon Index Parameter\n";
		break;
	case WatFalPrior::WaterfallStepType::wst_DeferredInterest:
		if (!HasParameter(wstParameters::SeniorityGroup)) Result += "Deferred Interest needs a Seniority Group Parameter\n";
		if (!HasParameter(wstParameters::SeniorityGroupLevel)) Result += "Deferred Interest needs a Seniority Group Level Parameter\n";
		if (!HasParameter(wstParameters::CouponIndex)) Result += "Deferred Interest needs a Coupon Index Parameter\n";	
		break;
	case WatFalPrior::WaterfallStepType::wst_Excess:
		if (!HasParameter(wstParameters::SourceOfFunding)) Result += "Excess Spread needs a Source of Funding Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup)) {
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
		if (HasParameter(wstParameters::RedemptionGroup)) {
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
	qint32 TempInt,TempSize;
	IntegerVector TempIV;
	BloombergVector TempBV;
	stream >> TempSize;
	for (int i = 0; i < TempSize; i++) {
		stream >> TempInt;
		TempIV.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> TempIV;
		IntParameters.insert(TempInt, new IntegerVector(TempIV));
	}
	stream >> TempSize;
	for (int i = 0; i < TempSize; i++) {
		stream >> TempInt;
		TempBV.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> TempBV;
		DoubleParameters.insert(TempInt, new BloombergVector(TempBV));
	}
	stream >> TempInt >> TriggerStruc;
	PriorityType = static_cast<WaterfallStepType>(TempInt);
	ResetProtocolVersion();
	return stream;
}

void WatFalPrior::ClearParameters() {
	for (auto i = IntParameters.begin(); i != IntParameters.end(); ++i)
		delete i.value();
	for (auto i = DoubleParameters.begin(); i != DoubleParameters.end(); ++i)
		delete i.value();
	IntParameters.clear();
	DoubleParameters.clear();
	TriggerStruc = QString();
}

WatFalPrior::~WatFalPrior() {
	ClearParameters();
}

QVariant WatFalPrior::GetParameter(qint32 ParameterType) const {
	QVariant Result;
	if (IntParameters.contains(ParameterType)) Result.setValue<IntegerVector>(*(IntParameters.value(ParameterType)));
	else if (DoubleParameters.contains(ParameterType)) Result.setValue<BloombergVector>(*(DoubleParameters.value(ParameterType)));
	else if (ParameterType == static_cast<qint32>(wstParameters::Trigger)) Result.setValue(TriggerStruc);
	return Result;
}

bool WatFalPrior::HasParameter(qint32 ParameterType) const {
	return
		IntParameters.contains(ParameterType)
		|| DoubleParameters.contains(ParameterType)
		|| (ParameterType == static_cast<qint32>(wstParameters::Trigger) && !TriggerStruc.isEmpty())
	;
}

void WatFalPrior::SetParameter(qint32 ParameterType, const QString& val) {
	switch (static_cast<wstParameters>(ParameterType)) {
	case wstParameters::RedemptionShare:
	case wstParameters::AdditionalCollateralShare:
	case wstParameters::TestTargetOverride:
	case wstParameters::IRRtoEquityTarget:{
		BloombergVector* TempV = new BloombergVector(val);
		if (DoubleParameters.contains(ParameterType))
			delete DoubleParameters[ParameterType];
		if (TempV->IsEmpty()) {
			DoubleParameters.remove(ParameterType);
			delete TempV;
		}
		else DoubleParameters[ParameterType] = TempV;
	break;
	}
	case wstParameters::Trigger:
		TriggerStruc = val;
	break;
	case wstParameters::SeniorityGroupLevel:
	case wstParameters::RedemptionGroupLevel:
	case wstParameters::CouponIndex:
	case wstParameters::ReserveIndex:
	{
		IntegerVector* TempV = new IntegerVector(val);
		TempV->SetShift(-1);
		if (IntParameters.contains(ParameterType))
			delete IntParameters[ParameterType];
		if (TempV->IsEmpty()) {
			IntParameters.remove(ParameterType);
			delete TempV;
		}
		else IntParameters[ParameterType] = TempV;
	}
	break;
	default:
	{
		IntegerVector* TempV = new IntegerVector(val);
		if (IntParameters.contains(ParameterType))
			delete IntParameters[ParameterType];
		if (TempV->IsEmpty()) {
			IntParameters.remove(ParameterType);
			delete TempV;
		}
		else IntParameters[ParameterType] = TempV;
	}
	}
}

void WatFalPrior::RemoveParameter(qint32 ParameterType) {
	if (DoubleParameters.contains(ParameterType)) {
		delete DoubleParameters[ParameterType];
		DoubleParameters.remove(ParameterType);
	}
	if (IntParameters.contains(ParameterType)) {
		delete IntParameters[ParameterType];
		IntParameters.remove(ParameterType);
	}
	if (ParameterType == static_cast<qint32>(wstParameters::Trigger)) TriggerStruc=QString();
}

QString WatFalPrior::ToString() const {
	QString Result="Waterfall Step\n";
	Result += QString("Type: %1\nTrigger Structure: %2").arg(static_cast<qint32>(PriorityType)).arg(TriggerStruc);
	for (auto i = IntParameters.constBegin(); i != IntParameters.constEnd(); ++i) 
		Result += QString("Parameter %1: %2\n").arg(i.key()).arg(i.value()->GetVector());
	for (auto i = DoubleParameters.constBegin(); i != DoubleParameters.constEnd(); ++i)
		Result += QString("Parameter %1: %2\n").arg(i.key()).arg(i.value()->GetVector());
	return Result;
}

QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows) {
	return flows.LoadOldVersion(stream);
}
QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows) {
	stream << flows.IntParameters.size();
	for (auto i = flows.IntParameters.constBegin(); i != flows.IntParameters.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	stream << flows.DoubleParameters.size();
	for (auto i = flows.DoubleParameters.constBegin(); i != flows.DoubleParameters.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	stream << qint32(flows.PriorityType) << flows.TriggerStruc;
	return stream;
}