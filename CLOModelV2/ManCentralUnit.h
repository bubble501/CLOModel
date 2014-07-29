#ifndef ManCentralUnit_h__
#define ManCentralUnit_h__
#include "CentralUnitOverlay.h"
#include "ManagedCommons.h"
#include "ManMortgage.h"
#include "ManWaterfall.h"

using namespace System;
namespace ManagedCLO {
	/*!
	\brief Delegate for the ManCentralUnit::ProgressUpdateEvent
	*/
	public delegate void ProgressUpdateDelegate(double);
	/*!
	\brief Delegate for the ManCentralUnit::CalculationFinishedEvent
	*/
	public delegate void CalculationFinishedDelegate();
	/*!
	\brief Class for managing loans and tranches cash flows
	\details This class provides a fast multi-threaded calculation of the cash flows for a loan pool and CLO tranches
	*/
	public ref class ManCentralUnit
	{
	public:
		
		/*!
		\brief The structure of the deal
		\details This property holds the waterfall that defines the structure of the deal.<br/>When the calculation is completed the structure will be populated with the cash flows
		\sa Result
		*/
		property ManWaterfall^ Structure{
			ManWaterfall^ get(){return gcnew ManWaterfall(Unmanaged->GetStructure());}
			void set(ManWaterfall^ a){Unmanaged->SetStructure(*(a->Unmanaged));}
		}
		/*!
		\brief Whether the model uses forward curves for the base rates or not
		*/
		property bool UseForwardCurve{
			bool get() { return Unmanaged->GetUseForwardCurve(); }
		}
		/*!
		\brief Uses the supplied base rates table to fill in the base rate values
		\arg Values A table containing the spot rates for the relevant base rates
		\details This function will use the values supplied in the table as the values for the relevant base index rates.<br/>If a base index is required by the model but not supplied it will be downloaded from Bloomberg and added to the Values table.
		\sa GetBaseRatesDatabase
		*/
		void CompileBaseRates(ManConstBaseRateTable^ Values) {
			Unmanaged->CompileBaseRates(Values);
		}
		/*!
		\brief Uses the supplied base rates table to fill in the base rate values
		\arg Values A table containing the forward curves for the relevant base rates
		\details This function will use the curves supplied in the table as the values for the relevant base index rates.<br/>If a base index is required by the model but not supplied its spot rate will be downloaded from Bloomberg, used as constant and added to the Values table.
		\sa GetBaseRatesDatabase
		*/
		void CompileBaseRates(ManForwBaseRateTable^ Values) {
			Unmanaged->CompileBaseRates(Values);
		}
#ifndef NO_DATABASE
		/*!
		\brief Downloads the necessary base rates from the database
		\arg Values A table of overrides for specific indexes.
		\arg DownloadAll If set to true it will download all rates from the database instead of just the needed ones. Normally set to false. If Values is used for more than one model using true may result in more efficiency
		\details This function will download the base rates values from the database.<br/>The Values table can be used to override specific base rates.<br/>If a base index is required by the model but not supplied it will be downloaded from Bloomberg.<br/>All downloaded values will be added to the Values table.
		\note If NO_DATABASE is defined at compile time this method will be unavailable
		\sa CompileBaseRates
		*/
		void GetBaseRatesDatabase(ManConstBaseRateTable^ Values, bool DownloadAll) {
			Unmanaged->GetBaseRatesDatabase(Values, DownloadAll);
		}
		/*!
		\brief Downloads the necessary base rates from the database
		\arg Values A table of overrides for specific indexes.
		\arg DownloadAll If set to true it will download all rates from the database instead of just the needed ones. Normally set to false. If Values is used for more than one model using true may result in more efficiency
		\details This function will download the base rates forward curves from the database.<br/>The Values table can be used to override specific base rates.<br/>If a base index is required by the model but not supplied its spot rate will be downloaded from Bloomberg and used as constant.<br/>All downloaded values will be added to the Values table.
		\note If NO_DATABASE is defined at compile time this method will be unavailable
		\sa CompileBaseRates
		*/
		void GetBaseRatesDatabase(ManForwBaseRateTable^ Values, bool DownloadAll) {
			Unmanaged->GetBaseRatesDatabase(Values, DownloadAll);
		}
#endif 
		/*!
		\brief Add a loan to the loan pool
		*/
		void AddLoan(ManMortgage^ a){Unmanaged->AddLoan(*(a->Unmanaged));}
		/*!
		\brief Remove all loans from the pool
		*/
		void ResetLoans(){Unmanaged->ResetLoans();}
		/*!
		\brief Number of loans in the pool
		*/
		property int LoansCount{
			int get(){return Unmanaged->LoansCount();}
		}
		/*!
		\brief Gets a loan in the pool
		\arg index The index of the loan to retrieve (the first loan has index 0)
		\details This function returns a copy of the loan at the specified index in the pool.<br/>If index is invalid null is returned
		*/
		ManMortgage^ GetLoan(int index){
			const Mortgage* Temp=Unmanaged->GetLoan(index);
			if(Temp) return gcnew ManMortgage(*Temp);
			return nullptr;
		}
		/*!
		\brief Overwrites a loan in the pool
		\arg index The index of the loan to overwrite (the first loan has index 0).
		\arg source The loan that will overwrite the current one
		\details This function will overwrite the loan at the specified index in the pool.<br/>If the index is out of range the function will do nothing.
		*/
		void SetLoan(int index,ManMortgage^ source){
			Mortgage* Temp=Unmanaged->GetLoan(index);
			if(Temp) (*Temp)=*(source->Unmanaged);
		}
		/*!
		\brief Runs the calculation of the waterfall
		\details This function starts the calculation of the loan pool cash flows and the tranches cash flows. The function will return almost immediately.<br/>ProgressUpdateEvent will be emitted any time the computation progresses. When the calculation is completed CalculationFinishedEvent is emitted. 
		*/
		void Calculate(){Unmanaged->Calculate();}
		/*!
		\brief Calculates the call scenario
		\details If this is set to true the Call scenario is calculated alongside the maturity one, otherwise CallResult will return an empty waterfall.<br/>By default this parameter is false.
		\note If this property is true, make sure the structure specifies either the ManWaterfall::CallDate or ManWaterfall::CallReserve and ManWaterfall::CallMultiple otherwise CallResult will be calculated even though the results will be equal to Result
		*/
		property bool RunCall{
			bool get(){return Unmanaged->GetRunCall();}
			void set(bool a){Unmanaged->SetRunCall(a);}
		}
		/*!
		\brief The loan pool cut off date
		 */
		property DateTime CutOffDate{
			DateTime get(){return QDate2DateTime(Unmanaged->GetPoolCutOff());}
			void set(DateTime a){Unmanaged->SetPoolCutOff(DateTime2QDate(a));}
		}
		/*!
		\brief Display the progress window
		\details This property determines whether or not the progress window is displayed.<br/>By default it's set to true.
		*/
		property bool ShowProgress{
			bool get(){return Unmanaged->GetShowProgress();}
			void set(bool a){Unmanaged->SetShowProgress(a);}
		}
		/*!
		\brief The results of the calculation of the maturity scenario 
		\details This property returns the results of the cash flows calculation in case the deal remains outstanding to maturity.<br/>Before the calculation this property will return Structure.
		\sa Structure
		\sa CallResult
		*/
		property ManWaterfall^ Result{
			ManWaterfall^ get(){return gcnew ManWaterfall(Unmanaged->GetResult());}
			void set(ManWaterfall^ source){Unmanaged->GetResult()=*(source->Unmanaged);}
		}
		/*!
		\brief The results of the calculation of the call scenario 
		\details This property returns the results of the cash flows calculation in case the deal gets called.<br/>Before the calculation and if RunCall is false this property will return an empty waterfall.
		\sa Result
		*/
		property ManWaterfall^ CallResult{
			ManWaterfall^ get(){return gcnew ManWaterfall(Unmanaged->GetCallResult());}
			void set(ManWaterfall^ source){Unmanaged->GetCallResult()=*(source->Unmanaged);}
		}
		/*!
		\brief Saves the results to file
		\arg FolderPath The path to the folder where the file will be saved, if the folder doesn't exist it will be created.
		\arg BaseCaseFileName The name of the file, the extension will be .clo
		\details This functions saves the results of the calculation to a .clo file.
		\sa LoadBaseCase
		*/
		void SaveBaseCase(String^ FolderPath,String^ BaseCaseFileName){Unmanaged->SaveBaseCase(String2QString(FolderPath),String2QString(BaseCaseFileName));}
		/*!
		\brief Loads the results from file
		\arg DestPath the path to the file to open
		\details This functions loads the results of the calculation from a .clo file.
		\sa SaveBaseCase
		*/
		bool LoadBaseCase(String^ DestPath){return Unmanaged->LoadBaseCase(String2QString(DestPath));}
		/*!
		\brief Saves the loan pool to file
		\arg FolderPath The path to the folder where the file will be saved, if the folder doesn't exist it will be created.
		\arg LoanPoolFileName The name of the file, the extension will be .clp
		\details This functions saves the results of the loan pool to a .clp file.
		\sa LoadLoanPool
		*/
		void SaveLoanPool(String^ FolderPath,String^ LoanPoolFileName){Unmanaged->SaveLoanPool(String2QString(FolderPath),String2QString(LoanPoolFileName));}
		/*!
		\brief Loads the loan pool from file
		\arg DestPath the path to the file to open
		\details This functions loads the loan pool from a .clp file.
		\sa SaveLoanPool
		*/
		bool LoadLoanPool(String^ DestPath){return Unmanaged->LoadLoanPool(String2QString(DestPath));}
		/*!
		\brief Saves the waterfall and loan pool to file
		\arg FolderPath The path to the folder where the file will be saved, if the folder doesn't exist it will be created.
		\arg ModelFileName The name of the file, the extension will be .clom
		\details This functions saves the whole model to a .clom file.
		\sa LoadModel
		*/
		void SaveModel(String^ FolderPath, String^ ModelFileName) { Unmanaged->SaveModel(String2QString(FolderPath), String2QString(ModelFileName)); }
		/*!
		\brief Loads the waterfall and loan pool from file
		\arg DestPath the path to the file to open
		\details This functions loads the whole model from a .clom file.
		\sa SaveModel
		*/
		bool LoadModel(String^ DestPath) { return Unmanaged->LoadModel(String2QString(DestPath)); }
		/*!
		\brief Determines if multi-threading calculation should be used
		\details This property determines if the model can leverage multiple CPU cores to speed up calculations.<br/>The number of threads will automatically adapt to the number of cores of the machine (both real and logical)<br/>For single core machines or if the number of cores can't be evaluated (e.g. some virtual machines) this property has no effect and all calculations will be sequential.<br/>By default it's set to false.
		\note If this property is set to false your CPU usage will reach 100% during the computation.
		*/
		property bool SequentialComputation{
			bool get(){return Unmanaged->GetSequentialComputation();}
			void set(bool a){Unmanaged->SetSequentialComputation(a);}
		}
		/*!
		\brief Date of the last update of base rates in the model
		*/
		property DateTime BaseRatesUpdateDate{
			DateTime get() { return QDate2DateTime(Unmanaged->GetLiborUpdateDate()); }
		}
		ManCentralUnit(){Unmanaged=new CentralUnitOverlay(this);}
		ManCentralUnit::~ManCentralUnit()
		{
			this->!ManCentralUnit();
			GC::SuppressFinalize(this);
		}
		ManCentralUnit::!ManCentralUnit()
		{
			if (Unmanaged)
			{
				Unmanaged->deleteLater();
				Unmanaged = NULL;
			}
		}
		/*!
		\brief Event called when the calculation progresses.
		\details This event will be triggered every time makes a progress and the current progress (in percentage points) is passed as argument
		*/
		event ProgressUpdateDelegate^ ProgressUpdateEvent;
		/*!
		\brief Event called when the calculation of the cash flows is completed
		*/
		event CalculationFinishedDelegate^ CalculationFinishedEvent;
	internal:
		CentralUnitOverlay* Unmanaged;
		void ProgressUpdateCallBack(double prog){
			ProgressUpdateEvent(prog);
		}
		void CalculationFinishedCallBack(){
			CalculationFinishedEvent();
		}
	};
}
#endif // ManCentralUnit_h__

