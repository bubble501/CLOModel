#include "WatFalPrior.h"
#include "Private/WatFalPrior_p.h"
#include "IntegerVector.h"
#include "BloombergVector.h"
#include <QMetaType>
#include "Private/InternalItems.h"
DEFINE_PUBLIC_COMMONS(WatFalPrior)
DEFINE_PUBLIC_COMMONS_COPY(WatFalPrior)
WatFalPriorPrivate::~WatFalPriorPrivate()
{

}
WatFalPriorPrivate::WatFalPriorPrivate(WatFalPrior *q,const WatFalPriorPrivate& other)
	:BackwardInterfacePrivate(q,other)
    , PriorityType(other.PriorityType)
    , FilledNullAnchors(other.FilledNullAnchors)
    , TriggerStruc(other.TriggerStruc)
    , m_AccrueOrPay(other.m_AccrueOrPay)
{
    for (auto i = other.IntParameters.constBegin(); i != other.IntParameters.constEnd(); ++i)
        IntParameters.insert(i.key(), std::make_shared< IntegerVector>(*(i.value())));
    for (auto i = other.DoubleParameters.constBegin(); i != other.DoubleParameters.constEnd(); ++i)
        DoubleParameters.insert(i.key(), std::make_shared< BloombergVector>(*(i.value())));
}
WatFalPriorPrivate::WatFalPriorPrivate(WatFalPrior *q)
	:BackwardInterfacePrivate(q)
    , PriorityType(WatFalPrior::WaterfallStepType::wst_Excess)
    , m_AccrueOrPay(WatFalPrior::wstAccrueOrPay::Invalid)
{}
WatFalPrior::WatFalPrior(WatFalPriorPrivate *d, const WatFalPrior& other)
	:BackwardInterface(d,other)
{}
WatFalPrior& WatFalPrior::operator=(const WatFalPrior& other){
	Q_D(WatFalPrior);
	BackwardInterface::operator=(other);
    ClearParameters();
    d->PriorityType = other.d_func()->PriorityType;
    d->FilledNullAnchors = other.d_func()->FilledNullAnchors;
    d->TriggerStruc = other.d_func()->TriggerStruc;
    d->m_AccrueOrPay = other.d_func()->m_AccrueOrPay;
    for (auto i = other.d_func()->IntParameters.constBegin(); i != other.d_func()->IntParameters.constEnd(); ++i)
        d->IntParameters.insert(i.key(), std::make_shared< IntegerVector>(*(i.value())));
    for (auto i = other.d_func()->DoubleParameters.constBegin(); i != other.d_func()->DoubleParameters.constEnd(); ++i)
        d->DoubleParameters.insert(i.key(), std::make_shared< BloombergVector>(*(i.value())));
	return *this;
}
WatFalPrior::WatFalPrior(WatFalPriorPrivate *d)
	:BackwardInterface(d)
{}

void WatFalPrior::SetPriorityType(WaterfallStepType a)
{
    Q_D(WatFalPrior);
    d->PriorityType = a;
}

