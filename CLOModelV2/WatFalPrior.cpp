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
	, FilledNullAnchors(a.FilledNullAnchors)
	, TriggerStruc(a.TriggerStruc)
{
	for (auto i = a.IntParameters.constBegin(); i != a.IntParameters.constEnd(); ++i)
		IntParameters.insert(i.key(), new IntegerVector(*(i.value())));
	for (auto i = a.DoubleParameters.constBegin(); i != a.DoubleParameters.constEnd(); ++i)
		DoubleParameters.insert(i.key(), new BloombergVector(*(i.value())));
}
WatFalPrior& WatFalPrior::operator=(const WatFalPrior& a){
	ClearParameters();
	for (auto i = a.IntParameters.constBegin(); i != a.IntParameters.constEnd(); ++i)
		IntParameters.insert(i.key(), new IntegerVector(*(i.value())));
	for (auto i = a.DoubleParameters.constBegin(); i != a.DoubleParameters.constEnd(); ++i)
		DoubleParameters.insert(i.key(), new BloombergVector(*(i.value())));
	PriorityType = a.PriorityType;
	TriggerStruc = a.TriggerStruc;
	FilledNullAnchors = a.FilledNullAnchors;
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
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "Interest needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Interest needs a Seniority Group Level Parameter\n";
		if (GetParameter(wstParameters::CouponIndex).value<IntegerVector>().IsEmpty(0)) Result += "Interest needs a Coupon Index Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_Principal:
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "Principal needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Principal needs a Seniority Group Level Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_OCTest:
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "OC Test needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "OC Test needs a Seniority Group Level Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup)) {
			if (GetParameter(wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty(1)) Result += "OC Test needs a Redemption Group Parameter\n";
			if (GetParameter(wstParameters::RedemptionGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "OC Test needs a Redemption Group Level Parameter\n";
			if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0,1.0)) Result += "OC Test needs a Redemption Share Parameter\n";
		}
		if (HasParameter(wstParameters::AdditionalCollateralShare)) {
			if (GetParameter(wstParameters::AdditionalCollateralShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "OC Test needs an Additional Collateral Share Parameter\n";
		}
		if (HasParameter(wstParameters::TestTargetOverride)) {
			if (GetParameter(wstParameters::TestTargetOverride).value<BloombergVector>().IsEmpty(0.0)) Result += "OC Test needs a target Override Parameter\n";
		}
		break;
	case WatFalPrior::WaterfallStepType::wst_ICTest:
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "IC Test needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "IC Test needs a Seniority Group Level Parameter\n";
		if (GetParameter(wstParameters::CouponIndex).value<IntegerVector>().IsEmpty(0)) Result += "IC Test needs a Coupon Index Parameter\n";
		if (HasParameter(wstParameters::TestTargetOverride)) {
			if (GetParameter(wstParameters::TestTargetOverride).value<BloombergVector>().IsEmpty(0.0)) Result += "IC Test needs a target Override Parameter\n";
		}
		break;
	case WatFalPrior::WaterfallStepType::wst_DeferredInterest:
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "Deferred Interest needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Deferred Interest needs a Seniority Group Level Parameter\n";
		if (GetParameter(wstParameters::CouponIndex).value<IntegerVector>().IsEmpty(0)) Result += "Deferred Interest needs a Coupon Index Parameter\n";
		break;
	case WatFalPrior::WaterfallStepType::wst_Excess:
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1,3)) Result += "Excess Spread needs a Source of Funding Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup)) {
			if (GetParameter(wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty(1)) Result += "Excess Spread needs a Redemption Group Parameter\n";
			if (GetParameter(wstParameters::RedemptionGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Excess Spread needs a Redemption Group Level Parameter\n";
			if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "Excess Spread needs a Redemption Share Parameter\n";
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal:
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1, 3)) Result += "Reinvest Principal needs a Source of Funding Parameter\n";
		if (GetParameter(wstParameters::AdditionalCollateralShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "Reinvest Principal needs a Additional Collateral Share Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_ReserveReplenish:
		if (GetParameter(wstParameters::ReserveIndex).value<IntegerVector>().IsEmpty(0)) Result += "Reserve Replenish needs a Reserve Index Parameter\n";
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1, 2)) Result += "Reserve Replenish needs a Source of Funding Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_Turbo:
		if (GetParameter(wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty(1)) Result += "Turbo needs a Redemption Group Parameter\n";
		if (GetParameter(wstParameters::RedemptionGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Turbo needs a Redemption Group Level Parameter\n";
		if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "Turbo needs a Redemption Share Parameter\n";
		break;
	case WatFalPrior::WaterfallStepType::wst_PDL:
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "PDL Cure needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "PDL Cure needs a Seniority Group Level Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup)) {
			if (GetParameter(wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty(1)) Result += "PDL Cure needs a Redemption Group Parameter\n";
			if (GetParameter(wstParameters::RedemptionGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "PDL Cure needs a Redemption Group Level Parameter\n";
			if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "PDL Cure needs a Redemption Share Parameter\n";
		}
		break;
	case WatFalPrior::WaterfallStepType::wst_FeesFromExcess:
		if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "Fees from Excess Spread needs a Redemption Share Parameter\n";
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1, 3)) Result += "Fees from Excess Spread needs a Source of Funding Parameter\n";
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
	stream >> TempInt >> TriggerStruc >> FilledNullAnchors;
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

void WatFalPrior::FillMissingAnchors(const QDate& a) {
	for (auto i = IntParameters.begin(); i != IntParameters.end(); ++i) {
		if (i.value()->GetAnchorDate().isNull()){
			FilledNullAnchors.insert(i.key());
			i.value()->SetAnchorDate(a);
		}
	}
	for (auto i = DoubleParameters.begin(); i != DoubleParameters.end(); ++i) {
		if (i.value()->GetAnchorDate().isNull()) {
			FilledNullAnchors.insert(i.key());
			i.value()->SetAnchorDate(a);
		}
	}	
}

void WatFalPrior::ResetMissinAnchors() {
	for (auto i = IntParameters.begin(); i != IntParameters.end(); ++i) {
		if (FilledNullAnchors.contains(i.key())) {
			i.value()->RemoveAnchorDate();
		}
	}
	for (auto i = DoubleParameters.begin(); i != DoubleParameters.end(); ++i) {
		if (FilledNullAnchors.contains(i.key())) {
			i.value()->RemoveAnchorDate();
		}
	}
	FilledNullAnchors.clear();
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
	stream << qint32(flows.PriorityType) << flows.TriggerStruc << flows.FilledNullAnchors;
	return stream;
}