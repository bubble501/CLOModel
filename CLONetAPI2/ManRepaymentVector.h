#ifndef ManRepaymentVector_h__
#define ManRepaymentVector_h__
#include "RepaymentVector.h"
#include <QByteArray>
#include <QDataStream>
#include "ManagedCommons.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Repayment scenario vector.
	\details This class handles vectors that follow the Bloomberg conventions in SCEN \<GO\> but accepting repayment scenarios as levels instead of numbers.<br/>Given the nature of the data, ramps are not allowed.<br/>Supported repayment schemes are described in the ManRepaymentMethods enumerator.
	*/
	public ref class ManRepaymentVector {
	private:
		static const unsigned int ClassIdentity = 12;
	internal:
		RepaymentVector* Unmanaged;
		ManRepaymentVector(const RepaymentVector& a) { Unmanaged = new RepaymentVector(a); }
	public:
		/*! Enum defining the different types of repayments supported.*/
		enum class ManRepaymentMethods : int {
			Invalid = -1		/*!< Invalid Repayment Type*/
			, InterestOnly		/*!< Interest Only Repayment Scheme. All principal is paid at maturity in a single bullet<br/>Value in the vector: I*/
			, Annuity			/*!< Annuity Repayment Scheme. Principal and interest are repaid keeping the monthly installment constant<br/>Value in the vector: A*/
			, Linear			/*!< Linear Repayment Scheme. The principal part is kept constant, the total installment declines through time.<br/>Value in the vector: L*/
			, Capitalization	/*!< Capitalization of Interest Repayment Scheme. No cash is paid out during the life of the loan, principal and capitalized interest are both paid at maturity<br/>Value in the vector: C*/
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
		\brief Gets the vector value for the specified date
		\arg index the date for which to get the value of the vector
		\details Returns the value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, ManRepaymentMethods::Invalid is returned.
		\sa GetValue(int)
		\sa GetValueString(DateTime)
		\sa GetValueString(int)
		*/
		ManRepaymentMethods GetValue(DateTime index) { return static_cast<ManRepaymentMethods>(static_cast<int>(Unmanaged->GetValue(DateTime2QDate(index)))); }
		/*!
		\brief Gets the vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\details Returns the value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, ManRepaymentMethods::Invalid is returned.
		\sa GetValue(DateTime)
		\sa GetValueString(DateTime)
		\sa GetValueString(int)
		*/
		ManRepaymentMethods GetValue(int index) { return static_cast<ManRepaymentMethods>(static_cast<int>(Unmanaged->GetValue(index))); }
		/*!
		\brief Gets the string representation of the vector value for the specified date
		\arg index the date for which to get the value of the vector
		\details Returns the string representation of the value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, ManRepaymentMethods::Invalid is returned.
		\sa GetValueString(int)
		\sa GetValue(int)
		\sa GetValue(DateTime)
		*/
		String^ GetValueString(DateTime index) { return QString2String(Unmanaged->GetValueString(DateTime2QDate(index))); }
		/*!
		\brief Gets the string representation of the vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\details Returns the string representation of the value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, ManRepaymentMethods::Invalid is returned.
		\sa GetValueString(DateTime)
		\sa GetValue(DateTime)
		\sa GetValue(int)
		*/
		String^ GetValueString(int index) { return QString2String(Unmanaged->GetValueString(index)); }

		ManRepaymentVector() { Unmanaged = new RepaymentVector(); }
		//! Copy constructor
		ManRepaymentVector(const ManRepaymentVector% a) { Unmanaged = new RepaymentVector(*(a.Unmanaged)); }
		//! Copy constructor for ref class
		ManRepaymentVector(ManRepaymentVector^ a) { Unmanaged = new RepaymentVector(*(a->Unmanaged)); }
		/*!
		\brief Constructor taking the vector string
		\details If the supplied vector is not a valid repayment vector the class will default to an empty vector
		*/
		ManRepaymentVector(String^ Vec) { Unmanaged = new RepaymentVector(String2QString(Vec)); }
		/*!
		\brief Constructor taking the vector string and the anchor date
		\details If the supplied vector is not a valid repayment vector the class will default to an empty vector.<br/>If the vector string starts with an anchor date, it will be overwritten by the supplied one.
		*/
		ManRepaymentVector(String^ Vec, DateTime Anchor) { Unmanaged = new RepaymentVector(String2QString(Vec), DateTime2QDate(Anchor)); }

		/*!
		\brief Tries to set the vector to the supplied string
		\arg Vec The string containing the repayment methods vector
		\return Whether the vector was changed or not
		\details This function will try to set the vector to represent the supplied string, if it's not a valid repayment vector string the vector won't be changed and false is returned
		*/
		bool SetVector(String^ Vec) { return Unmanaged->SetVector(String2QString(Vec)); }
		/*!
		\brief Set the anchor date of the vector
		\arg Anchor the new anchor date
		\details This function will set the anchor date of the vector to the supplied one overwriting any previous anchor date specified either manually of at the beginning of the vector string.
		*/
		void SetAnchorDate(DateTime Anchor) { Unmanaged->SetAnchorDate(DateTime2QDate(Anchor)); }
		/*!
		\brief Resets anchor date of the vector
		\details This function will reset the anchor date of the vector deleting any reference to any previous anchor date specified either manually of at the beginning of the vector string.
		*/
		void RemoveAnchorDate() { Unmanaged->RemoveAnchorDate(); }
		/*!
		\brief Gets the string representation of the vector
		\details This function will return the string representation of the vector. If the anchor date is specified, it will be prepended to the vector in the format "A mm/dd/yyyy"
		*/
		String^ GetVector() { return QString2String(Unmanaged->GetVector()); }
		/*!
		\brief Gets the anchor date of the vector
		*/
		DateTime GetAnchorDate() { return QDate2DateTime(Unmanaged->GetAnchorDate()); }
		/*!
		\brief Asserts either the vector is empty or not
		*/
		bool IsEmpty() { return Unmanaged->IsEmpty(); }
		/*!
		\brief Number of elements in the vector
		\details This will return the number of monthly elements in the vector. It will be 1 plus the number of steps in the vector.<br/>e.g.:
		<ul>
		<li>I has 1 step</li>
		<li>I 12S A has 13 steps</li>
		</ul>
		*/
		int NumElements() { return Unmanaged->NumElements(); }
		!ManRepaymentVector() {
			if (Unmanaged) {
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManRepaymentVector() {
			this->!ManRepaymentVector();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManRepaymentVector_h__