QString WatFalPrior::ReadyToCalculate() const
{
    Q_D(const WatFalPrior);
	QString Result = "";
    switch (d->PriorityType) {
	case WatFalPrior::WaterfallStepType::wst_juniorFees:
		if (!HasParameter(wstParameters::PayAccrue)) Result += "Junior Fees need an Accrue or Pay Parameter\n";
	case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
	case WatFalPrior::WaterfallStepType::wst_SeniorFees:
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1,2)) Result += "Expenses and Fees need a Source of Funding Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_Interest:
		if (!HasParameter(wstParameters::PayAccrue)) Result += "Interest needs an Accrue or Pay Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "Interest needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Interest needs a Seniority Group Level Parameter\n";
		if (GetParameter(wstParameters::CouponIndex).value<IntegerVector>().IsEmpty(0,(1 << MaximumInterestsTypes)-1)) Result += "Interest needs a Coupon Index Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_Principal:
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "Principal needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Principal needs a Seniority Group Level Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_ReinvestmentTest:
	case WatFalPrior::WaterfallStepType::wst_OCTest:
		if (!HasParameter(wstParameters::PayAccrue)) Result += "OC Test needs an Accrue or Pay Parameter\n";
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1, 2)) Result += "OC Test needs a Source of Funding Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "OC Test needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "OC Test needs a Seniority Group Level Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup)) {
			if (GetParameter(wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty(1)) Result += "OC Test needs a Redemption Group Parameter\n";
		}
		if (GetParameter(wstParameters::RedemptionGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "OC Test needs a Redemption Group Level Parameter\n";
		if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "OC Test needs a Redemption Share Parameter\n";
		if (GetParameter(wstParameters::AdditionalCollateralShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "OC Test needs an Additional Collateral Share Parameter\n";
		if (HasParameter(wstParameters::AdditionalCollateralShare) && HasParameter(wstParameters::RedemptionShare)) {
			BloombergVector ACS = GetParameter(wstParameters::AdditionalCollateralShare).value<BloombergVector>();
			BloombergVector RS = GetParameter(wstParameters::RedemptionShare).value<BloombergVector>();
			if ((RS + ACS).IsEmpty(0.00001, 1.0)) Result += "OC Test's Additional Collateral Share Parameter and Redemption Share parameters must sum to a value greater than 0 and below 100% at all times\n";
		}
		if (HasParameter(wstParameters::TestTargetOverride)) {
			if (GetParameter(wstParameters::TestTargetOverride).value<BloombergVector>().IsEmpty(0.0)) Result += "OC Test needs a target Override Parameter\n";
		}
		break;
	case WatFalPrior::WaterfallStepType::wst_ICTest:
		if (!HasParameter(wstParameters::PayAccrue)) Result += "IC Test needs an Accrue or Pay Parameter\n";
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1, 2)) Result += "IC Test needs a Source of Funding Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "IC Test needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "IC Test needs a Seniority Group Level Parameter\n";
		if (GetParameter(wstParameters::CouponIndex).value<IntegerVector>().IsEmpty(0, (1 << MaximumInterestsTypes) - 1)) Result += "IC Test needs a Coupon Index Parameter\n";
		if (HasParameter(wstParameters::TestTargetOverride)) {
			if (GetParameter(wstParameters::TestTargetOverride).value<BloombergVector>().IsEmpty(0.0)) Result += "IC Test needs a target Override Parameter\n";
		}
		break;
	case WatFalPrior::WaterfallStepType::wst_DeferredInterest:
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1, 2)) Result += "Deferred Interest needs a Source of Funding Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().IsEmpty(1)) Result += "Deferred Interest needs a Seniority Group Parameter\n";
		if (GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Deferred Interest needs a Seniority Group Level Parameter\n";
		if (GetParameter(wstParameters::CouponIndex).value<IntegerVector>().IsEmpty(0, (1 << MaximumInterestsTypes) - 1)) Result += "Deferred Interest needs a Coupon Index Parameter\n";
		break;
	case WatFalPrior::WaterfallStepType::wst_Excess:
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1,3)) Result += "Excess Spread needs a Source of Funding Parameter\n";
		if (HasParameter(wstParameters::RedemptionGroup)) {
			if (GetParameter(wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty(1)) Result += "Excess Spread needs a Redemption Group Parameter\n";
			if (GetParameter(wstParameters::RedemptionGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Excess Spread needs a Redemption Group Level Parameter\n";
			if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "Excess Spread needs a Redemption Share Parameter\n";
		}
		else {
			if (HasParameter(wstParameters::RedemptionShare)) Result += "Excess Spread cannot have a Redemption Share Parameter\n";
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
		}
		if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "PDL Cure needs a Redemption Share Parameter\n";
		break;
	case WatFalPrior::WaterfallStepType::wst_FeesFromExcess:
		if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "Fees from Excess Spread needs a Redemption Share Parameter\n";
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1, 3)) Result += "Fees from Excess Spread needs a Source of Funding Parameter\n";
	break;
	case WatFalPrior::WaterfallStepType::wst_AllocPrepayFees:
		if (GetParameter(wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty(1)) Result += "Allocation of prepayment fees needs a Redemption Group Parameter\n";
		if (GetParameter(wstParameters::RedemptionGroupLevel).value<IntegerVector>().IsEmpty(0)) Result += "Allocation of prepayment needs a Redemption Group Level Parameter\n";
		if (GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().IsEmpty(0.0, 1.0)) Result += "Allocation of prepayment fees needs a Redemption Share Parameter\n";
		if (GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().IsEmpty(1, 2)) Result += "Allocation of prepayment fees needs a Source of Funding Parameter\n";
	break;
	default: 
		Result += "Invalid Waterfall Step\n";
	}
	return Result;
}
QDataStream& WatFalPrior::LoadOldVersion(QDataStream& stream) 
{
    Q_D( WatFalPrior);
	qint32 TempInt,TempSize;
	quint8 TempUChar;
	IntegerVector TempIV;
	BloombergVector TempBV;
	stream >> TempSize;
	for (int i = 0; i < TempSize; i++) {
		stream >> TempInt;
		TempIV.SetLoadProtocolVersion(loadProtocolVersion()); stream >> TempIV;
        d->IntParameters.insert(TempInt, std::make_shared< IntegerVector>(TempIV));
	}
	stream >> TempSize;
	for (int i = 0; i < TempSize; i++) {
		stream >> TempInt;
        TempBV.SetLoadProtocolVersion(loadProtocolVersion()); stream >> TempBV;
        d->DoubleParameters.insert(TempInt, std::make_shared< BloombergVector>(TempBV));
	}
    stream >> TempInt >> d->TriggerStruc >> TempUChar >> d->FilledNullAnchors;
    d->PriorityType = static_cast<WaterfallStepType>(TempInt);
    d->m_AccrueOrPay = static_cast<wstAccrueOrPay>(TempUChar);
	ResetProtocolVersion();
	return stream;
}

