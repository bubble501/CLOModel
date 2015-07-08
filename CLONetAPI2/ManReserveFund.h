#ifndef ManReserveFund_h__
#define ManReserveFund_h__
#include "ManagedCommons.h"
#include "ManGenericCashFlow.h"
#include "ReserveFund.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Reserve Fund
	\details This class implements a way of manage a reserve fund.<br/>The cash flows are stored so that the interest and principal flows will distinguish whether the reserve fund was replenished using interest or principal proceeds and the deferred flows will represent shortfall in the reserve fund compared to the target level for a given period.
	*/
	public ref class ManReserveFund {
	private:
		static const unsigned int ClassIdentity = 13;
	internal:
		ReserveFund* Unmanaged;
		ManReserveFund(const ReserveFund& a) { Unmanaged = new ReserveFund(a); }
	public:
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
		\brief The target amount of the reserve fund
		\details This property will hold the Bloomberg vector representing the target level of the reserve fund
		\sa SetReserveFund
		*/
		property String^ ReserveFundTarget{
			String^ get() { return QString2String(Unmanaged->GetReserveFundTargetString()); }
			void set(String^ a) { Unmanaged->SetReserveFundTarget(String2QString(a)); }
		}
		/*!
		\brief The multiple of the target amount of the reserve fund
		\details This property will hold the Bloomberg vector representing the multiple of the target level of the reserve fund
		\sa SetReserveFund
		*/
		property String^ ReserveFundMultiple{
			String^ get() { return QString2String(Unmanaged->GetReserveFundMultipleString()); }
			void set(String^ a) { Unmanaged->SetReserveFundMultiple(String2QString(a)); }
		}
		/*!
		\brief The floor amount of the reserve fund
		\details This property will hold the Bloomberg vector representing the floor of the reserve fund
		\sa SetReserveFund
		*/
		property String^ ReserveFundFloor {
			String^ get() { return QString2String(Unmanaged->GetReserveFundFloorString()); }
			void set(String^ a) { Unmanaged->SetReserveFundFloor(String2QString(a)); }
		}
		/*!
		\brief The cap amount of the reserve fund
		\details This property will hold the Bloomberg vector representing the cap of the reserve fund
		\sa SetReserveFund
		*/
		property String^ ReserveFundCap {
			String^ get() { return QString2String(Unmanaged->GetReserveFundCapString()); }
			void set(String^ a) { Unmanaged->SetReserveFundCap(String2QString(a)); }
		}


		/*!
		\brief The current amount of the reserve fund
		\details This property will hold the value of the current amount of the reserve fund
		\sa SetReserveFund
		*/
		property double ReserveFundCurrent{
			double get(){ return Unmanaged->GetReserveFundCurrent(); }
			void set(double a) { Unmanaged->SetReserveFundCurrent(a); }
		}
		/*!
		\brief The seniority level after which the reserve is released
		\details This property will hold the seniority level that is used to determine when the reserve fund is released.<br/>After all the notes of this seniority level and any note senior to these have redeemed the reserve will be released in the interest waterfall.<br/>A value of 0 means that the reserve will be release only at maturity.
		\sa SetReserveFund
		*/
		property int ReserveFundFreed {
			int get(){ return Unmanaged->GetReserveFundFreed(); }
			void set(int a) { return Unmanaged->SetReserveFundFreed(a); }
		}
		/*!
		\brief The cash flows toward the reserve fund
		\details This property will hold the cash flows toward the replenishment of the reserve fund.<br/>The interest and principal flows will distinguish whether the reserve fund was replenished using interest or principal proceeds.<br/>Use ManTrancheCashFlow::GetTotalFlow to get the level of the reserve fund in a given period.<br/>Use ManTrancheCashFlow::GetDeferred to get the shortfall in the reserve fund compared to the target level for a given period.
		\sa SetReserveFund
		*/
		property ManGenericCashFlow^ ReserveFundFlow {
			ManGenericCashFlow^ get() { return gcnew ManGenericCashFlow(Unmanaged->GetReserveFundFlow()); }
			void set(ManGenericCashFlow^ source) { Unmanaged->SetReserveFundFlow(*(source->Unmanaged)); }
		}
		/*!
		\brief Whether the reserve will fund the interest or principal waterfall
		\details This property will hold true if the reserve fund will be part of interest proceeds and false if it will form part of principal proceeds or index is invalid.
		\sa SetReserveFund
		*/
		property bool GetReserveToInterest{
			bool get(){ return Unmanaged->GetReserveToInterest(); }
			void set(bool a) { Unmanaged->SetReserveToInterest(a); }
		}
		/*!
		\brief Set up a reserve fund
		\arg RFtarget a Bloomberg vector representing the target level of the reserve fund.
		\arg RFmultiple a Bloomberg vector representing the value by which RFtarget will be multiplied.
		\arg RFfloor a Bloomberg vector representing the floor level of the reserve fund. For non-amortizing RFs set to "0"
		\arg RFcap a Bloomberg vector representing the cap level of the reserve fund. For non-capped RFs set to "0"
		\arg RFcurrent the current size of the reserve fund
		\arg RFfreed A seniority level. When all the notes not junior to this seniority level will be redeemed, the RF will be released as excess spread. 0 for freeing it only at maturity
		\arg RFtoInterest Set to true if the reserve fund will be part of interest proceeds when calculating the waterfall, set to false if it's part of principal proceeds.
		\details This function will set up the specified reserve fund according to the supplied parameters.<br/>If RFtarget is a seniority level, the reserve fund target amount will be the sum of all the notes that rank not junior to that level, otherwise the reserve will be a non amortizing.<br/>The target level will then be multiplied by RFmultiple to assess the actual replenishment target.<br/>In any case, the target won't be set below the RFfloor amount.
		*/
		void SetReserveFund(String^ RFtarget, String^ RFmultiple, String^ RFfloor, String^ RFcap, double RFcurrent, int RFfreed, bool RFtoInterest) { Unmanaged->SetReserveFund(String2QString(RFtarget), String2QString(RFmultiple), String2QString(RFfloor), String2QString(RFcap), RFcurrent, RFfreed, RFtoInterest); }
		/*!
		\brief Removes all the settings for the specified reserve fund
		\details This function will remove all the settings and cash flows relative to the specified reserve fund.
		*/
		void ResetReserveFund() { Unmanaged->ResetReserve(); }
		
		ManReserveFund() { Unmanaged = new ReserveFund(); }
		//! Copy constructor
		ManReserveFund(const ManReserveFund% a) { Unmanaged = new ReserveFund(*(a.Unmanaged)); }
		//! Copy constructor for ref class
		ManReserveFund(ManReserveFund^ a) { Unmanaged = new ReserveFund(*(a->Unmanaged)); }
		!ManReserveFund() {
			if (Unmanaged) {
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManReserveFund() {
			this->!ManReserveFund();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManReserveFund_h__