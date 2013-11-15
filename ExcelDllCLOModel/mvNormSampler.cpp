#ifdef ModelNightly
#include "mvNormSampler.h"
#include <Eigen/Eigenvalues>
#include <boost/math/distributions/normal.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/math/constants/constants.hpp>
#include <ctime>
Eigen::MatrixXd mvNormSampler::ExtractSamples(unsigned int NumSamples) const{
	if(!AllValid || NumSamples==0) return Eigen::MatrixXd();
	boost::random::uniform_real_distribution<double> dist(0.0, 1.0);
	if(Dim==1U){ //Univariate case
		Eigen::VectorXd UnivariateResults(NumSamples);
		boost::math::normal NormalUnivariateSample(meanVect(0),VarCovMatrix(0,0));
		for(unsigned int j=0;j<NumSamples;j++){
			UnivariateResults(j)=boost::math::quantile(NormalUnivariateSample,dist(RandNumGen));
		}
	}
	Eigen::EigenSolver<Eigen::MatrixXd> ev(VarCovMatrix);
	Eigen::EigenSolver<Eigen::MatrixXd>::EigenvalueType ComlexEigVal=ev.eigenvalues();
	Eigen::MatrixXd RealEigVal(Dim,Dim); //We know that all the eigenvalues are real so extract the real part from the EigenSolver calculation and root it and put them into a diagonal matrix
	for(unsigned int i=0;i<Dim;i++){
		for(unsigned int j=0;j<Dim;j++){
			if(i==j) RealEigVal(i,j)=sqrt(ComlexEigVal(i).real());
			else RealEigVal(i,j)=0.0;
		}
	}
	Eigen::EigenSolver<Eigen::MatrixXd>::EigenvectorsType ComlexEigVect=ev.eigenvectors();
	Eigen::MatrixXd RealEigVectors(Dim,Dim); //We know that all the eigenvectors are real so extract the real part from the EigenSolver calculation
	for(unsigned int i=0;i<Dim;i++){
		for(unsigned int j=0;j<Dim;j++){
			RealEigVectors(i,j)=ComlexEigVect(i,j).real();
		}
	}

	Eigen::MatrixXd retval=RealEigVectors*RealEigVal*RealEigVectors.transpose();

	Eigen::MatrixXd NormalExtractions(NumSamples,Dim);
	for(unsigned int i=0;i<Dim;i++){
		boost::math::normal NormalSample(meanVect(i),1);
		for(unsigned int j=0;j<NumSamples;j++){
			NormalExtractions(j,i)=boost::math::quantile(NormalSample,dist(RandNumGen));
		}
	}
	return NormalExtractions*retval;
}
double mvNormSampler::GetDensity(const Eigen::VectorXd& Coordinates, bool GetLogDensity) const{
	if(!AllValid) return 0.0;
	if(Coordinates.rows()!=Dim) return 0.0;
	if(Dim==1U){ //Univariate case
		boost::math::normal NormalUnivariate(meanVect(0),VarCovMatrix(0,0));
		boost::math::pdf(NormalUnivariate,Coordinates(0));
	}
	double distval=(Coordinates-meanVect).transpose()*VarCovMatrix.inverse()*(Coordinates-meanVect);
	Eigen::EigenSolver<Eigen::MatrixXd>::EigenvalueType ComplexEigVal=VarCovMatrix.eigenvalues();
	double logdet=0.0;
	for(unsigned int j=0;j<Dim;j++)	logdet+=log(ComplexEigVal(j).real());
	double logretval=-((static_cast<double>(Dim)*log(2.0*boost::math::constants::pi<double>()))+logdet+distval)/2.0;
	if(GetLogDensity) return logretval;
	return exp(logretval);
}
mvNormSampler::mvNormSampler(unsigned int Dimension,const Eigen::VectorXd& mVect,const Eigen::MatrixXd& CovMatr)
	:Dim(Dimension)
	,meanVect(mVect)
	,VarCovMatrix(CovMatr)
{
	CheckValidity();
	CurrentSeed=static_cast<unsigned int>(std::time(NULL));
	RandNumGen.seed(CurrentSeed);
}
mvNormSampler::mvNormSampler(unsigned int Dimension)
	:Dim(Dimension)
	,meanVect(Eigen::VectorXd(Dimension))
	,VarCovMatrix(Eigen::MatrixXd(Dimension,Dimension))
{
	if(Dimension>0U){
		for(unsigned int i=0;i<Dim;i++){
			meanVect(i)=0.0;
			for(unsigned int j=0;j<Dim;j++){
				if(i==j) VarCovMatrix(i,j)=1.0;
				else VarCovMatrix(i,j)=0.0;
			}
		}
	}
	CheckValidity();
	CurrentSeed=static_cast<unsigned int>(std::time(NULL));
	RandNumGen.seed(CurrentSeed);
}
bool mvNormSampler::CheckValidity(){
	AllValid=true;
	if(Dim<1U){ //The dimension must be at least one
		AllValid=false;
		return AllValid;
	}
	if(meanVect.rows()!=Dim){ //The mean vector must be as many elements as there are dimensions
		meanVect.transposeInPlace();
		if(meanVect.rows()!=Dim){
			AllValid=false;
			return AllValid;
		}
	}
	if(VarCovMatrix.rows()!=VarCovMatrix.cols() || VarCovMatrix.rows()!=Dim){ //The Var-Cov Matrix must be squared and have as many rows as there are dimensions
		AllValid=false;
		return AllValid;
	}
	if(VarCovMatrix!=VarCovMatrix.transpose()){ //The Var-Cov Matrix must be symmetric
		AllValid=false;
		return AllValid;
	}
	//The Variance Covariance matrix must be positive definite
	if(VarCovMatrix.determinant()<=0.0){
		AllValid=false;
		return AllValid;
	}
	Eigen::VectorXcd RelatedEigen=VarCovMatrix.eigenvalues();
	for (unsigned int i=0;i<Dim;i++){
		if(RelatedEigen(i).real()<0.0){ 
			AllValid=false;
			return AllValid;
		}
	}
	return AllValid;
}
void mvNormSampler::SetRandomSeed(unsigned int NewSeed){
	CurrentSeed=NewSeed;
	RandNumGen.seed(NewSeed);
}
bool mvNormSampler::SetMeanVector(const Eigen::VectorXd& mVect){
	if(mVect.rows()!=Dim) //The mean vector must be as many elements as there are dimensions
			return false;
	meanVect=mVect;
	CheckValidity();
	return true;
}
bool mvNormSampler::SetMeanVector(const std::vector<double>& mVect){
	if(mVect.size()!=Dim) return false;
	for(unsigned int i=0;i<Dim;i++){
		meanVect(i)=mVect.at(i);
	}
	CheckValidity();
	return true;
}
bool mvNormSampler::SetDimension(unsigned int Dimension){
	if(Dimension==0U) return false;
	Dim=Dimension;
	for(unsigned int i=0;i<Dim;i++){
		meanVect(i)=0.0;
		for(unsigned int j=0;j<Dim;j++){
			if(i==j) VarCovMatrix(i,j)=1.0;
			else VarCovMatrix(i,j)=0.0;
		}
	}
	CheckValidity();
	return true;
}
bool mvNormSampler::SetVarCovMatrix(const Eigen::MatrixXd& CovMatr){
	if(CovMatr.rows()!=CovMatr.cols() || CovMatr.rows()!=Dim) //The Var-Cov Matrix must be squared and have as many rows as there are dimensions
		return false;
	if(CovMatr!=CovMatr.transpose()) //The Var-Cov Matrix must be symmetric
		return false;
	//The Variance Covariance matrix must be positive definite
	if(CovMatr.determinant()<0.0)
		return false;

	Eigen::VectorXcd RelatedEigen=CovMatr.eigenvalues();
	for (unsigned int i=0;i<Dim;i++){
		if(RelatedEigen(i).real()<0.0){ 
			return false;
		}
	}
	VarCovMatrix=CovMatr;
	CheckValidity();
	return true;
}
bool mvNormSampler::SetVarCovMatrix(const std::vector<double>& mVect, bool RowWise){
	if(mVect.size()!=Dim*Dim) return false;
	Eigen::MatrixXd TempMatrix(Dim,Dim);
	for(unsigned int i=0;i<mVect.size();i++){
		TempMatrix(i/Dim,i%Dim)=mVect.at(i);
	}
	if(!RowWise) TempMatrix.transposeInPlace();
	return SetVarCovMatrix(TempMatrix);
}
bool mvNormSampler::SetVarCovMatrix(const Eigen::MatrixXd& CorrelationMatrix,const Eigen::VectorXd& Variances){
	if(CorrelationMatrix.rows()!=CorrelationMatrix.cols() || CorrelationMatrix.rows()!=Dim) return false;
	if(!CorrelationMatrix.diagonal().isOnes()) return false;
	if(Variances.rows()!=Dim) return false;
	Eigen::MatrixXd TestVariance(Dim,Dim);
	for(unsigned int i=0;i<Dim;i++){
		if(Variances(i)<0.0) return false;
		TestVariance(i,i)=Variances(i);
		for(unsigned int j=0;j<Dim;j++){
			if(i==j) continue;
			if(CorrelationMatrix(i,j)>1.0 || CorrelationMatrix(i,j)<-1.0) return false;
			else TestVariance(i,j)=CorrelationMatrix(i,j)*sqrt(abs(Variances(i)*Variances(j)));
		}
	}
	return SetVarCovMatrix(TestVariance);
}
Eigen::MatrixXd mvNormSampler::GetCorrelationMatrix() const{
	if(!AllValid) return Eigen::MatrixXd();
	Eigen::MatrixXd Result(Dim,Dim);
	for(unsigned int i=0;i<Dim;i++){
		for(unsigned int j=0;j<Dim;j++){
			if(i==j) Result(i,j)=1.0;
			else Result(i,j)=VarCovMatrix(i,j)/(sqrt(VarCovMatrix(i,i)*VarCovMatrix(j,j)));
		}
	}
	return Result;
}
std::vector<double> mvNormSampler::ExtractSampleVector() const{
	std::vector<double> Result(Dim);
	if(!AllValid) return Result;
	Eigen::RowVectorXd TempVector=ExtractSample();
	for(unsigned int i=0;i<Dim;i++){
		Result[i]=TempVector(i);
	}
	return Result;
}
std::map<unsigned int,std::vector<double> > mvNormSampler::ExtractSamplesMap(unsigned int NumSamples) const{
	if(!AllValid || NumSamples==0) return std::map<unsigned int,std::vector<double> >();
	std::map<unsigned int,std::vector<double> > Result;
	std::vector<double> Series(NumSamples);
	Eigen::MatrixXd TempMatrix=ExtractSamples(NumSamples);
	for(unsigned int i=0;i<Dim;i++){
		for(unsigned int j=0;j<NumSamples;j++){
			Series[j]=TempMatrix(j,i);
		}
		Result.insert(std::pair<unsigned int,std::vector<double> >(i,Series));
	}
	return Result;
}
double mvNormSampler::GetDensity(const std::vector<double>& Coordinates, bool GetLogDensity)const{
	if(Coordinates.size()!=Dim) return 0.0;
	Eigen::VectorXd TempVector(Dim);
	for(unsigned int i=0;i<Dim;i++){
		TempVector(i)=Coordinates.at(i);
	}
	return GetDensity(TempVector,GetLogDensity);
}
#ifdef mvNormSamplerUnsafeMethods
void mvNormSampler::SetMeanVector(double* mVect){
	for(unsigned int i=0;i<Dim;i++){
		meanVect(i)=mVect[i];
	}
}
bool mvNormSampler::SetVarCovMatrix(double** mVect){
	Eigen::MatrixXd TempMatrix(Dim,Dim);
	for(unsigned int i=0;i<Dim;i++){
		for(unsigned int j=0;j<Dim;j++){
			TempMatrix(i,j)=mVect[i][j];
		}
	}
	return SetVarCovMatrix(TempMatrix);
}
double* mvNormSampler::ExtractSampleArray()const{
	if(!AllValid) return NULL;
	Eigen::RowVectorXd TempVector=ExtractSample();
	double* Result=new double[Dim];
	for(unsigned int i=0;i<Dim;i++){
		Result[i]=TempVector(i);
	}
	return Result;
}
double** mvNormSampler::ExtractSamplesMatix(unsigned int NumSamples) const{
	if(NumSamples==0 || !AllValid) return NULL;
	Eigen::MatrixXd TempMatrix=ExtractSamples(NumSamples);
	double** Result=new double*[NumSamples];
	for(unsigned int i=0;i<NumSamples;i++){
		Result[i]=new double[Dim];
		for(unsigned int j=0;j<Dim;j++){
			Result[i][j]=TempMatrix(i,j);
		}
	}
	return Result;
}
#endif
#endif