void WatFalPrior::ClearParameters()
{
    Q_D( WatFalPrior);
    d->IntParameters.clear();
    d->DoubleParameters.clear();
    d->TriggerStruc = QString();
}

WatFalPrior::WaterfallStepType WatFalPrior::GetPriorityType() const
{
    Q_D(const WatFalPrior);
    return d->PriorityType;
}

WatFalPrior::~WatFalPrior()
{
	ClearParameters();
}

QVariant WatFalPrior::GetParameter(qint32 ParameterType) const
{
    Q_D(const WatFalPrior);
	QVariant Result;
    if (d->IntParameters.contains(ParameterType)) 
        Result.setValue<IntegerVector>(*(d->IntParameters.value(ParameterType)));
    else if (d->DoubleParameters.contains(ParameterType)) 
        Result.setValue<BloombergVector>(*(d->DoubleParameters.value(ParameterType)));
	else if (ParameterType == static_cast<qint32>(wstParameters::Trigger)) 
        Result.setValue(d->TriggerStruc);
	else if (ParameterType == static_cast<qint32>(wstParameters::PayAccrue))
        Result.setValue(static_cast<quint8>(d->m_AccrueOrPay));
	return Result;
}

QVariant WatFalPrior::GetParameter(wstParameters ParameterType) const
{
    return GetParameter(static_cast<qint32>(ParameterType));
}

bool WatFalPrior::HasParameter(qint32 ParameterType) const
{
    Q_D(const WatFalPrior);
	return
        d->IntParameters.contains(ParameterType)
        || d->DoubleParameters.contains(ParameterType)
        || (ParameterType == static_cast<qint32>(wstParameters::Trigger) && !d->TriggerStruc.isEmpty())
        || (ParameterType == static_cast<qint32>(wstParameters::PayAccrue) && d->m_AccrueOrPay != wstAccrueOrPay::Invalid)
	;
}

bool WatFalPrior::HasParameter(wstParameters ParameterType) const
{
    return HasParameter(static_cast<qint32>(ParameterType));
}

