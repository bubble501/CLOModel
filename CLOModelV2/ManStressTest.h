#ifndef ManStressTest_h__
#define ManStressTest_h__
#include "StressTestOverlay.h"
#include "ManagedCommons.h"
#include "ManWaterfall.h"
#include <QHash>
using namespace System::Collections::Generic;
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Delegate for the ManStressTest::AllFinishEvent
	*/
	public delegate void AllFinishDelegate();
	/*!
	\brief Delegate for the ManStressTest::ProgressStatusEvent
	*/
	public delegate void ProgressStatusDelegate(double);
	/*!
	\brief Class for managing bi-dimensional stress test for tri-dimensional model.
	\details This class allows to stress two of the three assumption on the pool of loans: CPR, CDR and loss severity.<br/>The parameters being stressed lay on two dimensions, called X and Y while the third parameter remains constant 
	\note If the waterfall structure has ManWaterfall::UseCall set to true the stress will consider the call scenario.
	*/
	public ref class ManStressTest
	{
	public:
		//! Enum defining what parameter is currently being stressed
		enum class ManStressVariability : int{
			ChangingCDR, /*!< Constant Default Rate*/
			ChangingLS, /*!< Loss Severity*/
			ChangingCPR /*!< Constant Prepayment Rate*/
		};
		/*!
		\brief Display the progress window
		\details This property determines whether or not the progress window is displayed.<br/>By default it's set to true.
		\note Currently the progress will be displayed despite this property value, it will become operational after an update on the base model
		*/
		property bool ShowProgress{
			bool get(){return Unmanaged->GetShowProgress();}
			void set(bool a){Unmanaged->SetShowProgress(a);}
		}
		/*!
		\brief Stress values for the X dimension
		\details This property contains a list of the stress scenarios applied on the X dimension (the rows in the widget)
		*/
		property List<String^>^ XSpann{
			List<String^>^ get(){return QStringList2ListString(Unmanaged->GetXSpann());}
			void set(List<String^>^ a){Unmanaged->SetXSpann(ListString2QStringList(a));}
		}
		/*!
		\brief Add stress scenarios for the X dimension
		\arg a A list of Bloomberg vectors representing stress scenarios to add
		\details Add stress scenarios for the parameter in the X dimension (the rows in the widget)
		*/
		void AddXSpann(List<String^>^ a){Unmanaged->AddXSpann(ListString2QStringList(a));}
		/*!
		\brief Add a stress scenario for the X dimension
		\arg a A Bloomberg vector representing a stress scenario to add
		\details Add a stress scenario for the parameter in the X dimension (the rows in the widget)
		*/
		void AddXSpann(String^ a){Unmanaged->AddXSpann(String2QString(a));}
		/*!
		\brief Remove all stress scenarios for the X dimension
		\details Removes all stress scenarios for the parameter in the X dimension (the rows in the widget)
		*/
		void ResetXSpann(){Unmanaged->ResetXSpann();}
		/*!
		\brief Stress values for the Y dimension
		\details This property contains a list of the stress scenarios applied on the Y dimension (the columns in the widget)
		*/
		property List<String^>^ YSpann{
			List<String^>^ get(){return QStringList2ListString(Unmanaged->GetYSpann());}
			void set(List<String^>^ a){Unmanaged->SetYSpann(ListString2QStringList(a));}
		}
		/*!
		\brief Add a stress scenario for the Y dimension
		\arg a A list of Bloomberg vectors representing stress scenarios to add
		\details Add a stress scenario for the parameter in the Y dimension (the columns in the widget)
		*/
		void AddYSpann(List<String^>^ a){Unmanaged->AddYSpann(ListString2QStringList(a));}
		/*!
		\brief Add a stress scenario for the Y dimension
		\arg a A Bloomberg vector representing a stress scenario to add
		\details Add a stress scenario for the parameter in the Y dimension (the columns in the widget)
		*/
		void AddYSpann(String^ a){Unmanaged->AddYSpann(String2QString(a));}
		/*!
		\brief Remove all stress scenarios for the Y dimension
		\details Removes all stress scenarios for the parameter in the Y dimension (the columns in the widget)
		*/
		void ResetYSpann(){Unmanaged->ResetYSpann();}
		/*!
		\brief Remove all stress scenarios
		\details Removes all stress scenarios for all the dimensions
		\sa ResetYSpann
		\sa ResetXSpann
		*/
		void ResetStressLevels(){Unmanaged->ResetStressLevels();}
		/*!
		\brief Scenario for the parameter kept constant
		\details This property defines the Bloomberg vector representing the constant parameter for the stress
		*/
		property String^ ConstantPar{
			String^ get(){return QString2String(Unmanaged->GetConstantPar());}
			void set(String^ a){Unmanaged->SetConstantPar(String2QString(a));}
		}
		/*!
		\brief Number of loans in the pool
		*/
		property int LoansCount{
			int get(){return Unmanaged->GetLoans().size();}
		}
		/*!
		\brief Get a loan in the pool
		\arg index The index of the loan in the pool (first loan is at index 0).
		\details This function returns the loan at the specified index in the pool.<br/>If index is out of range null is returned.
		*/
		ManMortgage^ GetLoan(int index){
			const Mortgage* Temp=Unmanaged->GetLoans(index);
			if(Temp) return gcnew ManMortgage(*Temp);
			return nullptr;
		}
		/*!
		\brief Overwrites a loan in the pool
		\arg index The index of the loan to overwrite (the first loan has index 0).
		\arg source The loan that will overwrite the current one
		\details This function will overwrite the loan at the specified index in the pool.<br/>If the index is out of range the function will do nothing.
		*/
		void SetLoan(int index, ManMortgage^ source){
			Mortgage* Temp=Unmanaged->GetLoans(index);
			if(Temp) (*Temp)=*(source->Unmanaged);
		}
		/*!
		\brief The base Waterfall of the deal
		\details This property holds the waterfall structure of the deal.
		\warning If Structure::UseCall is set to true, the stress test will consider the call to be exercised
		*/
		property ManWaterfall^ Structure{
			ManWaterfall^ get(){return gcnew ManWaterfall(Unmanaged->GetStructure());}
			void set(ManWaterfall^ a){Unmanaged->SetStructure(*(a->Unmanaged));}
		}
		/*!
		\brief The loan pool cut off date
		*/
		property DateTime StartDate{
			DateTime get(){return QDate2DateTime(Unmanaged->GetStartDate());}
			void set(DateTime a){Unmanaged->SetStartDate(DateTime2QDate(a));}
		}
		/*!
		\brief The result of a stress scenario
		\arg Xvar the stress scenario for the X dimension (rows)
		\arg Yvar the stress scenario for the Y dimension (columns)
		\details This function returns the waterfall containing the results of the specified stress scenario.<br/>If RunStressTest was never called, ResetResults was called or the requested scenarios are not part of the stressed scenarios, null will be returned.
		*/
		ManWaterfall^ GetResults(String^ Xvar,String^ Yvar){
			QString Xstr=String2QString(Xvar);
			QString Ystr=String2QString(Yvar);
			if(!Unmanaged->GetResults().contains(Xstr)) return nullptr;
			if(!Unmanaged->GetResults().value(Xstr).contains(Ystr)) return nullptr;
			return gcnew ManWaterfall(Unmanaged->GetResults().value(Xstr).value(Ystr));
		}
		/*!
		\brief Determines the stressed parameter on the X dimension
		\details This property determines is being stressed in the X dimension (rows)<br/>By default it is set to ChangingCDR
		*/
		property ManStressVariability XVariability{
			ManStressVariability get(){return static_cast<ManStressVariability>(static_cast<int>(Unmanaged->GetXVariability()));}
			void set(ManStressVariability a){Unmanaged->SetXVariability(static_cast<StressTest::StressVariability>(static_cast<int>(a)));}
		}
		/*!
		\brief Determines the stressed parameter on the Y dimension
		\details This property determines is being stressed in the Y dimension (columns)<br/>By default it is set to ChangingLS
		*/
		property ManStressVariability YVariability{
			ManStressVariability get(){return static_cast<ManStressVariability>(static_cast<int>(Unmanaged->GetYVariability()));}
			void set(ManStressVariability a){Unmanaged->SetYVariability(static_cast<StressTest::StressVariability>(static_cast<int>(a)));}
		}
		/*!
		\brief Removes all results from previous stress calculations
		*/
		void ResetResults(){Unmanaged->ResetResult();}
		/*!
		\brief Adds a loan to the loan pool
		*/
		void AddLoan(ManMortgage^ a){Unmanaged->AddLoan(*(a->Unmanaged));}
		/*!
		\brief Removes all loans from the pool
		*/
		void ResetLoans(){Unmanaged->ResetLoans();}
		/*!
		\brief Determines if multi-threading calculation should be used
		\details This property determines if the model can leverage multiple CPU cores to speed up calculations.<br/>The number of threads will automatically adapt to the number of cores of the machine (both real and logical)<br/>For single core machines or if the number of cores can't be evaluated (e.g. some virtual machines) this property has no effect and all calculations will be sequential.<br/>By default it's set to true.
		\note If this property is set to true your CPU usage will reach 100% during the computation.
		*/
		property bool UseMultithread{
			bool get(){return Unmanaged->GetUseMultithread();}
			void set(bool a ){Unmanaged->UseMultithread(a);}
		}
		/*!
		\brief Determines if the simplified stress test should be used
		\details If this property is set to true, the stress test will be run using a simplified method:<br/>Instead of calculating the loans cash flows item by item for each stress level, the base scenario of 0 CPR, 0 CDR will be calculated and then stresses will be applied on the aggregated cash flows.<br/>While the results will be an approximation, especially with complicated loans structures (with different CPR/LS multipliers), this option would speed up the calculation substantially when the loan pool is too big for the normal algorithm.<br/>The accuracy of the test is not compromised in a substantial way. Tests on the AVOCA 11 provisional pool revealed a total error in cash flows of 0.0001 Euros<br/>The following image shows the calculation time for the two stress methods based on the AVOCA 11 structure for different number of loans in the pool.
		\image html StressPerformance.png "Performance comparison for the two methods"
		\image rtf StressPerformance.png "Performance comparison for the two methods"
		\image latex StressPerformance.eps "Performance comparison for the two methods" height=10cm
		*/
		property bool UseSimplifiedMethod{
			bool get() { return Unmanaged->GetUseFastVersion(); }
			void set(bool a) { Unmanaged->SetUseFastVersion(a); }
		}
		/*!
		\brief Saves the stress results
		\arg DestPath the path to the folder where the results will be saved
		\details This function saves the results of the stress in fcsr format for fast access.<br/>The fcsr format is a compressed file containing the different scenarios and a file containing the general information of the structure, this allows fast access to single scenarios of the stress test without having to load all of them in the memory.
		\warning Calling two instances of this function at the same time with the same DestPath will cause undefined behavior
		\sa LoadResultsFromFile
		*/
		void SaveResults(String^ DestPath){Unmanaged->SaveResults(String2QString(DestPath));}
		/*!
		\brief Loads the stress results
		\arg DestPath the path to the results file
		\details This function loads all the stress results from a a fcsr format file.
		\sa SaveResults
		*/
		bool LoadResultsFromFile(String^ DestPath){return Unmanaged->LoadResultsFromFile(String2QString(DestPath));}
		/*!
		\brief Loads a stress result
		\arg DestPath the path to the results file
		\arg XScenario The X dimension scenario to retrieve
		\arg YScenario The Y dimension scenario to retrieve
		\details This function retrieves the results of a specific scenario from a a fcsr format file.<br/>If the required scenarios are not available in the model, an empty waterfall is returned.
		\sa LoadResultsFromFile
		*/
		static ManWaterfall^ GetScenarioFromFile(String^ DestPath,String^ XScenario,String^ YScenario){
			return gcnew ManWaterfall(StressTestOverlay::GetScenarioFromFile(String2QString(DestPath),String2QString(XScenario),String2QString(YScenario)));
		}
		/*!
		\brief Runs the calculation of the stress test
		\details This function starts the calculation of the stress scenarios, the function will return almost immediately.<br/>ProgressStatusEvent will be emitted any time the computation progresses. When the calculation is completed AllFinishEvent is emitted. 
		*/
		void RunStressTest(){Unmanaged->RunStressTest();}

		ManStressTest(){Unmanaged=new StressTestOverlay(this);}
		ManStressTest::~ManStressTest()
		{
			this->!ManStressTest();
			GC::SuppressFinalize(this);
		}
		ManStressTest::!ManStressTest()
		{
			if (Unmanaged)
			{
				Unmanaged->deleteLater();
				Unmanaged = NULL;
			}
		}
		/*!
		\brief Event called when the calculation of all scenario is completed
		*/
		event AllFinishDelegate^ AllFinishEvent;
		/*!
		\brief Event called when the calculation of a scenario is completed.
		\details This event will be triggered every time a scenario gets calculated and the current progress (in percentage points) is passed as argument
		*/
		event ProgressStatusDelegate^ ProgressStatusEvent;
	internal:
		StressTestOverlay* Unmanaged;
		void ProgressStatusCallback(double prog){ProgressStatusEvent(prog);}
		void AllFinishCallback(){AllFinishEvent();}
	};
}
#endif // ManStressTest_h__

