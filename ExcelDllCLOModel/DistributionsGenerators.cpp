#include "DistributionsGenerators.h"
#ifdef ProbabilisticModel
#include <QTime>
#include <boost/math/distributions.hpp>

DistributionsGenerator::DistributionsGenerator()
	:Copula(IndependenceCopula)
	,PeriodsLegth(0)
	,NeedRecalculation(true)
	,CopulaParameter(10.0)
{
	qsrand(QTime::currentTime().msec());
	for(int i=0;i<Dimensionality;i++){
		for(int j=0;j<Dimensionality;j++){
			if(j==i) CopulaVarianceCovarianceMatrix(i,j)=1.0;
			else CopulaVarianceCovarianceMatrix(i,j)=0.0;
		}
		BBGVector[i]="0";
		Drifts[i]=0.0;
		TimePars[i]=0.0;
		MarginsDistribution[i]=Normal;
	}
}
void DistributionsGenerator::SetBBGVector(VectorType VecType, const QString& NewVect){
	if(NewVect.isEmpty()) BBGVector[VecType]="0";
	else BBGVector[VecType]=NewVect;
	NeedRecalculation=true;
}
void DistributionsGenerator::SetRandSeed(int a){qsrand(a);}
QHash<DistributionsGenerator::VectorType,QString> DistributionsGenerator::GetResultVector()const{
	if(NeedRecalculation) CalculateResult();
	QHash<VectorType,QString> ResultVector;
	QString TempResult;
	QList<qreal> TempList;
	for (int j=0;j<Dimensionality;j++){
		TempResult.clear();
		TempList=Result.values(static_cast<VectorType>(j));
		for(QList<qreal>::const_iterator i=TempList.constBegin();i!=TempList.constEnd();i++){
			if(i!=TempList.constBegin()) TempResult+=" 1S ";
			TempResult+=QString::number(*i,'f',4);
		}
		ResultVector.insert(static_cast<VectorType>(j),TempResult);
	}
	return ResultVector;
}
const QHash<DistributionsGenerator::VectorType,qreal>& DistributionsGenerator::GetResultValues() const{
	if(NeedRecalculation) CalculateResult();
	return Result;
}
void DistributionsGenerator::CalculateResult()const{
	if(PeriodsLegth<=0) return;
	//Generate the shocks
	switch(Copula){

	case GaussianCopula:
	break;
	default: //Independence

	break;
	}
	NeedRecalculation=false;
}
void DistributionsGenerator::SetCopulaVarianceCovarianceMatrix(const Eigen::Matrix<double,Dimensionality,Dimensionality>& a){
	Eigen::Matrix<double,Dimensionality,Dimensionality> b(a);
	b.transposeInPlace();
	if(a==b) CopulaVarianceCovarianceMatrix=a;
	else throw NotSimmetricVar;
}
void DistributionsGenerator::SetCopulaVarianceCovarianceMatrix(double a[Dimensionality][Dimensionality]){
	Eigen::Matrix<double,Dimensionality,Dimensionality> b;
	for(int i=0;i<Dimensionality;i++){
		for(int j=0;j<Dimensionality;j++){
			b(i,j)=a[i][j];
		}
	}
	SetCopulaVarianceCovarianceMatrix(b);
}
#endif