void WatFalPrior::SetParameter(qint32 ParameterType, const QString& val)
{
    Q_D( WatFalPrior);
	switch (static_cast<wstParameters>(ParameterType)) {
	case wstParameters::RedemptionShare:
	case wstParameters::AdditionalCollateralShare:
	case wstParameters::TestTargetOverride:
	case wstParameters::IRRtoEquityTarget:{
        auto TempV = std::make_shared<BloombergVector>(val);
        auto parIter = d->DoubleParameters.find(ParameterType);
        if (parIter == d->DoubleParameters.end())
            d->DoubleParameters.insert(ParameterType, TempV);
        else if (TempV->IsEmpty())
            d->DoubleParameters.erase(parIter);
        else
            parIter.value() = TempV;
	break;
	}
	case wstParameters::Trigger:
        d->TriggerStruc = NormaliseTriggerStructure(val);
	break;
	case wstParameters::PayAccrue:{
		if (val.isEmpty()) {
            d->m_AccrueOrPay = wstAccrueOrPay::Invalid;
			return;
		}
		bool CheckConv;
		int PorA = val.toInt(&CheckConv);
		if (!CheckConv) {
            d->m_AccrueOrPay = wstAccrueOrPay::Invalid;
			return;
		}
		if (PorA < static_cast<quint8>(wstAccrueOrPay::Invalid) || PorA > static_cast<quint8>(wstAccrueOrPay::AccrueAndPay)) {
            d->m_AccrueOrPay = wstAccrueOrPay::Invalid;
			return; 
		}
        d->m_AccrueOrPay = static_cast<wstAccrueOrPay>(PorA);
	}
	break;
	case wstParameters::SeniorityGroupLevel:
	case wstParameters::RedemptionGroupLevel:
	case wstParameters::CouponIndex:
	case wstParameters::ReserveIndex:
	{
        auto TempV = std::make_shared<IntegerVector>(val);
        TempV->SetShift(-1);
        auto parIter = d->IntParameters.find(ParameterType);
        if (parIter == d->IntParameters.end())
            d->IntParameters.insert(ParameterType, TempV);
        else if (TempV->IsEmpty())
            d->IntParameters.erase(parIter);
        else
            parIter.value() = TempV;
	}
	break;
	default:
	{
        auto TempV = std::make_shared<IntegerVector>(val);
        auto parIter = d->IntParameters.find(ParameterType);
        if (parIter == d->IntParameters.end())
            d->IntParameters.insert(ParameterType, TempV);
        else if (TempV->IsEmpty())
            d->IntParameters.erase(parIter);
        else
            parIter.value() = TempV;
	}
	}
}

void WatFalPrior::SetParameter(wstParameters ParameterType, const QString& val)
{
    SetParameter(static_cast<qint32>(ParameterType), val);
}

void WatFalPrior::RemoveParameter(qint32 ParameterType)
{
    Q_D( WatFalPrior);
    d->DoubleParameters.remove(ParameterType);
    d->IntParameters.remove(ParameterType);
    if (ParameterType == static_cast<qint32>(wstParameters::Trigger)) 
        d->TriggerStruc = QString();
    if (ParameterType == static_cast<qint32>(wstParameters::PayAccrue)) 
        d->m_AccrueOrPay = wstAccrueOrPay::Invalid;
}

void WatFalPrior::RemoveParameter(wstParameters ParameterType)
{
    RemoveParameter(static_cast<qint32>(ParameterType));
}

QString WatFalPrior::ToString() const
{
    Q_D(const WatFalPrior);
	QString Result="Waterfall Step\n";
    Result += QString("Type: %1\nTrigger Structure: %2\n").arg(static_cast<qint32>(d->PriorityType)).arg(d->TriggerStruc);
    switch (d->m_AccrueOrPay) {
	case WatFalPrior::wstAccrueOrPay::Accrue:
        Result += '\n' + (d->PriorityType == WaterfallStepType::wst_Interest || d->PriorityType == WaterfallStepType::wst_DeferredInterest) ? QString("Accrue") : QString("Calculate");
	break;
	case WatFalPrior::wstAccrueOrPay::Pay:
        Result += '\n' + (d->PriorityType == WaterfallStepType::wst_Interest || d->PriorityType == WaterfallStepType::wst_DeferredInterest) ? QString("Pay") : QString("Cure");
	break;
	case WatFalPrior::wstAccrueOrPay::AccrueAndPay:
        Result += '\n' + (d->PriorityType == WaterfallStepType::wst_Interest || d->PriorityType == WaterfallStepType::wst_DeferredInterest) ? QString("Accrue") : QString("Calculate");
		Result += " and ";
        Result += (d->PriorityType == WaterfallStepType::wst_Interest || d->PriorityType == WaterfallStepType::wst_DeferredInterest) ? QString("Pay") : QString("Cure");
	break;
	default:
		break;
	}
    for (auto i = d->IntParameters.constBegin(); i != d->IntParameters.constEnd(); ++i)
		Result += QString("Parameter %1: %2\n").arg(i.key()).arg(i.value()->GetVector());
    for (auto i = d->DoubleParameters.constBegin(); i != d->DoubleParameters.constEnd(); ++i)
		Result += QString("Parameter %1: %2\n").arg(i.key()).arg(i.value()->GetVector());
	return Result;
}

