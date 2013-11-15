#ifdef ModelNightly
#include "DistributionsGenerators.h"
#include <QTime>
#include <boost/math/distributions.hpp>
#include <qmath.h>
DistributionsGenerator::DistributionsGenerator()
	:Copula(IndependenceCopula)
	,PeriodsLegth(0)
	,NeedRecalculation(true)
	,CopulaParameter(10.0)
{
	RandNumGen.seed(QTime::currentTime().msec());
	for(int i=0;i<Dimensionality;i++){
		for(int j=0;j<Dimensionality;j++){
			if(j==i) CopulaVarianceCovarianceMatrix(i,j)=1.0;
			else CopulaVarianceCovarianceMatrix(i,j)=0.0;
		}
		BBGVector[i]="0";
		Drifts[i]=0.0;
		TimePars[i]=0.0;
		MarginsDistribution[i]=Normal;
		MarginStdDev[i]=1.0;
	}
}
void DistributionsGenerator::SetBBGVector(VectorType VecType, const QString& NewVect){
	if(NewVect.isEmpty()) BBGVector[VecType]="0";
	else BBGVector[VecType]=NewVect;
	NeedRecalculation=true;
}
void DistributionsGenerator::SetRandSeed(quint32 a){RandNumGen.seed(a);}
QHash<DistributionsGenerator::VectorType,QString> DistributionsGenerator::GetResultVector(){
	if(NeedRecalculation) CalculateResult();
	QHash<VectorType,QString> ResultVector;
	QString TempResult;
	QList<qreal> TempList;
	for (int j=0;j<Dimensionality;j++){
		TempResult.clear();
		TempList=Result.value(static_cast<VectorType>(j));
		for(QList<qreal>::const_iterator i=TempList.constBegin();i!=TempList.constEnd();i++){
			if(i!=TempList.constBegin()) TempResult+=" 1S ";
			TempResult+=QString::number(*i,'f',4);
		}
		ResultVector.insert(static_cast<VectorType>(j),TempResult);
	}
	return ResultVector;
}
const QHash<DistributionsGenerator::VectorType,QList<qreal> >& DistributionsGenerator::GetResultValues(){
	if(NeedRecalculation) CalculateResult();
	return Result;
}
void DistributionsGenerator::CalculateResult(){
	Result.clear();
	if(PeriodsLegth<=0) return;
	//Generate the shocks
	for (int h=0;h<PeriodsLegth;h++){
		switch(Copula){
		case GaussianCopula:
		break;
		default:{ //Independence
			boost::random::uniform_real_distribution<qreal> dist(0.0, 1.0);
			for (int i=0;i<Dimensionality;i++){
				Result[static_cast<VectorType>(i)].append(MarginQuantile(static_cast<VectorType>(i),dist(RandNumGen)));
			}
		}break;

		}
	}
	NeedRecalculation=false;
}
void DistributionsGenerator::SetCopulaVarianceCovarianceMatrix(const Eigen::Matrix<double,Dimensionality,Dimensionality>& a){
	for(int i=0;i<Dimensionality;i++){
		if(a(i,i)<0.0){
			throw NegativeVariance;
			return;
		}
	}
	Eigen::Matrix<double,Dimensionality,Dimensionality> b(a);
	b.transposeInPlace();
	if(a==b) CopulaVarianceCovarianceMatrix=a;
	else throw NotSimmetricVar;
}
void DistributionsGenerator::SetCopulaVarianceCovarianceMatrix(qreal a[Dimensionality][Dimensionality]){
	Eigen::Matrix<double,Dimensionality,Dimensionality> b;
	for(int i=0;i<Dimensionality;i++){
		for(int j=0;j<Dimensionality;j++){
			b(i,j)=a[i][j];
		}
	}
	SetCopulaVarianceCovarianceMatrix(b);
}
qreal DistributionsGenerator::MarginQuantile(VectorType VecType,qreal Prob){
	if(Prob<0.0 || Prob>1.0){
		throw InavlidQuantile;
		return 0.0;
	}
	if(MarginStdDev[VecType]<0.0){
		throw NegativeVariance;
		return 0.0;
	}
	switch(MarginsDistribution[VecType]){
	case Uniform:{
			if(MarginStdDev[VecType]==0.0) return 0.0; //Deterministic
			else{
				boost::math::uniform_distribution<qreal> UnifSample(-qSqrt(3.0)*MarginStdDev[VecType],qSqrt(3.0)*MarginStdDev[VecType]);
				return boost::math::quantile(UnifSample,Prob);
			}
		}break;
	case tStudent:{
			if(MarginStdDev[VecType]<1.0) return 0.0; //0 degrees of freedom
			else{
				boost::math::students_t tSample(static_cast<qint32>((2.0*MarginStdDev[VecType]*MarginStdDev[VecType])/((MarginStdDev[VecType]*MarginStdDev[VecType])+1.0)));
				return boost::math::quantile(tSample,Prob);
			}
		}break;
	case ChiSqauare:{
			if(MarginStdDev[VecType]*MarginStdDev[VecType]<2.0) return 0.0; //0 degrees of freedom
			else{
				boost::math::chi_squared ChiSample(qMax(1,static_cast<qint32>((MarginStdDev[VecType]*MarginStdDev[VecType])/2.0)));
				return boost::math::quantile(ChiSample,Prob);
			}
		}break;
	default:{ //Normal
			if(MarginStdDev[VecType]==0.0) return 0.0; //Deterministic
			else{
				boost::math::normal NormalSample(0.0,MarginStdDev[VecType]);
				return boost::math::quantile(NormalSample,Prob);
			}
		}
	}
}
void DistributionsGenerator::SetCopulaVariance(Eigen::Matrix<qreal,Dimensionality,1> Diag){
	for(int i=0;i<Dimensionality;i++){
		if(Diag(i)<0.0){
			throw NegativeVariance;
			return;
		}
	}
	for(int i=0;i<Dimensionality;i++){
		CopulaVarianceCovarianceMatrix(i,i)=Diag(i);
	}
}
void DistributionsGenerator::SetCopulaVariance(qreal a[Dimensionality]){
	Eigen::Matrix<qreal,Dimensionality,1> Diag;
	for(int i=0;i<Dimensionality;i++){
		Diag(i)=a[i];
	}
	return SetCopulaVariance(Diag);
}
void DistributionsGenerator::SetCorrelationMatrix(Eigen::Matrix<qreal,Dimensionality,Dimensionality> a){
	for(int i=0;i<Dimensionality;i++){
		for(int j=0;j<Dimensionality;j++){
			if(i==j){
				if(a(i,j)!=1.0){
					throw InvalidCorrelationMatrix;
					return;
				}
			}
			else{
				if(a(i,j)>1.0 || a(i,j)<-1.0){
					throw InvalidCorrelationMatrix;
					return;
				}
			}
		}
	}
	Eigen::Matrix<double,Dimensionality,Dimensionality> b(a);
	b.transposeInPlace();
	if(a!=b){
		throw InvalidCorrelationMatrix;
		return;
	}
	for(int i=0;i<Dimensionality;i++){
		for(int j=0;j<Dimensionality;j++){
			if(i==j) continue;
			CopulaVarianceCovarianceMatrix(i,j)=a(i,j)*qSqrt(CopulaVarianceCovarianceMatrix(i,i))*qSqrt(CopulaVarianceCovarianceMatrix(j,j));
		}
	}
}
void DistributionsGenerator::SetCorrelationMatrix(qreal a[Dimensionality][Dimensionality]){
	Eigen::Matrix<qreal,Dimensionality,Dimensionality> b;
	for(int i=0;i<Dimensionality;i++){
		for(int j=0;j<Dimensionality;j++){
			b(i,j)=a[i][j];
		}
	}
	return SetCorrelationMatrix(b);
}
#endif