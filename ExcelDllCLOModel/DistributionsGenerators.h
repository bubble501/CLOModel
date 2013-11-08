//#define ProbabilisticModel
#ifdef ProbabilisticModel
#ifndef DistributionsGenerators_h__
#define DistributionsGenerators_h__
#include "CommonFunctions.h"
#include "EigenMultivarNormal.h"
#include <QString>
#include <QList>
#include <QHash>
#define Dimensionality 3
//!Bloomberg Vector Statistical Transformations
/*!
This class can generate Bloomberg-like vectors for CPR, CDR and loss severity series from a model of the type:

\f$Y=X+e\f$

Where \f$X\f$ is a Bloomberg vector for the process and \f$e\f$ is an error term.

The Bloomberg vector is considered as the location parameter for the marginal distribution of the errors.

This means that for uniform, Normal and t distributions the vector is the mean while for the Chi Square distribution the vector represent the "best case scenario".

By default the errors are distributed as a standard normal and are independent both in time and between different variables (independent Gaussian white noises).
*/
class DistributionsGenerator {
public:
	//! Enum defining exceptions that can be trown by the program
	enum DistrExceptions{
		NotSimmetricVar, /*!< The Variance-Covariance matrix supplyed is non symmetric*/
	};
	//! Enum defining the margins of the distribution of the error terms
	enum DistrFamily{
		Normal, /*!< Normal Gaussian Distribution. */
		tStudent, /*!< Student-t Distribution. */
		ChiSqauare, /*!< Chi Sqaure Distribution. */
		Uniform /*!< Uniform Distribution. */
	};
	//! Enum defining the margins of the distribution of the error terms
	enum CopulaType{
		IndependenceCopula, /*!< The variables are independents from one another */
		GaussianCopula, /*!< The variables are have linear correlation. */
		tCopula, /*!< The variables have tail dependence. */
		ClaytonCopula, /*!< Greater Dependence of negative values. */
		GumbelCopula /*!< Greater Dependence of positive values. */
	};
	//! Enum Defining the type of vector
	enum VectorType{
		CPR, /*!< Constant Prepayment Rate Vector. */
		CDR, /*!< Constant Default Rate Vector. */
		LS /*!< Loss Severity Vector. */
	};
	//! Default Constructor
	DistributionsGenerator();
	//! Calculates and returns the simulation results
	/*!
	\return A table containing the simulated values for the three variables
	\sa GetResultVector()
	
	Simulates a path for the errors according to the structure and returns it's values.

	The result is then stored and available for quick retrieval if the structure doesn't change.
	*/
	const QHash<VectorType,qreal>& GetResultValues() const;
	//! Calculates and returns the simulation results as a Bloomberg Vector
	/*!
	\return A Table containing a Bloomberg vector representing the simulated values for the three variables
	\sa GetResultVector()
	
	Simulates a path for the errors according to the structure and returns their Bloomberg vector representation.

	The result is then stored and available for quick retrieval if the structure doesn't change.
	*/
	QHash<VectorType,QString> GetResultVector()const;
	//! Returns the Bloomberg Vector representing the VecType variable
	const QString& GetBBGVector(VectorType VecType)const{return BBGVector[VecType];}
	//! Autoregressive structure of the error
	/*!
	\param VecType the variable whose error the structure is applied to.
	\return A list containing the coefficients of the AR polynomial
	\sa SetARMAstructure()
	*/
	const QList<qreal>& GetARcoeff(VectorType VecType)const{return ARcoeff[VecType];}
	//! Moving Average structure of the error
	/*!
	\param VecType the variable whose error the structure is applied to.
	\return A list containing the coefficients of the MA polynomial
	\sa SetARMAstructure()
	*/
	//! Moving Average structure of the error
	const QList<qreal>& GetMAcoeff(VectorType VecType)const{return MAcoeff[VecType];}
	//! Set the ARMA structure for the error
	/*!
	\param VecType the variable whose error the structure is applied to.
	\param AR A list containing the coefficients of the AR polynomial
	\param MA A list containing the coefficients of the MA polynomial
	\param Drift The drift of the process.
	\param TimePar The time coefficients for the process.
	
	This function defines an ARMA structure for the error term of the VecType variable.

	The error term will take the form:

	\f$e_t=\textrm{Drift} + t*\textrm{TimePar}+AR[0] e_{t-1}+AR[1] e_{t-2}+...+AR[n] e_{t-n-1}+v_t+MA[0] v_{t-1}+MA[1] v_{t-2}+...+MA[m] v_{t-m-1}\f$
	
	The process is not required to be stationary

	For terms with \f$t-i<=0\f$ zero is used as a value for \f$e_{t-i}\f$ and \f$v_{t-i}\f$
	*/
	void SetARMAstructure(VectorType VecType,QList<qreal> AR,QList<qreal> MA,qreal Drift=0.0,qreal TimePar=0.0);
	//! Sets the Bloomberg Vector representing the VecType variable to NewVect
	void SetBBGVector(VectorType VecType, const QString& NewVect);
	//! Force the recalculation of the paths the next time GetResultVector() or GetResultValues() is called
	void ForceRecalc(){NeedRecalculation=true;}
	//! Set or reset the random number generator seed
	void SetRandSeed(int a);
	//! Set the length of the resulting path
	void SetPeriodsLegth(quint32 Len){PeriodsLegth=Len; NeedRecalculation=true;}
	//! Set the variance covariance matrix
	/*!
	\param a A symmetric matrix.
	\sa GetCopulaVarianceCovarianceMatrix()
	
	This matrix is used differently depending on the type of copula:
	+ Gaussian: This is the variance covariance matrix of the multivariate normal describing the copula
	+ t: The scale matrix of the multivariate t describing the copula
	+ Clayton: This matrix is ignored
	+ Gumbel: This matrix is ignored
	+ Independence: This matrix is ignored
	
	The parameter is initialized as an identity matrix
	*/
	void SetCopulaVarianceCovarianceMatrix(const Eigen::Matrix<double,Dimensionality,Dimensionality>& a);
	//! Set the variance covariance matrix
	/*!
	\param a A symmetric matrix.
	\sa GetCopulaVarianceCovarianceMatrix()
	
	This is an overloaded function.
	*/
	void SetCopulaVarianceCovarianceMatrix(double a[Dimensionality][Dimensionality]);
	//! Get the Variance Covariance Matrix
	/*!
	\sa SetCopulaVarianceCovarianceMatrix()
	*/
	const Eigen::Matrix<double,Dimensionality,Dimensionality>& GetCopulaVarianceCovarianceMatrix() const{return CopulaVarianceCovarianceMatrix;}
	//! Get the Scalar Parameter
	/*!
	\sa SetCopulaParameter()
	*/
	qreal GetCopulaParameter() const{return CopulaParameter;}
	//! Set the Scalar Parameter
	/*!
	\param a a scalar parameter for the copula
	\sa GetCopulaParameter()
	
	This parameter is used differently depending on the type of copula:
	+ Gaussian: This parameter is ignored
	+ t: This parameter represents the number of degrees of freedom. It is be cast to int so decimals are truncated
	+ Clayton: This is the parameter indexing the whole copula
	+ Gumbel: This is the parameter indexing the whole copula
	+ Independence: This parameter is ignored
	
	The parameter is initialized to 10.0
	*/
	void SetCopulaParameter(qreal a){CopulaParameter=a;}
	//! Returns the Standard Deviation associated to the VecType variable distribution
	qreal GetMarginStdDev(VectorType VecType)const{return MarginStdDev[VecType];}
	//! Set the Standard Deviation associated to the VecType variable distribution
	/*!
	\param VecType The variable whose margin standard deviation you are setting
	\param StdDev The new standard deviation
	\sa GetMarginStdDev()
	
	This parameter is used differently depending on the type of copula:
	+ Gaussian: This parameter is ignored
	+ t: This parameter represents the number of degrees of freedom. It is be cast to int so decimals are truncated
	+ Clayton: This is the parameter indexing the whole copula
	+ Gumbel: This is the parameter indexing the whole copula
	+ Independence: This parameter is ignored
	
	The parameter is initialized to 10.0
	*/
	void SetMarginStdDev(VectorType VecType, qreal StdDev){MarginStdDev[VecType]=StdDev;}
private:
	mutable bool NeedRecalculation;
	QString BBGVector[Dimensionality];
	QList<qreal> ARcoeff[Dimensionality];
	QList<qreal> MAcoeff[Dimensionality];
	qreal Drifts[Dimensionality];
	qreal TimePars[Dimensionality];
	DistrFamily MarginsDistribution[Dimensionality];
	CopulaType Copula;
	quint32 PeriodsLegth;
	Eigen::Matrix<double,Dimensionality,Dimensionality> CopulaVarianceCovarianceMatrix;
	qreal MarginStdDev[Dimensionality];
	mutable QHash<VectorType,qreal> Result;
	void CalculateResult() const;
	qreal CopulaParameter;
};
#endif // DistributionsGenerators_h__
#endif