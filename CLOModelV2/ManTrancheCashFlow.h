#ifndef ManTrancheCashFlow_h__
#define ManTrancheCashFlow_h__

#include "TrancheCashFlow.h"
#include "ManagedCommons.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Cash Flows from a tranche of a CLO
	\details This class implements a way of manage cash flows from a tranche of a CLO.<br/>The flows will always be sorted by date.
	 */
	public ref class ManTrancheCashFlow
	{
	private:
		static const unsigned int ClassIdentity=6;
	internal:
		TrancheCashFlow* Unmanaged;
		ManTrancheCashFlow(const TrancheCashFlow& a){Unmanaged=new TrancheCashFlow(a);}
	public:
		/*! Enum defining the type of flow*/
		enum class ManTrancheFlowType : int {
			InterestFlow, /*!< Interest cash flow*/
			PrincipalFlow,/*!< Principal cash flow*/
			OCFlow, /*!< OC test result*/
			ICFlow, /*!< IC test result*/
			DeferredFlow /*!< Interest deferral*/
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
		\warning If the first element in the stream is not of this class, the stream will be advanced by 8 bytes anyway
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
		\brief Number of flows in the series
		*/
		property int Count{int get(){return Unmanaged->Count();}}
		/*!
		\brief Get the date of a flow in the series
		\arg index the index of the flow for which to retrieve the date (0 is the first)
		*/
		DateTime GetDate(int index){return QDate2DateTime(Unmanaged->GetDate(index));}
		/*!
		\brief Get the index of the flow happening at a certain date
		\arg a the date of the flow to search
		\return the index of the flow or -1 if not found
		*/
		int FindDate(DateTime a){return Unmanaged->FindDate(DateTime2QDate(a));}
		/*!
		\brief Gets the interest cash flow.
		\arg index The index of the cash flow for which the interest must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the interest cash flow at position index in the series.
		\sa GetInterest(DateTime)
		*/
		double GetInterest(int index){return Unmanaged->GetInterest(index);}
		/*!
		\brief Gets the principal cash flow.
		\arg index The index of the cash flow for which the principal must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the principal cash flow at position index in the series.
		\sa GetPrincipal(DateTime)
		*/
		double GetPrincipal(int index){return Unmanaged->GetPrincipal(index);}
		/*!
		\brief Gets the result of the OC test.
		\arg index The index of the cash flow for which the OC test must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant test result.<br/>If index is out of range, 0 is returned
		\details Returns the OC test result at position index in the series.
		\sa GetOCTest(DateTime)
		*/
		double GetOCTest(int index){return Unmanaged->GetOCTest(index);}
		/*!
		\brief Gets the result of the IC test.
		\arg index The index of the cash flow for which the IC test must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant test result.<br/>If index is out of range, 0 is returned
		\details Returns the IC test result at position index in the series.
		\sa GetICTest(DateTime)
		*/
		double GetICTest(int index){return Unmanaged->GetICTest(index);}
		/*!
		\brief Gets the total cash flow.
		\arg index The index of the cash flow for which the total cash must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the total (principal and interest) cash flow at position index in the series.
		\sa GetTotalFlow(DateTime)
		*/
		double GetTotalFlow(int index){return Unmanaged->GetTotalFlow(index);}
		/*!
		\brief Gets the amount outstanding of the tranche
		\arg index The index of the cash flow for which the outstanding must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant outstanding amount.<br/>If index is out of range, 0 is returned
		\details Returns the amount outstanding of the tranche at position index in the series.
		\sa GetAmountOutstanding(DateTime)
		*/
		double GetAmountOutstanding(int index){return Unmanaged->GetAmountOutstanding(index);}
		/*!
		\brief Gets the cumulative deferred interest of the tranche
		\arg index The index of the cash flow for which the deferred interest must be retrieved.<br/>The first flow will be at index 0
		\return A double containing the relevant deferred interest.<br/>If index is out of range, 0 is returned
		\details Returns the cumulative deferred interest of the tranche at position index in the series.
		\sa GetDeferred(DateTime)
		*/
		double GetDeferred(int index){return Unmanaged->GetDeferred(index);}
		/*!
		\brief Gets the interest cash flow.
		\arg index The date for which the interest must be retrieved.
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the interest cash flow paid on the date index.
		\sa GetInterest(int)
		*/
		double GetInterest(DateTime index){return Unmanaged->GetInterest(DateTime2QDate(index));}
		/*!
		\brief Gets the interest cash flow.
		\arg index The date for which the interest must be retrieved.
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the interest cash flow paid on the date index.
		\sa GetPrincipal(int)
		*/
		double GetPrincipal(DateTime index){return Unmanaged->GetPrincipal(DateTime2QDate(index));}
		/*!
		\brief Gets the result of the OC test.
		\arg index The date for which the interest must be retrieved.
		\return A double containing the relevant test result.<br/>If index is out of range, 0 is returned
		\details Returns the OC test result on the date index.
		\sa GetOCTest(int)
		*/
		double GetOCTest(DateTime index){return Unmanaged->GetOCTest(DateTime2QDate(index));}
		/*!
		\brief Gets the result of the IC test.
		\arg index The date for which the interest must be retrieved.
		\return A double containing the relevant test result.<br/>If index is out of range, 0 is returned
		\details Returns the IC test result on the date index.
		\sa GetICTest(int)
		*/
		double GetICTest(DateTime index){return Unmanaged->GetICTest(DateTime2QDate(index));}
		/*!
		\brief Gets the total cash flow.
		\arg index The date for which the interest must be retrieved.
		\return A double containing the relevant cash flow.<br/>If index is out of range, 0 is returned
		\details Returns the total (principal and interest) cash flow paid on the date index.
		\sa GetTotalFlow(int)
		*/
		double GetTotalFlow(DateTime index){return Unmanaged->GetTotalFlow(DateTime2QDate(index));}
		/*!
		\brief Gets the amount outstanding of the tranche
		\arg index The date for which the interest must be retrieved.
		\return A double containing the relevant outstanding amount.<br/>If index is out of range, 0 is returned
		\details Returns the amount outstanding of the tranche on the date index.
		\sa GetAmountOutstanding(int)
		*/
		double GetAmountOutstanding(DateTime index){return Unmanaged->GetAmountOutstanding(DateTime2QDate(index));}
		/*!
		\brief Gets the cumulative deferred interest of the tranche
		\arg index The date for which the interest must be retrieved.
		\return A double containing the relevant deferred interest.<br/>If index is out of range, 0 is returned
		\details Returns the cumulative deferred interest of the tranche on the date index.
		\sa GetDeferred(int)
		*/
		double GetDeferred(DateTime index){return Unmanaged->GetDeferred(DateTime2QDate(index));}

		/*!
		\brief Equivalent to GetLastFlowDate(false)
		\sa GetLastFlowDate(bool)
		\sa GetLastFlowIndex()
		\sa GetLastFlowIndex(bool)
		*/
		DateTime GetLastFlowDate() { return GetLastFlowDate(false); }
		/*!
		\brief Equivalent to GetLastFlowIndex(false)
		\sa GetLastFlowIndex(bool)
		\sa GetLastFlowDate()
		\sa GetLastFlowDate(bool)
		*/
		int GetLastFlowIndex() { return GetLastFlowIndex(false); }
		/*!
		\brief Gets the date of the last cash flow (either principal, interest or deferred interest if IncludeDeferred is true)
		\arg IncludeDeferred determines if the accrued and deferred interest is considered as a cash flow or not
		\return The date of the last cash flow or a null date if there are no positive cash flows.
		\sa GetLastFlowDate()
		\sa GetLastFlowIndex()
		\sa GetLastFlowIndex(bool)
		*/
		DateTime GetLastFlowDate(bool IncludeDeferred) { return QDate2DateTime(Unmanaged->GetLastFlowDate(IncludeDeferred)); }
		/*!
		\brief Gets the index of the last cash flow (either principal, interest or deferred interest if IncludeDeferred is true)
		\arg IncludeDeferred determines if the accrued and deferred interest is considered as a cash flow or not
		\return The index of the last cash flow or -1 if there are no positive cash flows.
		\sa GetLastFlowDate()
		\sa GetLastFlowDate(bool)
		\sa GetLastFlowIndex()
		*/
		int GetLastFlowIndex(bool IncludeDeferred) { return Unmanaged->GetLastFlowIndex(IncludeDeferred); }
		/*!
		\brief The initial amount outstanding of the tranche
		*/
		property double InitialOutstanding{
			void set(double a){Unmanaged->SetInitialOutstanding(a);}
			double get(){return Unmanaged->GetInitialOutstanding();}
		}
		/*!
		\brief Adds a flow to the series.
		\arg Dte The date of the flow.
		\arg Amt The amount of the flow.
		\arg FlwTpe The type of flow
		\details Adds to the series at date Dte a flow of type FlwTpe. If at the specified date another amount of the same type is already in the series Amt will be added to it.
		\sa AddFlow(ManTrancheCashFlow^)
		*/
		void AddFlow(DateTime Dte,double Amt, ManTrancheFlowType FlwTpe){
			Unmanaged->AddFlow(DateTime2QDate(Dte),Amt,static_cast<TrancheCashFlow::TrancheFlowType>(static_cast<int>(FlwTpe)));
		}
		/*!
		\brief Adds flows to the series.
		\arg a The source of the flows
		\details Joins two ManTrancheCashFlow into one another.
		\sa AddFlow(DateTime,double,ManMtgFlowType)
		*/
		void AddFlow(ManTrancheCashFlow^ a){Unmanaged->AddFlow(*(a->Unmanaged));}
		/*!
		\brief Deletes all the flows in the series.
		*/
		void ResetFlows(){Unmanaged->ResetFlows();}
		/*!
		\brief Copy another ManTrancheCashFlow
		*/
		void Copy(ManTrancheCashFlow^ a){Unmanaged->operator=(*(a->Unmanaged));}
		/*!
		\brief Replace a date in the series
		\arg OriginalDate date to find
		\arg NewDate date to replace with
		\details If there is a flow happening in date OriginalDate the date will be replaced with NewDate<br/>If OriginalDate is not in the series nothing gets done.
		*/
		void ReplaceDate(DateTime OriginalDate, DateTime NewDate){Unmanaged->ReplaceDate(DateTime2QDate(OriginalDate),DateTime2QDate(NewDate));}

		ManTrancheCashFlow(){Unmanaged=new TrancheCashFlow();}
		//! Copy constructor
		ManTrancheCashFlow(const ManTrancheCashFlow% a){Unmanaged=new TrancheCashFlow(*(a.Unmanaged));}
		//! Copy constructor for ref class
		ManTrancheCashFlow(ManTrancheCashFlow^ a){Unmanaged=new TrancheCashFlow(*(a->Unmanaged));}
		!ManTrancheCashFlow()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManTrancheCashFlow()
		{
			this->!ManTrancheCashFlow();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManTrancheCashFlow_h__
