#ifndef ManBaseRateTable_h__
#define ManBaseRateTable_h__
#include "ManagedCommons.h"
#include <QList>
#include "BaseRateTable.h"
#include "ManBloombergVector.h"
namespace ManagedCLO {
	/*!
	\brief Class that describes container for base rates
	\details This class is used wrap and manage base rates spot values.
	*/
	public ref class ManConstBaseRateTable {
	private:
		static const unsigned int ClassIdentity = 14;
	internal:
		ConstantBaseRateTable* Unmanaged;
		ManConstBaseRateTable(const ConstantBaseRateTable& a) { Unmanaged = new ConstantBaseRateTable(a); }
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
				for (unsigned int i = 0; i < BlockSize; i++)DataBuffer[i] = DataRaw.at(i);
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
				if (BlockSize >> 16 != ClassIdentity || (BlockSize & 0xFFFF) < MinSupportedVersion || (BlockSize & 0xFFFF) > VersionNumber) return false;
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
		\brief Determines if the specified base rate is included in the table
		*/
		bool Contains(String^ key)  { return Unmanaged->Contains(String2QString(key)); }
		/*!
		\brief Returns the spot rate for the specified base index or -1 if it's not found in the table
		*/
		double GetValue(String^ key)  { return Unmanaged->GetValue(String2QString(key)); }
		/*!
		\brief Sets the value for the specified rate.
		\details If the base rate it's included in the table it's value will be ovewritten by the new one, otherwise it will be added to the table.
		*/
		void SetValue(String^ key, double a) { return Unmanaged->SetValue(String2QString(key), a); }
		//ManBloombergVector GetValue(String^ key) const { return gcnew ManBloombergVector(Unmanaged->GetValue(String2QString(key))); }

		/*!
		\brief The date in which the rates were last updated.
		*/
		property DateTime UpdateDate{
			DateTime get() { return QDate2DateTime(Unmanaged->GetUpdateDate()); }
			void set(DateTime val) { Unmanaged->SetUpdateDate(DateTime2QDate(val)); }
		}
		/*!
		\brief The number of base rates in the table.
		*/
		property int CountKeys {
			int get() { return Unmanaged->GetAvailableKeys().size(); }
		}
		/*!
		\brief Returns the name of the base rate at position index.
		\details It will return an empty string if index is negative or greater than the number of rates in the table
		*/
		String^ KeyAt(int index) { QList<QString> Result = Unmanaged->GetAvailableKeys(); if (index < 0 || index >= Result.size()) return ""; return QString2String(Result.at(index)); }

		ManConstBaseRateTable() { Unmanaged = new ConstantBaseRateTable(); }
		//! Copy constructor
		ManConstBaseRateTable(const ManConstBaseRateTable% a) { Unmanaged = new ConstantBaseRateTable(*(a.Unmanaged)); }
		//! Copy constructor for ref class
		ManConstBaseRateTable(ManConstBaseRateTable^ a) { Unmanaged = new ConstantBaseRateTable(*(a->Unmanaged)); }
		!ManConstBaseRateTable() {
			if (Unmanaged) {
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManConstBaseRateTable() {
			this->!ManConstBaseRateTable();
			GC::SuppressFinalize(this);
		}
	};

	/*!
	\brief Class that describes container for forward base rates
	\details This class is used wrap and manage base rates forward curves.
	*/
	public ref class ManForwBaseRateTable {
	private:
		static const unsigned int ClassIdentity = 15;
	internal:
		ForwardBaseRateTable* Unmanaged;
		ManForwBaseRateTable(const ForwardBaseRateTable& a) { Unmanaged = new ForwardBaseRateTable(a); }
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
				for (unsigned int i = 0; i < BlockSize; i++)DataBuffer[i] = DataRaw.at(i);
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
				if (BlockSize >> 16 != ClassIdentity || (BlockSize & 0xFFFF) < MinSupportedVersion || (BlockSize & 0xFFFF) > VersionNumber) return false;
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
		\brief Determines if the specified base rate is included in the table
		*/
		bool Contains(String^ key)  { return Unmanaged->Contains(String2QString(key)); }
		/*!
		\brief Returns the spot rate for the specified base index or -1 if it's not found in the table
		*/
		ManBloombergVector^ GetValue(String^ key)  { return gcnew ManBloombergVector(Unmanaged->GetValue(String2QString(key))); }
		/*!
		\brief Sets the value for the specified rate.
		\details If the base rate it's included in the table it's value will be overwritten by the new one, otherwise it will be added to the table.
		*/
		void SetValue(String^ key, ManBloombergVector^ a) { return Unmanaged->SetValue(String2QString(key), *(a->Unmanaged)); }
		

		/*!
		\brief The date in which the rates were last updated.
		*/
		property DateTime UpdateDate{
			DateTime get() { return QDate2DateTime(Unmanaged->GetUpdateDate()); }
			void set(DateTime val) { Unmanaged->SetUpdateDate(DateTime2QDate(val)); }
		}
		/*!
		\brief The number of base rates in the table.
		*/
		property int CountKeys {
			int get() { return Unmanaged->GetAvailableKeys().size(); }
		}
		/*!
		\brief Returns the name of the base rate at position index.
		\details It will return an empty string if index is negative or greater than the number of rates in the table
		*/
		String^ KeyAt(int index) { QList<QString> Result = Unmanaged->GetAvailableKeys(); if (index < 0 || index >= Result.size()) return ""; return QString2String(Result.at(index)); }

		ManForwBaseRateTable() { Unmanaged = new ForwardBaseRateTable(); }
		//! Copy constructor
		ManForwBaseRateTable(const ManForwBaseRateTable% a) { Unmanaged = new ForwardBaseRateTable(*(a.Unmanaged)); }
		//! Copy constructor for ref class
		ManForwBaseRateTable(ManForwBaseRateTable^ a) { Unmanaged = new ForwardBaseRateTable(*(a->Unmanaged)); }
		!ManForwBaseRateTable() {
			if (Unmanaged) {
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManForwBaseRateTable() {
			this->!ManForwBaseRateTable();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManBaseRateTable_h__