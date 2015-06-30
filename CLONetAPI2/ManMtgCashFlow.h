#ifndef ManMtgCashFlow_h__
#define ManMtgCashFlow_h__
#include "MtgCashFlow.h"
#include <QByteArray>
#include <QDataStream>
#include "ManagedCommons.h"
#include "ManGenericCashFlow.h"
#include "ManBloombergVector.h"
using namespace System;
//! Namespace containing all the classes of the CLO model
namespace ManagedCLO {
	/*!
	\brief Cash Flows from a Loan or a Pool of Loans
	\details This class implements a way of manage cash flows from a pool of loans.<br/>The flows will always be sorted by date.
	 */
	public ref class ManMtgCashFlows : ManGenericCashFlow
	{
	private:
		static const unsigned int ClassIdentity=1;
	internal:
		MtgCashFlow* Unmanaged;
		ManMtgCashFlows(const MtgCashFlow& a){Unmanaged=new MtgCashFlow(a);}
	public:			
		/*! Enum that defines the different kind of flows stored in the class */
		enum class ManMtgFlowType : int {
			InterestFlow, /*!< Interest */
			PrincipalFlow, /*!< Scheduled Principal */
			PrepaymentFlow, /*!< Prepayment of Principal */
			AmountOutstandingFlow, /*!< Amount outstanding of the loan */
			WACouponFlow, /*!< Weighted Average Coupon of the Pool */
			AccruedInterestFlow, /*!< Accrued unpaid Interest */
			LossOnInterestFlow, /*!< Loss on accrued interest */
			PrincipalDefault, /*!< New defaults */
			PrincipalRecovered, /*!< Recoveries of principal */
			InterestRecovered, /*!< Recoveries of accrued interest */
			LossFlow /*!< Loss on principal */
		};
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
				out.setVersion(QDataStream::Qt_5_3);
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
		\warning If the first element in the stream is not of this class, the stream will be advanced by 4 bytes anyway
		\return Whether the read operation was successful
		*/
		bool Read(IO::Stream^ Source) {
			if (!Source->CanRead) return false;
			quint32 BlockSize;
			try {
				IO::BinaryReader SizeReader(Source);
				BlockSize = SizeReader.ReadUInt32();
				if (BlockSize >> 16 != ClassIdentity || (BlockSize & 0xFFFF)< MinSupportedVersion || (BlockSize & 0xFFFF)> VersionNumber) return false;
				Unmanaged->SetLoadProtocolVersion(BlockSize & 0xFFFF);
				BlockSize = SizeReader.ReadUInt32();
				array<Byte>^ DataBuffer = SizeReader.ReadBytes(BlockSize);
				QByteArray DataRaw;
				for (unsigned int i = 0; i < BlockSize; i++) DataRaw.append(DataBuffer[i]);
				QDataStream DataIn(&DataRaw, QIODevice::ReadOnly);
				DataIn >> (*Unmanaged);
				return true;
			}
			catch (Exception^) {
				return false;
			}
		}
		/*!
		\brief Gets the interest cash flow.
		\arg index The index of the cash flow for which the interest must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the interest cash flow at position index in the series.
		\sa GetInterest(DateTime)
		*/
		double GetInterest(int index) {return Unmanaged->GetInterest(index);}
		/*!
		\brief Gets the scheduled principal cash flow.
		\arg index The index of the cash flow for which the scheduled principal must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the scheduled principal cash flow at position index in the series.
		\sa GetScheduled(DateTime)
		*/
		double GetScheduled(int index) {return Unmanaged->GetScheduled(index);}
		/*!
		\brief Gets the prepayments cash flow.
		\arg index The index of the cash flow for which the prepaid principal must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the prepaid principal cash flow at position index in the series.
		\sa GetPrepay(DateTime)
		*/
		double GetPrepay(int index) {return Unmanaged->GetPrepay(index);}
		/*!
		\brief Gets the total principal payments cash flow.
		\arg index The index of the cash flow for which the principal must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the principal cash flow at position index in the series, this include both scheduled principal and prepayments.
		\sa GetPrincipal(DateTime)
		*/
		double GetPrincipal(int index) {return GetScheduled(index)+GetPrepay(index);}
		/*!
		\brief Gets the principal loss cash flow.
		\arg index The index of the cash flow for which the loss must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant loss.<br/>If index is out of range, 0 is returned
		\details Returns the principal loss occurred at position index in the series.
		\sa GetLoss(DateTime)
		*/
		double GetLoss(int index) {return Unmanaged->GetLoss(index);}
		/*!
		\brief Gets the Amount Outstanding of the loan.
		\arg index The index of the cash flow for which the amount outstanding must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant amount outstanding.<br/>If index is out of range, 0 is returned
		\details Returns the amount outstanding of the loans at position index in the series.
		\sa GetAmountOut(DateTime)
		*/
		double GetAmountOut(int index) {return Unmanaged->GetAmountOut(index);}
		/*!
		\brief Gets the accrued interest of the loan.
		\arg index The index of the cash flow for which the accrued interest must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant accrued interest.<br/>If index is out of range, 0 is returned
		\details Returns the accrued unpaid interest of the loans at position index in the series.
		\sa GetAccruedInterest(DateTime)
		*/
		double GetAccruedInterest(int index) {return Unmanaged->GetAccruedInterest(index);}
		/*!
		\brief Gets the loss on accrued interest of the loans.
		\arg index The index of the cash flow for which the loss accrued interest must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant loss.<br/>If index is out of range, 0 is returned
		\details Returns the loss on accrued unpaid interest occurred at position index in the series.
		\sa GetLossOnInterest(DateTime)
		*/
		double GetLossOnInterest(int index) {return Unmanaged->GetLossOnInterest(index);}
		/*!
		\brief Gets the WA coupon of the loans.
		\arg index The index of the cash flow for which the WA coupon must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the WA coupon.<br/>If index is out of range, 0 is returned
		\details Returns the weighted average coupon of the loans at position index in the series.
		\sa GetWAcoupon(DateTime)
		*/
		double GetWAcoupon(int index) {return Unmanaged->GetWAcoupon(index);}
		/*!
		\brief Gets the interest cash flow.
		\arg index The date for which the interest must be retrieved.
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the interest cash flow paid on the date index.
		\sa GetInterest(int)
		*/
		double GetInterest(DateTime index) {return Unmanaged->GetInterest(DateTime2QDate(index));}
		/*!
		\brief Gets the scheduled principal cash flow.
		\arg index The date for which the scheduled principal must be retrieved.
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the scheduled principal cash flow paid on the date index.
		\sa GetScheduled(int)
		*/
		double GetScheduled(DateTime index) {return Unmanaged->GetScheduled(DateTime2QDate(index));}
		/*!
		\brief Gets the prepayments cash flow.
		\arg index The date for which the prepaid principal must be retrieved.
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the prepaid principal cash flow paid on the date index.
		\sa GetPrepay(int)
		*/
		double GetPrepay(DateTime index) {return Unmanaged->GetPrepay(DateTime2QDate(index));}
		/*!
		\brief Gets the total principal payments cash flow.
		\arg index The date for which the principal must be retrieved.
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the principal cash flow paid on the date index, this include both scheduled principal and prepayments.
		\sa GetPrincipal(int)
		*/
		double GetPrincipal(DateTime index) {return Unmanaged->GetPrincipal(DateTime2QDate(index));}
		/*!
		\brief Gets the principal loss cash flow.
		\arg index The date for which the loss must be retrieved.
		\return A double containing the relevant loss.<br/>If index is out of range, 0 is returned
		\details Returns the principal loss occurred on date index.
		\sa GetLoss(int)
		*/
		double GetLoss(DateTime index) {return Unmanaged->GetLoss(DateTime2QDate(index));}
		/*!
		\brief Gets the Amount Outstanding of the loan.
		\arg index The idate for which the amount outstanding must be retrieved.
		\return A double containing the relevant amount outstanding.<br/>If index is out of range, 0 is returned
		\details Returns the amount outstanding of the loans on date index.
		\sa GetAmountOut(int)
		*/
		double GetAmountOut(DateTime index) {return Unmanaged->GetAmountOut(DateTime2QDate(index));}
		/*!
		\brief Gets the accrued interest of the loan.
		\arg index The date for which the accrued interest must be retrieved.
		\return A double containing the relevant accrued interest.<br/>If index is out of range, 0 is returned
		\details Returns the accrued unpaid interest of the loans on date index.
		\sa GetAccruedInterest(int)
		*/
		double GetAccruedInterest(DateTime index) {return Unmanaged->GetAccruedInterest(DateTime2QDate(index));}
		/*!
		\brief Gets the loss on accrued interest of the loans.
		\arg index The date for which the loss accrued interest must be retrieved.
		\return A double containing the relevant loss.<br/>If index is out of range, 0 is returned
		\details Returns the loss on accrued unpaid interest occurred on date index.
		\sa GetLossOnInterest(int)
		*/
		double GetLossOnInterest(DateTime index) {return Unmanaged->GetLossOnInterest(DateTime2QDate(index));}
		/*!
		\brief Gets the WA coupon of the loans.
		\arg index The date for which the WA coupon must be retrieved.
		\return A double containing the WA coupon.<br/>If index is out of range, 0 is returned
		\details Returns the weighted average coupon of the loans on date index.
		\sa GetWAcoupon(int)
		*/
		double GetWAcoupon(DateTime index) {return Unmanaged->GetWAcoupon(DateTime2QDate(index));}
		/*!
		\brief Adds a flow to the series.
		\arg Dte The date of the flow.
		\arg Amt The amount of the flow.
		\arg FlowTpe The type of flow
		\details Adds to the series at date Dte a flow of type FlowTpe. If at the specified date another amount of the same type is already in the series Amt will be added to it.
		\sa AddFlow(ManMtgCashFlows^)
		*/
		void AddFlow(DateTime Dte, double Amt, ManMtgFlowType FlowTpe){Unmanaged->AddFlow(DateTime2QDate(Dte),Amt,static_cast<MtgCashFlow::MtgFlowType>(static_cast<int>(FlowTpe)));}
		/*!
		\brief Adds flows to the series.
		\arg a A reference to the source of the flows
		\details Joins two ManMtgCashFlows into one another.
		\sa AddFlow(DateTime,double,ManMtgFlowType)
		*/
		void AddFlow(ManMtgCashFlows^ a){Unmanaged->AddFlow(*(a->Unmanaged));}
		/*!
		\brief Applies a stress scenario to a series of cash flows
		\arg CPRVec The vector describing the CPR to apply to the cash flows
		\arg CDRVec The vector describing the CDR to apply to the cash flows
		\arg LossVec The vector describing the Loss Severity to apply to the cash flows
		\details This function is designed to apply a stress scenario to a series of base cash flow.<br/>To obtain the right result the base should be a cash flow with no prepayments and no losses.
		\warning If the original loans run different prepayments or losses multipliers, the results will only be an approximation.
		\code{.cpp}

		// Example Usage:
		ManMortgage^ ExamplMtg = gcnew ManMortgage();
		ExamplMtg->Annuity = "A"; // Annuity Repayment
		ExamplMtg->Interest = "5"; // 5% Annual Interest Rate
		ExamplMtg->PaymentFreq = "3"; // Quarterly Payments
		ExamplMtg->MaturityDate = DateTime(2020, 5, 30); // Maturity 30th May 2020
		ExamplMtg->Size = 1000000.0; // Nominal of 1 million
		ExamplMtg->CalculateCashFlows(DateTime(2015, 5, 30),"0","0","0"); // Calculate the cash flow with cut-off 30th May 2015, 0 prepayments and 0 losses
		ManMtgCashFlows^ StressedFlows = ExamplMtg->CashFlow->ApplyScenario("10", "2", "50"); // Apply 10% CPR, 2% CDR and 50% Loss Severity

		\endcode
		*/
		ManMtgCashFlows^ ApplyScenario(String^ CPRVec, String^ CDRVec, String^ LossVec) {
			return gcnew ManMtgCashFlows(Unmanaged->ApplyScenario(String2QString(CPRVec), String2QString(CDRVec), String2QString(LossVec)));
		}
		ManMtgCashFlows(){Unmanaged=new MtgCashFlow();}
		//! Copy constructor
		ManMtgCashFlows(const ManMtgCashFlows% a){Unmanaged=new MtgCashFlow(*(a.Unmanaged));}

		//! Copy constructor for ref class
		ManMtgCashFlows(ManMtgCashFlows^ a){Unmanaged=new MtgCashFlow(*(a->Unmanaged));}
		!ManMtgCashFlows()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManMtgCashFlows()
		{
			this->!ManMtgCashFlows();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManMtgCashFlow_h__
