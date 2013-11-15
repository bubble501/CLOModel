#ifndef mvNormSampler_h__
#define mvNormSampler_h__
#include <Eigen/Dense>
#include <vector>
#include <map>
#include <boost/random/mersenne_twister.hpp>
//! Multivariate Normal Distribution
/*!
\details This class provides the functionality of calculating the probability density value and generate random samples from a multivariate normal.

It uses the [Eigen](http://eigen.tuxfamily.org) libraries for linear algebra computation and [Boost](http://www.boost.org/) libraries for statistical distribution and random number generation.

The analytical process is based upon the [mvtnorm package](http://cran.r-project.org/web/packages/mvtnorm/index.html) for [R](http://www.r-project.org/) by Alan Genz, Frank Bretz, Tetsuhisa Miwa, Xuefei Mi, Friedrich Leisch, Fabian Scheipl, Bjoern Bornkamp, Torsten Hothorn

To generate samples a [boost::random::mt19937](http://www.boost.org/doc/libs/1_55_0/doc/html/boost/random/mt19937.html) random number generator is used and seeded with [std::time(NULL)](http://www.cplusplus.com/reference/ctime/time/).<br>
If you construct multiple instances of this class, to avoid the generated samples to be the same you should supply a different seed. To do so, for example, you can call `MyDistribution.SetRandomSeed(MyDistribution.GetCurrentSeed()+1U);`

Example Usage:<br>
Create a bivariate normal with mean \f$ \begin{bmatrix}
1 \\
0
\end{bmatrix}
\f$ and variance-covariance matrix \f$ \begin{bmatrix}
3 & 1 \\
1 & 3
\end{bmatrix}
\f$:

\code
std::vector<double> MeanVector;
MeanVector.push_back(1.0);
MeanVector.push_back(0.0);
std::vector<double> VarMatrix(4);
VarMatrix[0]=3;
VarMatrix[1]=1;
VarMatrix[2]=1;
VarMatrix[3]=3;
mvNormSampler BivNorm(2); //A Bivariate standard normal 
BivNorm.SetMeanVector(MeanVector); //Set the Mean Vector
BivNorm.SetVarCovMatrix(VarMatrix); //Set the Var-Cov Matrix
\endcode

Calculate the density of the distribution corresponding to the values \f$ \begin{bmatrix} 
0.5 \\
-0.3
\end{bmatrix}
\f$ and print it to the console:

\code
std::vector<double> Values;
Values.push_back(0.5);
Values.push_back(-0.3);
double densityValue=BivNorm.GetDensity(Values);
cout << endl << "Desity for [0.5;-0.3]: " << densityValue;
\endcode

Generate 1000 random samples from this distribution and print them to the console

\code
std::map<unsigned int,std::vector<double> > Samples=BivNorm.ExtractSamples(1000);
cout << "Var 1\tVar2";
for(int j=0;j<1000;j++){
	cout << endl;
	for(unsigned int i=0;i<BivNorm.GetDimension();i++){
		cout << Samples.at(i).at(j) << '\t'
	}
}
\endcode

\remark This class is re-entrant
\author Luca Beldi
\date November 2013
\license This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see [gnu.org](http://www.gnu.org/licenses/).
*/

