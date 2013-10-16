#include "WatFalPrior.h"

WatFalPrior::WatFalPrior()
	:GroupTarget(1)
	,RedemptionGroup(0)
	,RedemptionShare(0.0)
	,PriorityType(wst_Excess)
{}
WatFalPrior::WatFalPrior(const WatFalPrior& a)
	:GroupTarget(a.GroupTarget)
	,RedemptionGroup(a.RedemptionGroup)
	,RedemptionShare(a.RedemptionShare)
	,PriorityType(a.PriorityType)
{}
WatFalPrior& WatFalPrior::operator=(const WatFalPrior& a){
	GroupTarget=a.GroupTarget;
	RedemptionGroup=a.RedemptionGroup;
	RedemptionShare=a.RedemptionShare;
	PriorityType=a.PriorityType;
	return *this;
}
QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows){
	stream
		<< flows.GroupTarget
		<< flows.RedemptionGroup
		<< flows.RedemptionShare
		<< qint32(flows.PriorityType)
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows){
	qint32 TempInt;
	stream
		>> flows.GroupTarget
		>> flows.RedemptionGroup
		>> flows.RedemptionShare;
	stream>>TempInt;
	flows.PriorityType=WatFalPrior::WaterfallStepType(TempInt);
	return stream;
}