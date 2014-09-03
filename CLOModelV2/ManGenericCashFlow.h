#ifndef ManGenericCashFlow_h__
#define ManGenericCashFlow_h__
#include "GenericCashFlow.h"
#include <QByteArray>
#include <QDataStream>
#include "ManagedCommons.h"
#include "ManIntegerVector.h"
using namespace System;
//! Namespace containing all the classes of the CLO model
namespace ManagedCLO {
	/*!
	\brief Cash Flows
	\details This class implements a generic way of managing cash flows.<br/>The flows will always be sorted by date.
	*/
	public ref class ManGenericCashFlow {
	private:
		static const unsigned int ClassIdentity = 16;
	internal:
		GenericCashFlow* Unmanaged;
		ManGenericCashFlow(const GenericCashFlow& a) { Unmanaged = new GenericCashFlow(a); }
	public:
		/*! Enum defining the type of aggregation of the flow*/
		enum class ManCashFlowAggregation {
			NoAggregation /*!< Aggregation day by day*/
			, Weekly /*!< Aggregation by week as define din ISO 8601*/
			, Monthly  /*!< Aggregation by month*/
			, Quarterly /*!< Aggregation by quarter*/
			, SemiAnnually /*!< Aggregation by semester*/
			, Annually /*!< Aggregation by year*/
			, TotalAggragate /*!< All cash flows aggregated together*/
		};
		/*!
		\brief Write class to stream
		\arg Dest Stream to write on
		\details This function writes the class information to a Stream (e.g. FileStream or NetworkStream)
		\return whether the write operation was successful
		*/
		bool Write(IO::Stream^ Dest) {
			if (!Dest->CanWrite) return false;
			quint32 MagicNumber = ((ClassIdentity << 16) | VersionNumber);
			quint32 BlockSize;
			try {
				IO::BinaryWriter StreamWriter(Dest);
				QByteArray DataRaw;
				QDataStream out(&DataRaw, QIODevice::WriteOnly);
				out.setVersion(QDataStream::Qt_5_3);
				out << (*Unmanaged);
				BlockSize = DataRaw.size();
				StreamWriter.Write(MagicNumber);
				StreamWriter.Write(BlockSize);
				array<Byte>^ DataBuffer = gcnew array<Byte>(BlockSize);
				for (unsigned int i = 0; i<BlockSize; i++)DataBuffer[i] = DataRaw.at(i);
				StreamWriter.Write(DataBuffer, 0, BlockSize);
				return true;
			}
			catch (Exception^) {
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
		\brief Gets the date of a cash flow.
		\arg index The index of the cash flow for which the date must be retrieved.<br/>The first flow will be at index 0
		\return A DateTime object containing the relevant date.<br/>If index is out of range, a default DateTime is returned
		\details Returns the date of the cash flow at position index in the series.
		*/
		DateTime GetDate(int index) { return QDate2DateTime(Unmanaged->GetDate(index)); }
		/*!
		\brief Gets the cash flow.
		\arg index The index of the cash flow to be retrieved.<br/>The first flow will be at index 0
		\arg FlowTpe An identifier of the type of cash flow
		\return A double containing the relevant cash flow.<br/>If index is out of range or the FlowTpe is not found, 0 is returned
		\details Returns the cash flow at position index in the series.
		\sa GetFlow(DateTime)
		*/
		double GetFlow(int index, int FlowTpe) { return Unmanaged->GetFlow(index, FlowTpe); }
		/*!
		\brief Gets the cash flow.
		\arg index The date for which the cash flow must be retrieved.
		\arg FlowTpe An identifier of the type of cash flow
		\return A double containing the relevant cash flow.<br/>If index is out of range or the FlowTpe is not found, 0 is returned
		\details Returns the cash flow on the date index.
		\sa GetFlow(int)
		*/
		double GetFlow(DateTime index, int FlowTpe) { return Unmanaged->GetFlow(DateTime2QDate(index), FlowTpe); }
		/*!
		\brief Gets the previous cash flow.
		\arg index The of the cash flow to be retrieved will be index-1.<br/>The first flow will be at index 0
		\arg FlowTpe An identifier of the type of cash flow
		\return A double containing the relevant cash flow.<br/>If index is negative or the FlowTpe is not found, 0 is returned, if index is 0 or greater than Count, the last cash flow is returned.
		\details Returns the cash flow at position index-1 in the series.
		\sa GetPreviousFlow(DateTime)
		*/
		double GetPreviousFlow(int index, int FlowTpe) { return Unmanaged->GetFlow(index, FlowTpe); }
		/*!
		\brief Gets the previous cash flow.
		\arg index The date following the one of the cash flow that must be retrieved.
		\arg FlowTpe An identifier of the type of cash flow
		\return A double containing the relevant cash flow.<br/>If index is invalid of range or the FlowTpe is not found, 0 is returned, if index is valid but not found the last cash flow is returned.
		\details Returns the cash flow relating to the date before the one specified by index.
		\sa GetPreviousFlow(int)
		*/
		double GetPreviousFlow(DateTime index, int FlowTpe) { return Unmanaged->GetFlow(DateTime2QDate(index), FlowTpe); }
		/*!
		\brief The number of flows in the series.
		\details Read only property that returns the number of flows in the series
		*/
		property int Count{
			int get() { return Unmanaged->Count(); }
		}
		/*!
		\brief Finds the index corresponding to a certain date.
		\arg a The date to search
		\return An integer representing the index of the flow in the series.<br/>If the date is not found -1 is returned
		\details Returns the index that represents the position of a date in the series.
		*/
		int FindDate(DateTime a) { return Unmanaged->FindDate(DateTime2QDate(a)); }
		/*!
		\brief Adds a flow to the series.
		\arg Dte The date of the flow.
		\arg Amt The amount of the flow.
		\arg FlowTpe An identifier of the type of cash flow
		\details Adds to the series at date Dte a flow of type FlowTpe. If, at the specified date, another amount of the same type is already in the series Amt will be added to it.
		\sa AddFlow(ManGenericCashFlow^)
		*/
		void AddFlow(DateTime Dte, double Amt, int FlowTpe) { Unmanaged->AddFlow(DateTime2QDate(Dte), Amt, FlowTpe); }

		/*!
		\brief Adds flows to the series.
		\arg a A reference to the source of the flows
		\details Joins two ManGenericCashFlow into one another.
		\sa AddFlow(DateTime,double,int)
		*/
		void AddFlow(ManGenericCashFlow^ a) { Unmanaged->AddFlow(*(a->Unmanaged)); }
		/*!
		\brief Deletes all the flows in the series.
		*/
		void Clear() { Unmanaged->Clear(); }
		/*!
		\brief Estimates the payment frequency of the cash flows
		*/
		ManIntegerVector^ GetPaymentFrequency() { return gcnew ManIntegerVector(Unmanaged->GetPaymentFrequency()); }
		/*!
		\brief Returns the date of the last non-zero cash flow
		*/
		DateTime MaturityDate() { return QDate2DateTime(Unmanaged->MaturityDate()); }

		/*!
		\brief Replace a date in the cash flows
		\arg OriginalDate the date to find and replace.
		\arg NewDate the new value of the date
		*/
		void ReplaceDate(DateTime OriginalDate, DateTime NewDate) { Unmanaged->ReplaceDate(DateTime2QDate(OriginalDate), DateTime2QDate(NewDate)); }

		/*!
		\brief Aggregates the cash flows
		\arg Freq the kind of aggregation to carry out
		\details Aggregates the current cash flows based on the specified granularity.<br/>The earliest of dates in a single period will be used as the date of the aggregated flow.<br/>All future cash flows added will be aggregated accordingly.
		*/
		void Aggregate(ManCashFlowAggregation Freq) { Unmanaged->Aggregate(static_cast<GenericCashFlow::CashFlowAggregation>(static_cast<int>(Freq))); }

		//! Compares two cash flows
		bool operator==(ManGenericCashFlow^ a) { return Unmanaged->operator==(*(a->Unmanaged)); }
	
		ManGenericCashFlow() { Unmanaged = new GenericCashFlow(); }

		//! Copy constructor for ref class
		ManGenericCashFlow(ManGenericCashFlow^ a) { Unmanaged = new GenericCashFlow(*(a->Unmanaged)); }
		!ManGenericCashFlow() {
			if (Unmanaged) {
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManGenericCashFlow() {
			this->!ManGenericCashFlow();
			GC::SuppressFinalize(this);
		}
	};
}

#endif // ManGenericCashFlow_h__
