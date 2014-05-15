#ifndef ManWaterfall_h__
#define ManWaterfall_h__
#include "ManagedCommons.h"
#include "ManMtgCashFlow.h"
#include "ManTrancheCashFlow.h"
#include "ManWatFalPrior.h"
#include "ManReinvestmentTest.h"
#include "ManTranche.h"
#include "Waterfall.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief CLO Waterfall 
	\details This class implements a way of manage and calculate a whole waterfall for a CLO deal.
	\backward The Read method can load files written by versions 1.73 and later of the model.<br>According to the version you load the results will change according to the following list:<ol style="list-style:none;">
	<li><span>1.73 - </span>Reserve funds targets, floors and multipliers will be loaded as single value vectors with no anchor date</li>
	<li><span>1.74 - </span>Payment frequency will be loaded as a single value vector with no anchor date</li>
	</ol>
	 */
	public ref class ManWaterfall
	{
	private:
		static const unsigned int ClassIdentity=7;
	internal:
		Waterfall* Unmanaged;
		ManWaterfall(const Waterfall& a){Unmanaged=new Waterfall(a);}
	public:
		/*!
		\brief Write class to stream
		\arg Dest Stream to write on
		\details This function writes the class information to a Stream (e.g. FileStream or NetworkStream)
		\return whether the write operation was successful
		*/
		bool Write(IO::Stream^ Dest){
			if(!Dest->CanWrite) return false;
			quint32 MagicNumber=((ClassIdentity << 16) | VersionNumber);
			quint32 BlockSize;
			try{
				IO::BinaryWriter StreamWriter(Dest);
				QByteArray DataRaw;
				QDataStream out(&DataRaw, QIODevice::WriteOnly);
				out.setVersion(QDataStream::Qt_4_8);
				out << (*Unmanaged);
				BlockSize=DataRaw.size();
				StreamWriter.Write(MagicNumber);
				StreamWriter.Write(BlockSize);
				array<Byte>^ DataBuffer=gcnew array<Byte>(BlockSize);
				for(unsigned int i=0;i<BlockSize;i++)DataBuffer[i]=DataRaw.at(i);
				StreamWriter.Write(DataBuffer,0,BlockSize);
				return true;
			}
			catch(Exception^){
				return false;
			}
		}
		/*!
		\brief Read class from stream
		\arg Source Stream to read from
		\details This function reads the class information from a Stream (e.g. FileStream or NetworkStream)
		\warning If the first element in the stream is not of this class, the stream will be advanced by 8 bytes anyway
		\return Whether the read operation was successful
		*/
		bool Read(IO::Stream^ Source){
			if(!Source->CanRead) return false;
			quint32 BlockSize;
			try{
				IO::BinaryReader SizeReader(Source);
				BlockSize=SizeReader.ReadUInt32();
				//if(BlockSize!=MagicNumber) return false;
				if(BlockSize >> 16!=ClassIdentity || (BlockSize & 0xFFFF)< MinSupportedVersion) return false;
				Unmanaged->SetLoadProtocolVersion(BlockSize & 0xffff);
				BlockSize=SizeReader.ReadUInt32();
				array<Byte>^ DataBuffer= SizeReader.ReadBytes(BlockSize);
				QByteArray DataRaw;
				for(unsigned int i=0;i<BlockSize;i++) DataRaw.append(DataBuffer[i]);
				QDataStream DataIn(&DataRaw, QIODevice::ReadOnly);
				DataIn >> (*Unmanaged);
				return true;
			}
			catch(Exception^){
				return false;
			}
		}
		/*!
		\brief The target amount of the reserve fund
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve
		\details This function will return the Bloomberg vector representing the target level of the specified reserve fund
		\sa SetReserveFund
		*/
		String^ GetReserveFundTarget(int index){return QString2String(Unmanaged->GetReserveFundTarget(index));}
		/*!
		\brief The multiple of the target amount of the reserve fund
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve
		\details This function will return the Bloomberg vector representing the multiple of the target level of the specified reserve fund
		\sa SetReserveFund
		*/
		String^ GetReserveFundMultiple(int index){return QString2String(Unmanaged->GetReserveFundMultiple(index));}
		/*!
		\brief The floor amount of the reserve fund
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve
		\details This function will return the Bloomberg vector representing the floor of the specified reserve fund
		\sa SetReserveFund
		*/
		String^ GetReserveFundFloor(int index){return QString2String(Unmanaged->GetReserveFundFloor(index));}
		/*!
		\brief The current amount of the reserve fund
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve
		\details This function will return the value of the current amount of the specified reserve fund
		\sa SetReserveFund
		*/
		double GetReserveFundCurrent(int index){return Unmanaged->GetReserveFundCurrent(index);}
		/*!
		\brief The seniority level after which the reserve is released
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve
		\details This function will return the seniority level that is used to determine when the reserve fund is released.<br/>After all the notes of this seniority level and any note senior to these have redeemed the reserve will be released in the interest waterfall.<br/>A value of 0 means that the reserve will be release only at maturity.
		\sa SetReserveFund
		*/
		int GetReserveFundFreed(int index){return Unmanaged->GetReserveFundFreed(index);}
		/*!
		\brief Whether the reserve funds should be considered stacked
		\details If the property is set to true, the amount outstanding to the credit of the first reserve fund is deducted when calculating the target amount of the second reserve fund.
		*/
		property bool CumulativeReserves{
			bool get() {return Unmanaged->GetCumulativeReserves();}
			void set(bool a){Unmanaged->SetCumulativeReserves(a);}
		}
		/*!
		\brief The cash flows toward the reserve fund
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve. If index is out of range the function will return an empty cash flow.
		\details This function will return the cash flows toward the replenishment of the reserve fund.<br/>The interest and principal flows will distinguish whether the reserve fund was replenished using interest or principal proceeds.<br/>Use ManTrancheCashFlow::GetTotalFlow to get the level of the reserve fund in a given period.<br/>Use ManTrancheCashFlow::GetDeferred to get the shortfall in the reserve fund compared to the target level for a given period.
		\sa SetReserveFund
		\sa SetReserveFundFlow
		*/
		ManTrancheCashFlow^ GetReserveFundFlow(int index){return gcnew ManTrancheCashFlow(Unmanaged->GetReserveFundFlow(index));}
		/*!
		\brief Overwrite the cash flows toward the reserve fund
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve. If index is out of range the function will do nothing
		\arg source The new cash flows that should replace the current ones
		\details This function will overwrite the cash flows toward the reserve fund.<br/>The interest and principal flows will distinguish whether the reserve fund was replenished using interest or principal proceeds.
		\sa SetReserveFund
		\sa GetReserveFundFlow
		*/
		void SetReserveFundFlow(int index, ManTrancheCashFlow^ source){Unmanaged->SetReserveFundFlow(index,*(source->Unmanaged));}
		/*!
		\brief Whether the reserve will fund the interest or principal waterfall
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve
		\details This function will return true if the reserve fund will be part of interest proceeds and false if it will form part of principal proceeds or index is invalid.
		\sa SetReserveFund
		*/
		bool GetReserveToInterest(int RFindex){return Unmanaged->GetReserveToInterest(RFindex);}
		/*!
		\brief Set up a reserve fund
		\arg RFindex the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve
		\arg RFtarget a Bloomberg vector representing the target level of the reserve fund.
		\arg RFmultiple a Bloomberg vector representing the value by which RFtarget will be multiplied.
		\arg RFfloor a Bloomberg vector representing the floor level of the reserve fund. For non-amortizing RFs set to "0"
		\arg RFcurrent the current size of the reserve fund
		\arg RFfreed A seniority level. When all the notes not junior to this seniority level will be redeemed, the RF will be released as excess spread. 0 for freeing it only at maturity
		\arg RFtoInterest Set to true if the reserve fund will be part of interest proceeds when calculating the waterfall, set to false if it's part of principal proceeds.
		\details This function will set up the specified reserve fund according to the supplied parameters.<br/>If RFtarget is a seniority level, the reserve fund target amount will be the sum of all the notes that rank not junior to that level, otherwise the reserve will be a non amortizing.<br/>The target level will then be multiplied by RFmultiple to assess the actual replenishment target.<br/>In any case, the target won't be set below the RFfloor amount.
		*/
		void SetReserveFund(int RFindex,String^ RFtarget,String^ RFmultiple,String^ RFfloor,double RFcurrent,int RFfreed,bool RFtoInterest){Unmanaged->SetReserveFund(RFindex,String2QString(RFtarget),String2QString(RFmultiple),String2QString(RFfloor),RFcurrent,RFfreed,RFtoInterest);}
		/*!
		\brief Removes all the settings for the specified reserve fund
		\arg index the index of the reserve fund. Currently 2 reserves are modeled so  index can be either 0 or 1 for, respectively, the first and second reserve. -1 will reset all the reserves
		\details This function will remove all the settings and cash flows relative to the specified reserve fund.
		*/
		void ResetReserveFund(int index){Unmanaged->ResetReserve(index);}
		/*!
		\brief Share AUM paid as senior expenses
		\details Formatted as percentage (1% = 1)
		*/
		property double SeniorExpenses{
			double get(){return Unmanaged->GetSeniorExpenses();}
			void set(double a){Unmanaged->SetSeniorExpenses(a);}
		}
		/*!
		\brief Share AUM paid as senior fees
		\details Formatted as percentage (1% = 1)
		*/
		property double SeniorFees{
			double get(){return Unmanaged->GetSeniorFees();}
			void set(double a){Unmanaged->SetSeniorFees(a);}
		}
		/*!
		\brief Share AUM paid as junior fees
		\details Formatted as percentage (1% = 1)
		*/
		property double JuniorFees{
			double get(){return Unmanaged->GetJuniorFees();}
			void set(double a){Unmanaged->SetJuniorFees(a);}
		}
		/*!
		\brief Number of months between two IPDs
		\details A Bloomberg vector representing the the number of months between two payment dates
		*/
		property String^ PaymentFrequency{
			String^ get(){return QString2String(Unmanaged->GetPaymentFrequency());}
			void set(String^ a){Unmanaged->SetPaymentFrequency(String2QString(a));}
		}
		/*!
		\brief CCC test limit
		*/
		property double CCCTestLimit{
			double get(){return Unmanaged->GetCCCTestLimit();}
			void set(double a){Unmanaged->SetCCCTestLimit(a);}
		}
		/*!
		\brief Haircut applied to CCC assets above the test limit
		*/
		property double CCChaircut{
			double get(){return Unmanaged->GetCCChaircut();}
			void set(double a){Unmanaged->SetCCChaircut(a);}
		}
		/*!
		\brief Using turbo feature
		\details If set to false, all OC and reinvestment test will redeem the most senior notes in the structure despite any other target specified in the relative waterfall step
		*/
		property bool UseTurbo{
			bool get(){return Unmanaged->GetUseTurbo();}
			void set(bool a){Unmanaged->SetUseTurbo(a);}
		}
		/*!
		\brief Cash in the principal collection account
		*/
		property double PrincipalAvailable{
			double get(){return Unmanaged->GetPrincipalAvailable();}
			void set(double a){Unmanaged->SetPrincipalAvailable(a);}
		}
		/*!
		\brief Cash in the interest collection account
		*/
		property double InterestAvailable{
			double get(){return Unmanaged->GetInterestAvailable();}
			void set(double a){Unmanaged->SetInterestAvailable(a);}
		}
		/*!
		\brief Interest rate charged on deferred junior fees
		*/
		property double JuniorFeesCoupon{
			double get(){return Unmanaged->GetJuniorFeesCoupon();}
			void set(double a){Unmanaged->SetJuniorFeesCoupon(a);}
		}
		/*! 
		\brief Price of the pool upon call
		*/
		property double PoolValueAtCall{
			double get(){return Unmanaged->GetPoolValueAtCall();}
			void set(double a){Unmanaged->SetPoolValueAtCall(a);}
		}
		/*! 
		\brief Consider the call
		\details If set to true the deal will get called at the specified date or residual outstanding amount
		*/
		property bool UseCall{
			bool get(){return Unmanaged->GetUseCall();}
			void set(bool a){Unmanaged->SetUseCall(a);}
		}
		/*! 
		\brief The multiple applied to the residual outstanding to be called
		\details The deal will be called when the residual outstanding of the notes does not exceeds %CallMultiple times the CallReserve
		\sa CallReserve
		*/
		property double CallMultiple{
			double get(){return Unmanaged->GetCallMultiple();}
			void set(double a){Unmanaged->SetCallMultiple(a);}
		}
		/*! 
		\brief The residual outstanding to be called
		\details This can be either a seniority group index or a static amount<br/>In case the seniority group is indicated, the relevant amount will be the outstanding amount of every note at the indicated seniority level or more junior<br/>The deal will be called when the residual outstanding of the notes does not exceeds %CallMultiple times the CallReserve
		\sa CallMultiple
		*/
		property double CallReserve{
			double get(){return Unmanaged->GetCallReserve();}
			void set(double a){Unmanaged->SetCallReserve(a);}
		}
		/*! 
		\brief The cash flows of the loans
		\details The cash flows of the current loan pool, not including reinvestments
		\sa CalculatedMtgPayments
		*/
		property ManMtgCashFlows^ MortgagesPayments{
			ManMtgCashFlows^ get(){return gcnew ManMtgCashFlows(Unmanaged->GetMortgagesPayments());}
			void set(ManMtgCashFlows^ source){Unmanaged->GetMortgagesPayments()=*(source->Unmanaged);}
		}
		/*! 
		\brief The cash flows of the loans
		\details The cash flows of the whole pool, including reinvestments
		\sa MortgagesPayments
		*/
		property ManMtgCashFlows^ CalculatedMtgPayments{
			ManMtgCashFlows^ get(){return gcnew ManMtgCashFlows(Unmanaged->GetCalculatedMtgPayments());}
			void set(ManMtgCashFlows^ source){Unmanaged->GetCalculatedMtgPayments()=*(source->Unmanaged);}
		}
		/*! 
		\brief Cash flows to excess spread
		\sa GetAnnualizedExcess
		*/
		property ManTrancheCashFlow^ ExcessCashFlow{
			ManTrancheCashFlow^ get() {return gcnew ManTrancheCashFlow(Unmanaged->GetExcessCashFlow());}
			void set(ManTrancheCashFlow^ source){Unmanaged->GetExcessCashFlow()=*(source->Unmanaged);}
		}
		/*! 
		\brief Cash flows toward satisfaction of senior expenses
		*/
		property ManTrancheCashFlow^ TotalSeniorExpenses{
			ManTrancheCashFlow^ get() {return gcnew ManTrancheCashFlow(Unmanaged->GetTotalSeniorExpenses());}
			void set(ManTrancheCashFlow^ source){Unmanaged->GetTotalSeniorExpenses()=*(source->Unmanaged);}
		}
		/*! 
		\brief Cash flows toward satisfaction of senior fees
		*/
		property ManTrancheCashFlow^ TotalSeniorFees{
			ManTrancheCashFlow^ get() {return gcnew ManTrancheCashFlow(Unmanaged->GetTotalSeniorFees());}
			void set(ManTrancheCashFlow^ source){Unmanaged->GetTotalSeniorFees()=*(source->Unmanaged);}
		}
		/*! 
		\brief Cash flows toward satisfaction of junior fees
		*/
		property ManTrancheCashFlow^ TotalJuniorFees{
			ManTrancheCashFlow^ get() {return gcnew ManTrancheCashFlow(Unmanaged->GetTotalJuniorFees());}
			void set(ManTrancheCashFlow^ source){Unmanaged->GetTotalJuniorFees()=*(source->Unmanaged);}
		}
		/*! 
		\brief Cash flows toward junior fees and excess spread
		*/
		property ManTrancheCashFlow^ RawAnnualizedExcess{
			ManTrancheCashFlow^ get() {return gcnew ManTrancheCashFlow(Unmanaged->GetRawAnnualizedExcess());}
			void set(ManTrancheCashFlow^ source){Unmanaged->GetRawAnnualizedExcess()=*(source->Unmanaged);}
		}
		/*! 
		\brief Cash flows toward reinvestments
		*/
		property ManTrancheCashFlow^ Reinvested{
			ManTrancheCashFlow^ get() {return gcnew ManTrancheCashFlow(Unmanaged->GetReinvested());}
			void set(ManTrancheCashFlow^ source){Unmanaged->GetReinvested()=*(source->Unmanaged);}
		}
		/*! 
		\brief Vector representing the share of CCC assets in the pool
		*/
		property String^ CCCcurve{
			String^ get() {return QString2String(Unmanaged->GetCCCcurve());}
			void set(String^ a) {Unmanaged->SetCCCcurve(String2QString(a));}
		}
		/*! 
		\brief Next IPD
		*/
		property DateTime FirstIPDdate{
			DateTime get() {return QDate2DateTime(Unmanaged->GetFirstIPDdate());}
			void set(DateTime a){Unmanaged->SetFirstIPDdate(DateTime2QDate(a));}
		}
		/*! 
		\brief Latest IPD
		*/
		property DateTime LastIPDdate{
			DateTime get() {return QDate2DateTime(Unmanaged->GetLastIPDdate());}
			void set(DateTime a){Unmanaged->SetLastIPDdate(DateTime2QDate(a));}
		}
		/*! 
		\brief Date on which the deal should get called
		\sa CalledPeriod
		*/
		property DateTime CallDate{
			DateTime get() {return QDate2DateTime(Unmanaged->GetCallDate());}
			void set(DateTime a){Unmanaged->SetCallDate(DateTime2QDate(a));}
		}
		/*! 
		\brief Date on which the deal gets called according to model
		\details This represents the date on which the residual amount outstanding of notes reaches the call trigger or the call date is reached
		*/
		property DateTime CalledPeriod{
			DateTime get() {return QDate2DateTime(Unmanaged->GetCalledPeriod());}
		}
		/*! 
		\brief Estimated maturity of the deal
		*/
		property DateTime StructureMaturity{
			DateTime get() {return QDate2DateTime(Unmanaged->GetStructureMaturity());}
		}
		/*! 
		\brief Number of tranches in the deal
		*/
		property int TranchesCount{
			int get(){return Unmanaged->GetTranchesCount();}
		}
		/*! 
		\brief Number of waterfall steps in the deal
		*/
		property int StepsCount{
			int get(){return Unmanaged->GetStepsCount();}
		}
		/*! 
		\brief Calculates the relative excess spread
		\arg index The period index for which to calculate the relative excess spread (first period id 0)
		\arg AsShareOfLoans if set to true the percentage will be calculated as share of the AUM instead of the notes (usually set to false)
		\details calculates the annualized excess spread (excess spread and junior fees) as a share of the notes outstanding or of the AUM
		*/
		double GetAnnualizedExcess(int index, bool AsShareOfLoans){return Unmanaged->GetAnnualizedExcess(index,AsShareOfLoans);}
		/*! 
		\brief Calculates the equity return
		\arg index The period index for which to calculate the relative excess spread (first period id 0)
		\details Calculates the cash flows to equity as a share of the original notes outstanding
		\sa GetCumulativeEquityReturn
		*/
		double GetEquityReturn(int index){return Unmanaged->GetEquityReturn(index);}
		/*! 
		\brief Calculates the cumulative equity return
		\arg index The period index for which to calculate the relative excess spread (first period id 0)
		\details Calculates the cumulative cash flows to equity as a share of the original notes outstanding
		\sa GetEquityReturn
		*/
		double GetCumulativeEquityReturn(int index){return Unmanaged->GetCumulativeEquityReturn(index);}
		/*! 
		\brief Calculates the ratio between call value and equity notes
		\arg index The period index for which to calculate the relative excess spread (first period id 0)
		\details Calculates the ratio between the call value of the pool and the equity notes outstanding
		*/
		double GetCallEquityRatio(int index){return Unmanaged->GetCallEquityRatio(index);}
		/*! 
		\brief Calculates the weighted average cost of capital of the deal
		*/
		double GetWACostOfCapital(int index){return Unmanaged->GetWACostOfCapital(index);}
		/*! 
		\brief Calculates the credit enhancement of the specified tranche
		\arg Tranchename the ticker (*without* yellow key) of the tranche to consider or its ISIN
		\arg TimeIndex index of time for which the CE should be calculated. Use -1 for the current CE
		\sa GetCreditEnhancement(int,int)
		*/
		double GetCreditEnhancement(String^ Tranchename,int TimeIndex){return Unmanaged->GetCreditEnhancement(String2QString(Tranchename),TimeIndex);}
		/*! 
		\brief Calculates the credit enhancement of the specified tranche
		\arg TrancheIndex the index of the tranche to consider
		\arg TimeIndex index of time for which the CE should be calculated. Use -1 for the current CE
		\sa GetCreditEnhancement(String^,int)
		*/
		double GetCreditEnhancement(int TrancheIndex,int TimeIndex){return Unmanaged->GetCreditEnhancement(TrancheIndex,TimeIndex);}
		/*! 
		\brief Adds a cash flow to the loan pool
		\arg a The cash flow to add
		*/
		void AddMortgagesFlows(ManMtgCashFlows^ a){Unmanaged->AddMortgagesFlows(*(a->Unmanaged));}
		/*! 
		\brief Removes all loans cash flows
		*/
		void ResetMtgFlows(){Unmanaged->ResetMtgFlows();}
		/*!
		\brief Define the characteristics of the reinvestment test
		\arg ReinvPer The date on which the reinvestment period will end
		\arg TstLvl The minimum level of the test for it to pass in a scalar form (1=100%, 1.1=110%)
		\arg IIshare Share toward acquiring new collateral during reinvestment period in a scalar form (0.5=50%)
		\arg IRshare Share redemption of notes during reinvestment period in a scalar form (0.5=50%)
		\arg OIshare Share toward acquiring new collateral after reinvestment period in a scalar form (0.5=50%)
		\arg ORshare Share redemption of notes after reinvestment period in a scalar form (0.5=50%)
		*/
		void SetupReinvestmentTest(DateTime ReinvPer,double TstLvl, double IIshare,double IRshare,double OIshare,double ORshare){Unmanaged->SetupReinvestmentTest(DateTime2QDate(ReinvPer),TstLvl,IIshare,IRshare,OIshare,ORshare);}
		/*!
		\brief Define the characteristics of the bond bought using the reinvestment funds
		\arg IntrVec A Bloomberg vector defining the coupon or the margin paid by the bond
		\arg CPRVec The CPR vector used for the reinvestment bond
		\arg CDRVec The CDR vector used for the reinvestment bond
		\arg LSVec The LS vector used for the reinvestment bond
		\arg WALval The WAL vector used for the reinvestment bond
		\arg PayFreq The number of months between two interest payments from the bond 
		\arg AnnuityVec A Bloomberg-style vector defining whether the bond is an annuity.<br/>The vector can assume the value N for interest only and Y for annuity and can only move in steps.<br/>Examples of *valid* vectors:<br/>Y<br/>N 24S Y<br/>Examples of *invalid* vectors:<br/>5 24S N<br/>N 24R Y<br/>
		\arg FloatingBase the base rate value to be added to IntrVec if it's a margin vector. Use 0 for fixed rate.
		*/
		void SetupReinvBond(String^ IntrVec,String^ CPRVec,String^ CDRVec,String^ LSVec,String^ WALval,int PayFreq,String^ AnnuityVec,double FloatingBase){
			Unmanaged->SetupReinvBond(String2QString(IntrVec),String2QString(CPRVec),String2QString(CDRVec),String2QString(LSVec),String2QString(WALval),PayFreq,String2QString(AnnuityVec),FloatingBase);
		}
		/*! 
		\brief Removes all steps from the waterfall
		*/
		void ResetSteps(){Unmanaged->ResetSteps();}
		/*! 
		\brief Removes all tranches from the structure
		*/
		void ResetTranches(){Unmanaged->ResetTranches();}
		/*! 
		\brief Calculates all the cash flows according to the structure
		*/
		bool CalculateTranchesCashFlows(){return Unmanaged->CalculateTranchesCashFlows();}
		/*!
		\brief Determines if all the parameters needed for the cash flow calculation are correct.
		\return An empty string if all the parameters are correct otherwise a list of the the invalid parameters (one for each line).
		\details <table border="1" cellpadding="3">
		<tr>
		<th>Check Performed</th>
		<th>String returned if failed</th>
		</tr>
		<tr>
		<td>Senior expenses not negative</td>
		<td>Senior Expenses</td>
		</tr>
		<tr>
		<td>Senior fees not negative</td>
		<td>Senior Fees</td>
		</tr>
		<tr>
		<td>Junior fees not negative</td>
		<td>Junior Fees</td>
		</tr>
		<tr>
		<td>At least one tranche in the deal</td>
		<td>Tranches</td>
		</tr>
		<tr>
		<td>At least one step in the waterfall</td>
		<td>Waterfall Steps</td>
		</tr>
		<tr>
		<td>Cash flows of loanpool not empty</td>
		<td>Loans Payments</td>
		</tr>
		<tr>
		<td>Not less than one month between two interest payments</td>
		<td>IDP Frequency</td>
		</tr>
		<tr>
		<td>Reinvestment test CPR assuption is a valid vector</td>
		<td>CPR</td>
		</tr>
		<tr>
		<td>Reinvestment test CDR assuption is a valid vector</td>
		<td>CDR</td>
		</tr>
		<tr>
		<td>Reinvestment test LS assuption is a valid vector</td>
		<td>LS</td>
		</tr>
		<tr>
		<td>Reinvestment bond WAL is a valid vector</td>
		<td>Reinvestment Bond WAL</td>
		</tr>
		<tr>
		<td>Reinvestment period date valid and greater than 1/1/2000</td>
		<td>Reinvestment Period</td>
		</tr>
		<tr>
		<td>Reinvestment test trigger not negative</td>
		<td>Reinvestment Test Limit</td>
		</tr>
		<tr>
		<td>Not less than one month between two interest payments made by the reinvestment bond</td>
		<td>Reinvestment Bond Payment Frequency</td>
		</tr>
		<tr>
		<td>Reinvestment bond margin is a valid vector</td>
		<td>Reinvestment Bond Spread</td>
		</tr>
		<tr>
		<td>CCC curve is a valid vector</td>
		<td>CCC Curve</td>
		</tr>
		<tr>
		<td>Haircut on CCC not negative</td>
		<td>CCC Value</td>
		</tr>
		<tr>
		<td>CCC test trigger not negative</td>
		<td>CCC Test Limit</td>
		</tr>
		<tr>
		<td>Pool call price not negative</td>
		<td>Pool Value at Call</td>
		</tr>
		<tr>
		<td>No call criteria specified and UseCall set to true</td>
		<td>Specify a call Criteria to use the Call</td>
		</tr>
		<tr>
		<td>Loan pool cut off valid and greater than 1/1/2000</td>
		<td>Pool Cut Off Date</td>
		</tr>
		<tr>
		<td>Invalid shares to apply in case of broken reinvestment test</td>
		<td>Reinvestment Test Shares InRedempShare: %1 | OutRedempShare: %2 | InReinvShare: %3 | OutReinvShare: %4 | %5 | %6</td>
		</tr>
		<tr>
		<td>Previous IPD valid and greater than 1/1/2000</td>
		<td>Pool Cut Off Date</td>
		</tr>
		<tr>
		<td>Next IPD valid and greater than 1/1/2000</td>
		<td>Pool Cut Off Date</td>
		</tr>
		</table>
		*/
		property String^ ReadyToCalculate{
			String^ get(){return QString2String(Unmanaged->ReadyToCalculate());}
		}
		/*! 
		\brief Returns the Index-th step in the waterfall
		\arg Index The index of the step of the waterfall (first is 0). If invalid a null pointer is returned
		*/
		ManWatFalPrior^ GetStep(int Index){
			const WatFalPrior* Temp=Unmanaged->GetStep(Index);
			if(Temp) return gcnew ManWatFalPrior(*Temp);
			return nullptr;
		}
		/*! 
		\brief Overwrites a step in the waterfall
		\arg Index The index of the step of the waterfall (first is 0). If out of range the function will do nothing.
		\arg source The step that will overwrite the current one.
		*/
		void SetStep(int Index,ManWatFalPrior^ source){
			WatFalPrior* Temp=Unmanaged->GetStep(Index);
			if(Temp) (*Temp)=*(source->Unmanaged);
		}
		/*! 
		\brief Adds a step to the waterfall
		\arg a The step to add
		\details The step will be inserted at the bottom of the waterfall
		*/
		void AddStep(ManWatFalPrior^ a){Unmanaged->AddStep(*(a->Unmanaged));}
		/*! 
		\brief Returns the reinvestment test
		*/
		property ManReinvestmentTest^ reinvestmentTest{
			ManReinvestmentTest^ get(){return gcnew ManReinvestmentTest(Unmanaged->GetReinvestmentTest());}
		}
		/*! 
		\brief Returns the tranche at the specified index 
		\arg Index the index of the tranche (first is 0). If invalid NULL is returned
		*/
		ManTranche^ GetTranche(int Index){
			const Tranche* Temp=Unmanaged->GetTranche(Index);
			if(Temp) return gcnew ManTranche(*Temp);
			return nullptr;
		}
		/*! 
		\brief Returns the tranche with the specified name or ISIN
		\arg TrancheName The ticker (*without* yellow key) of the tranche or its ISIN. If invalid NULL is returned
		*/
		ManTranche^ GetTranche(String^ TrancheName){
			const Tranche* Temp=Unmanaged->GetTranche(String2QString(TrancheName));
			if(Temp) return gcnew ManTranche(*Temp);
			return nullptr;
		}
		/*! 
		\brief Overwrites the tranche at the specified index
		\arg Index the index of the tranche (first is 0). If index is out of range the function will do nothing.
		\arg source The tranche that will overwrite the current one
		*/
		void SetTranche(int Index,ManTranche^ source){
			Tranche* Temp=Unmanaged->GetTranche(Index);
			if(Temp) (*Temp)=*(source->Unmanaged);
		}
		/*! 
		\brief Overwrites the tranche with the specified name or ISIN
		\arg Index the index of the tranche (first is 0). If index is out of range the function will do nothing.
		\arg source The tranche that will overwrite the current one
		*/
		void SetTranche(String^ TrancheName,ManTranche^ source){
			Tranche* Temp=Unmanaged->GetTranche(String2QString(TrancheName));
			if(Temp) (*Temp)=*(source->Unmanaged);
		}
		/*! 
		\brief Add a tranche to the structure
		\arg a Tranche to add
		*/
		void AddTranche(const ManTranche^ a){
			Unmanaged->AddTranche(*(a->Unmanaged));
		}

		/*! 
		\brief Copy another waterfall
		\arg a Waterfall to copy
		*/
		void CopyWaterfall(ManWaterfall^ a){Unmanaged->operator=(*(a->Unmanaged));}
		ManWaterfall(){Unmanaged=new Waterfall();}
		//! Copy constructor
		ManWaterfall(const ManWaterfall% a){Unmanaged=new Waterfall(*(a.Unmanaged));}
		//! Copy constructor for ref class
		ManWaterfall(ManWaterfall^ a){Unmanaged=new Waterfall(*(a->Unmanaged));}
		!ManWaterfall()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManWaterfall()
		{
			this->!ManWaterfall();
			GC::SuppressFinalize(this);
		}
	
	};
}
#endif // ManWaterfall_h__
