#ifndef ManIntegerVector_h__
#define ManIntegerVector_h__
#include "IntegerVector.h"
#include <QByteArray>
#include <QDataStream>
#include "ManagedCommons.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Bloomberg-like vector of integers
	\details This class manages vector that follow the convention of Bloomberg scenarios vector as in the SCEN \<GO\> screen.<br/>The values in the vector can only be integers and ramps will round the result to the nearest integer
	*/
	public ref class ManIntegerVector {
	private:
		static const unsigned int ClassIdentity = 11;
	internal:
		IntegerVector* Unmanaged;
		ManIntegerVector(const IntegerVector& a) { Unmanaged = new IntegerVector(a); }
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

		ManIntegerVector() { Unmanaged = new IntegerVector(); }
		//! Copy constructor
		ManIntegerVector(const ManIntegerVector% a) { Unmanaged = new IntegerVector(*(a.Unmanaged)); }
		//! Copy constructor for ref class
		ManIntegerVector(ManIntegerVector^ a) { Unmanaged = new IntegerVector(*(a->Unmanaged)); }
		/*!
		\brief Constructor taking the vector string
		\details If the supplied vector is not a valid Bloomberg vector the class will default to an empty vector
		*/
		ManIntegerVector(String^ Vec) { Unmanaged = new IntegerVector(String2QString(Vec)); }
		/*!
		\brief Constructor taking the vector string and the anchor date
		\details If the supplied vector is not a valid Bloomberg vector the class will default to an empty vector.<br/>If the vector string starts with an anchor date, it will be overwritten by the supplied one.
		*/
		ManIntegerVector(String^ Vec, DateTime Anchor) { Unmanaged = new IntegerVector(String2QString(Vec), DateTime2QDate(Anchor)); }
		/*!
		\brief Gets the vector value for the specified date
		\arg index the date for which to get the value of the vector
		\details Returns the value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, 0 is returned.
		\sa GetValue(int)
		*/
		double GetValue(DateTime index) { return Unmanaged->GetValue(DateTime2QDate(index)); }
		/*!
		\brief Gets the vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\details Returns the value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, 0 is returned.
		\sa GetValue(DateTime)
		*/
		double GetValue(int index) { return Unmanaged->GetValue(index); }
		/*!
		\brief Tries to set the vector to the supplied string
		\arg Vec The string containing the Bloomberg-like vector
		\return Whether the vector was changed or not
		\details This function will try to set the vector to represent the supplied string, if it's not a valid integers vector string the vector won't be changed and false is returned
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
		\brief Number of elements in the vector
		\details This will return the number of monthly elements in the vector. It will be 1 plus the number of steps and ramps in the vector.<br/>e.g.:
		<ul>
		<li>5 has 1 step</li>
		<li>5 12S 6 has 13 step</li>
		<li>5 12S 6 12R 7 has 25 step</li>
		</ul>
		*/
		int NumElements() { return Unmanaged->NumElements(); }
		/*!
		\brief Asserts either the vector is empty or not
		*/
		bool IsEmpty() { return Unmanaged->IsEmpty(); }
		!ManIntegerVector() {
			if (Unmanaged) {
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		/*!
		\brief Asserts either the vector is empty or not and if the values are in a certain band
		\details This function will return false if the vector is valid and all the values in it are in the closed interval from Lbound to Ubound
		*/
		bool IsEmpty(int Lbound, int Ubound) { return Unmanaged->IsEmpty(Lbound, Ubound); }
		~ManIntegerVector() {
			this->!ManIntegerVector();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManIntegerVector_h__