void WatFalPrior::FillMissingAnchors(const QDate& a)
{
    Q_D( WatFalPrior);
    for (auto i = d->IntParameters.begin(); i != d->IntParameters.end(); ++i) {
		if (i.value()->GetAnchorDate().isNull()){
            d->FilledNullAnchors.insert(i.key());
			i.value()->SetAnchorDate(a);
		}
	}
    for (auto i = d->DoubleParameters.begin(); i != d->DoubleParameters.end(); ++i) {
		if (i.value()->GetAnchorDate().isNull()) {
            d->FilledNullAnchors.insert(i.key());
			i.value()->SetAnchorDate(a);
		}
	}	
}

void WatFalPrior::ResetMissinAnchors()
{
    Q_D( WatFalPrior);
    for (auto i = d->IntParameters.begin(); i != d->IntParameters.end(); ++i) {
        if (d->FilledNullAnchors.contains(i.key())) {
			i.value()->RemoveAnchorDate();
		}
	}
    for (auto i = d->DoubleParameters.begin(); i != d->DoubleParameters.end(); ++i) {
        if (d->FilledNullAnchors.contains(i.key())) {
			i.value()->RemoveAnchorDate();
		}
	}
    d->FilledNullAnchors.clear();
}

QString WatFalPrior::CodeForDialog() const
{
    Q_D(const WatFalPrior);
    return QString("%1").arg(static_cast<qint16>(d->PriorityType)) + '#' +
		GetParameter(wstParameters::SeniorityGroup).value<IntegerVector>().GetVector() + '#' +
		GetParameter(wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetVector() + '#' +
		GetParameter(wstParameters::RedemptionGroup).value<IntegerVector>().GetVector() + '#' +
		GetParameter(wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetVector() + '#' +
		GetParameter(wstParameters::RedemptionShare).value<BloombergVector>().GetVector() + '#' +
		GetParameter(wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetVector() + '#' +
		GetParameter(wstParameters::SourceOfFunding).value<IntegerVector>().GetVector() + '#' +
		GetParameter(wstParameters::CouponIndex).value<IntegerVector>().GetVector() + '#' +
		GetParameter(wstParameters::TestTargetOverride).value<BloombergVector>().GetVector() + '#' +
		GetParameter(wstParameters::IRRtoEquityTarget).value<BloombergVector>().GetVector() + '#' +
		GetParameter(wstParameters::ReserveIndex).value<IntegerVector>().GetVector() + '#' +
		(HasParameter(wstParameters::PayAccrue) ? QString('#' + GetParameter(wstParameters::PayAccrue).toString() + '#'):QString()) +
		GetParameter(wstParameters::Trigger).toString();
	
	;
}

QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows) {
	return flows.LoadOldVersion(stream);
}
QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows) {
	stream << flows.d_func()->IntParameters.size();
    for (auto i = flows.d_func()->IntParameters.constBegin(); i != flows.d_func()->IntParameters.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
    stream << flows.d_func()->DoubleParameters.size();
    for (auto i = flows.d_func()->DoubleParameters.constBegin(); i != flows.d_func()->DoubleParameters.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
    stream << qint32(flows.d_func()->PriorityType) << flows.d_func()->TriggerStruc << static_cast<quint8>(flows.d_func()->m_AccrueOrPay) << flows.d_func()->FilledNullAnchors;
	return stream;
}