class mvNormSampler{
private:
	mutable boost::random::mt19937 RandNumGen;
	unsigned int Dim;
	Eigen::VectorXd meanVect;
	Eigen::MatrixXd VarCovMatrix;
	bool AllValid;
	bool CheckValidity();
	mvNormSampler(const mvNormSampler& a);
	mvNormSampler& operator=(const mvNormSampler& a);
	unsigned int CurrentSeed;
public:
	//! Construct a multivariate normal with the given parameters
	/*!
	\param Dimension The dimensionality of the multivariate normal (supports also univariate gaussian distributions in case this is 1)
	\param mVect The column vector of expected values
	\param CovMatr The variance-covariance
	\details Construct a multivariate normal distribution.
	
	In case:
	- The Dimension is 0
	- The mean vector has a number of elements different from the Dimension
	- The variance-covariance is not square
	- The variance-covariance is not symmetric
	- The variance-covariance is not semi-positive definite
	- The variance-covariance has a number of rows different from the Dimension
	
	The class will be considered invalid (it can be checked using `IsValid()`) and won't produce any result until the problem is fixed.
	*/
	mvNormSampler(unsigned int Dimension,const Eigen::VectorXd& mVect,const Eigen::MatrixXd& CovMatr);
	//! Construct a multivariate standard normal
	/*!
	\param Dimension The dimensionality of the multivariate normal (supports also univariate Gaussian distributions in case this is 1)
	\details Construct a multivariate normal distribution with all expected values set to 0, unitary variances and null covariates.
	
	In case The Dimension is 0 the class will be considered invalid (it can be checked using `IsValid()`) and won't produce any result until the problem is fixed.

	If dimension is unspecified, a univariate standard normal is constructed
	*/
	mvNormSampler(unsigned int Dimension=1U);
	//! Check if the distribution is valid
	/*!
	\return A boolean determining if the structure of the distribution is valid
	\details If this function returns false the structure of the distribution is meaningless and no result will be produced until the invalid parameters are cured.
	*/
	bool IsValid() const {return AllValid;}
	//! Set the random number generator seed
	/*!
	\param NewSeed the new random seed
	\note This seed is different from the srand() seed and, even with the same seed, random number generated by the internal generator will be different by those generated by rand()
	\sa GetCurrentSeed()
	*/
	void SetRandomSeed(unsigned int NewSeed);
	//! Get the random number generator seed
	/*!
	\return The random number generator seed.
	\details This function return the seed that was used to initialize the random number generator.
	\note This seed is different from the srand() seed and, even with the same seed, random number generated by the internal generator will be different by those generated by rand()
	\sa SetRandomSeed()
	*/
	unsigned int GetCurrentSeed()const{return CurrentSeed;}
	//! Set the expected values vector
	/*!
	\param mVect the vector of new values for the expected values vector
	\return A boolean determining if the expected values vector was changed successfully
	\details This function attempts to set the expected values vector to the new values supplied.
	
	If the dimension of the vector is different from the dimension of the distribution the mean vector is not changed and false is returned
	\sa GetMeanVector()
	*/
	bool SetMeanVector(const Eigen::VectorXd& mVect);
	//! Set the expected values vector
	/*!
	\details This is an overloaded version of SetMeanVector(const Eigen::VectorXd&)
	*/
	bool SetMeanVector(const std::vector<double>& mVect);
	//! Set the dimensionality of the distribution
	/*!
	\param Dimension the new dimensionality of the distribution 
	\return A boolean determining if the dimensionality was changed successfully
	\details This function will try to change the dimensionality of the distribution (e.g. 2 for bivariate, 3 for trivariate, etc.)
	
	All the components of the expected value vector will be reset to 0 and the variance covariance matrix will default to an identity matrix

	If the argument passed is 0 the dimensionality will not be changed and the function will return false

	\sa GetDimension()
	*/
	bool SetDimension(unsigned int Dimension);
	//! Set the Var-Cov matrix of the distribution
	/*!
	\param CovMatr the new variance covariance matrix of the distribution 
	\return A boolean determining if the variance covariance matrix of the distribution was changed successfully
	\details This function tries to set the variance covariance matrix of the distribution to the new one.

	In case:
	- The variance-covariance is not square
	- The variance-covariance is not symmetric
	- The variance-covariance is not semi-positive definite
	- The variance-covariance has a number of rows different from the Dimension

	The variance covariance matrix of the distribution will not be changed and this function will return false

	\sa GetVarCovMatrix()
	*/
	bool SetVarCovMatrix(const Eigen::MatrixXd& CovMatr);
	//! Set the Var-Cov matrix of the distribution
	/*!
	\param mVect a vector containing the elements of the new variance covariance matrix of the distribution
	\param RowWise if it's set to true (the default) the matrix will be filled by row. If it's false it will be filled by columns
	\return A boolean determining if the variance covariance matrix of the distribution was changed successfully
	\details This function tries to set the variance covariance matrix of the distribution to the new one.

	Constructs square a matrix with number of rows equal to the dimensionality of the distribution, it is then filled with the values supplied in order according to the RowWise parameter

	In case:
	- The vector size is different from the square of the distribution dimensionality
	- The variance-covariance is not symmetric
	- The variance-covariance is not semi-positive definite
	- The variance-covariance has a number of rows different from the Dimension

	The variance covariance matrix of the distribution will not be changed and this function will return false

	\sa GetVarCovMatrix()
	*/
	bool SetVarCovMatrix(const std::vector<double>& mVect, bool RowWise=true);
	//! Set the Var-Cov matrix of the distribution
	/*!
	\param CorrelationMatrix The correlation coefficients matrix
	\param Variances the vector of variances
	\return A boolean determining if the variance covariance matrix of the distribution was changed successfully
	\details This function tries to set the variance covariance matrix of the distribution according to the correlation matrix supplied.

	Constructs the variance covariance matrix of the distribution using Variances as diagonal elements and covariates corresponding to the linear correlation coefficient supplied

	In case:
	- The size of the vector of variances is different from the distribution dimensionality
	- The correlation matrix is not symmetric
	- The correlation matrix has elements different from 1 on the diagonal
	- The correlation matrix has a number of rows different from the Dimension
	- The correlation matrix is not squared
	- The correlation matrix has off-diagonal elements greater than 1 in absolute value

	The variance covariance matrix of the distribution will not be changed and this function will return false

	\sa GetVarCovMatrix()
	*/
	bool SetVarCovMatrix(const Eigen::MatrixXd& CorrelationMatrix, const Eigen::VectorXd& Variances);
	//! Get the dimensionality of the distribution
	/*!
	\return The current dimensionality of the distribution
	\sa SetDimension()
	*/
	unsigned int GetDimension() const {return Dim;}
	//! Get the expected values vector of the distribution
	/*!
	\return The current expected values vector of the distribution
	\sa SetMeanVector(const Eigen::VectorXd&)
	*/
	const Eigen::VectorXd& GetMeanVector() const {return meanVect;}
	//! Get the Var-Cov matrix of the distribution
	/*!
	\return The current variance-covariance matrix of the distribution
	\sa SetVarCovMatrix(const Eigen::MatrixXd&)
	\sa SetVarCovMatrix(const std::vector<double>&,bool)
	\sa SetVarCovMatrix(const Eigen::MatrixXd&, const Eigen::VectorXd&)
	*/
	const Eigen::MatrixXd& GetVarCovMatrix() const {return VarCovMatrix;}
	//! Get the linear correlation matrix
	/*!
	\return The linear correlation matrix associated with the current variance-covariance matrix of the distribution
	\sa GetVarCovMatrix()
	*/
	Eigen::MatrixXd GetCorrelationMatrix() const;
	//! Generates a single simulation from the distribution
	/*!
	\return A vector with number of elements equal to the dimensionality of the distribution representing a single extraction from the distribution
	\details This is equal to calling `ExtractSamples(1U)`
	\sa ExtractSamples()
	*/
	Eigen::RowVectorXd ExtractSample() const{return ExtractSamples(1U);}
	//! Generates a single simulation from the distribution
	/*!
	\return A vector with number of elements equal to the dimensionality of the distribution representing a single extraction from the distribution
	\details This is equivalent to ExtractSample() but returns an std::vector intead of an Eigen::RowVectorXd
	\sa ExtractSamples()
	*/
	std::vector<double> ExtractSampleVector() const;
	//! Generates a multiple simulation from the distribution
	/*!
	\param NumSamples The number of simulation to run
	\return A matrix with columns equal to the dimensionality of the distribution and rows equal to the number of simulations
	\details This function generates NumSamples simulation from the current distribution and returns them in matrix form.
	
	If NumSamples is 0 or the distribution is invalid, a null matrix is returned
	*/
	Eigen::MatrixXd ExtractSamples(unsigned int NumSamples) const;
	//! Generates a multiple simulation from the distribution
	/*!
	\param NumSamples The number of simulation to run
	\return A map that has as keys the index of the dimension (starting from 0) and as values a vector containing the simulation results for that dimension
	\details This function generates NumSamples simulation from the current distribution and returns them in a map form.
	
	If NumSamples is 0 or the distribution is invalid, an empty map is returned
	*/
	std::map<unsigned int,std::vector<double> > ExtractSamplesMap(unsigned int NumSamples) const;
	//! Computes the probability density function of the distribution in correspondence of the supplied coordinates
	/*!
	\param Coordinates A vector containing the coordinates of the point for which the pdf should be computed
	\param GetLogDensity If set to true the log density is returned instead of the actual density
	\return The value of the probability density function
	\details This function computes the probability density function of the current distribution associated with the given coordinates.
	
	If the number of elements in Coordinates is different from the dimensionality of the distribution or the distribution is invalid, 0 is returned
	*/
	double GetDensity(const Eigen::VectorXd& Coordinates, bool GetLogDensity=false)const;
	//! Computes the probability density function of the distribution in correspondence of the supplied coordinates
	/*!
	\param Coordinates  A vector containing the coordinates of the point for which the pdf should be computed
	\param GetLogDensity If set to true the log density is returned instead of the actual density
	\return The value of the probability density function
	\details This is an overloaded version of  GetDensity(const Eigen::VectorXd&, bool)
	*/
	double GetDensity(const std::vector<double>& Coordinates, bool GetLogDensity=false)const;
//#ifdef mvNormSamplerUnsafeMethods
	/** \name Unsafe Methods
	The methods in this group use unsafe memory access or return arrays allocated on the heap that must be manually deleted.

	These functions are normally not compiled for safety reasons. To use them, the mvNormSamplerUnsafeMethods symbol must be defined at compile time
	\{
	*/
	/**
	\brief Set the expected values vector
	\param mVect an array containing the new values for the expected values vector
	\details This is an overloaded version of SetMeanVector(const Eigen::VectorXd&)
	\warning This function will search for a number of elements equal to the dimensionality of the distribution in the array. This may mean accessing unallocated memory blocks if the supplied array is not big enough
	 */
	void SetMeanVector(double* mVect);
	/**
	\brief Set the Var-Cov matrix of the distribution
	\param mVect a matrix containing the new values for the variance covariance matrix of the distribution
	\details This is an overloaded version of SetVarCovMatrix(const Eigen::MatrixXd&)
	\warning This function will search for a number of elements, both in the row and the column dimension, equal to the dimensionality of the distribution in the matrix. This may mean accessing unallocated memory blocks if the supplied matrix is not big enough
	 */
	bool SetVarCovMatrix(double** mVect);
	/**
	\brief Generates a single simulation from the distribution
	\return A dynamically allocated array with number of elements equal to the dimensionality of the distribution representing a single extraction from the distribution
	\details This is equivalent to ExtractSample() but returns an array intead of an Eigen::RowVectorXd
	\warning This function will return an array allocated on the heap. If the user doesn't take care of deleting it, this will lead to memory leaks
	*/
	double* ExtractSampleArray() const;
	/**
	\brief Generates a single simulation from the distribution
	\param NumSamples The number of simulation to run
	\return A dynamically allocated matrix with number of columns equal to the dimensionality of the distribution and number of rows equal to the number of simulations representing multiple draws from the distribution
	\details This is equivalent to ExtractSamples() but returns a matrix intead of an Eigen::RowVectorXd
	\warning This function will return a matrix allocated on the heap. If the user doesn't take care of deleting it, this will lead to memory leaks
	*/
	double** ExtractSamplesMatix(unsigned int NumSamples) const;
	/**
	\brief Computes the probability density function of the distribution in correspondence of the supplied coordinates
	\param Coordinates An array containing the coordinates of the point for which the pdf should be computed
	\param GetLogDensity If set to true the log density is returned instead of the actual density
	\return The value of the probability density function
	\details This is an overloaded version of GetDensity(const Eigen::VectorXd&, bool)
	\warning This function will search for a number of elements equal to the dimensionality of the distribution in the array. This may mean accessing unallocated memory blocks if the supplied array is not big enough
	*/
	double GetDensity(double* Coordinates, bool GetLogDensity=false)const;
	/// \}
//#endif
};
#endif // mvNormSampler_h__