#ifndef ManBaseRateVector_h__
#define ManBaseRateVector_h__
#include "BaseRateVect.h"
#include <QByteArray>
#include <QDataStream>
#include "ManagedCommons.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Base rate scenario vector.
	\details This class handles vectors that follow the Bloomberg conventions in SCEN \<GO\> but accepting index rates as levels instead of numbers.<br/>Given the nature of the data, ramps are not allowed.<br/>Supported base rates are described in the ManAvailableRates enumerator.
	 */
	public ref class ManBaseRateVector
	{
	private:
		static const unsigned int ClassIdentity=10;
	internal:
		BaseRateVector* Unmanaged;
		ManBaseRateVector(const BaseRateVector& a){Unmanaged=new BaseRateVector(a);}
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
		\brief Gets the string representation of the vector value for the specified date
		\arg index the date for which to get the value of the vector
		\details Returns the string representation of the value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, ManAvailableRates::Invalid is returned.
		\sa GetValueString(int)
		\sa GetValue(int)
		\sa GetValue(DateTime)
		*/
		String^ GetValueString(DateTime index){return QString2String(Unmanaged->GetValue(DateTime2QDate(index)));}
		/*! 
		\brief Gets the string representation of the vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\details Returns the string representation of the value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, ManAvailableRates::Invalid is returned.
		\sa GetValueString(DateTime)
		\sa GetValue(DateTime)
		\sa GetValue(int)
		*/
		String^ GetValueString(int index){return QString2String(Unmanaged->GetValue(index));}
		/*! 
		\brief Returns a Bloomberg vector containing the value of the base indexes represented in this vector using Values as a source
		\arg Values a table containing the relevant index values
		\details This function will use the values supplied in the table as the values for the relevant base index rates.<br/>If a base index is required by the model but not supplied it will be downloaded from Bloomberg using GetRefRateValueFromBloomberg and added to the Values table.
		\sa GetRefRateValueFromBloomberg
		\sa GetBaseRatesDatabase
		*/
		ManBloombergVector^ CompileReferenceRateValue(ManConstBaseRateTable^ Values){
			ConstantBaseRateTable FillValues = *(Values->Unmanaged);
			ManBloombergVector^ Result = gcnew ManBloombergVector(Unmanaged->CompileReferenceRateValue(FillValues));
			Values = gcnew ManConstBaseRateTable(FillValues);
			return Result;
		}
		/*!
		\brief Returns a Bloomberg vector containing the value of the base indexes represented in this vector using Values as a source
		\arg Values a table containing the relevant index forward curves
		\details This function will use the forward curves supplied in the table as the values for the relevant base index rates.<br/>If a base index is required by the model but not supplied its spot rate will be downloaded from Bloomberg using GetRefRateValueFromBloomberg and added to the Values table.
		\sa GetRefRateValueFromBloomberg
		\sa GetBaseRatesDatabase
		*/
		ManBloombergVector^ CompileReferenceRateValue(ManForwBaseRateTable^ Values) {
			ForwardBaseRateTable FillValues = *(Values->Unmanaged);
			ManBloombergVector^ Result = gcnew ManBloombergVector(Unmanaged->CompileReferenceRateValue(FillValues));
			Values = gcnew ManForwBaseRateTable(FillValues);
			return Result;
		}
#ifndef NO_DATABASE
		/*!
		\brief Returns a Bloomberg vector containing the value of the base indexes represented in this vector using the database as a source
		\arg Values A dictionary of overrides for specific indexes. It must contain the Bloomberg tickers (without yellow keys) of the relevant base index as key and it's value as as value.
		\arg DownloadAll If set to true it will download all rates from the database instead of just the needed ones. Normally set to false. If Values is used for more than one model using true may result in more efficiency
		\details This function will download the base rates values from the database.<br/>The Values table can be used to override specific base rates.<br/>If a base index is required by the model but not supplied it will be downloaded from Bloomberg.<br/>All downloaded values will be added to the Values table.
		\note This function will not be available if the NO_DATABASE symbol is defined at compile time
		\sa GetRefRateValueFromBloomberg
		\sa CompileReferenceRateValue
		*/
		ManBloombergVector^ GetBaseRatesDatabase(ManConstBaseRateTable^ Values, bool DownloadAll) {
			ConstantBaseRateTable FillValues = *(Values->Unmanaged);
			ManBloombergVector^ Result = gcnew ManBloombergVector(Unmanaged->GetBaseRatesDatabase(FillValues, DownloadAll));
			Values = gcnew ManConstBaseRateTable(FillValues);
			return Result;
		}
		/*!
		\brief Returns a Bloomberg vector containing the value of the base indexes represented in this vector using the database as a source
		\arg Values A table of overrides for specific indexes.
		\arg DownloadAll If set to true it will download all rates from the database instead of just the needed ones. Normally set to false. If Values is used for more than one model using true may result in more efficiency
		\details This function will download the base rates forward curves from the database.<br/>The Values table can be used to override specific base rates.<br/>If a base index is required by the model but not supplied, its spot rate will be downloaded from Bloomberg.<br/>All downloaded values will be added to the Values table.
		\note This function will not be available if the NO_DATABASE symbol is defined at compile time
		\sa GetRefRateValueFromBloomberg
		\sa CompileReferenceRateValue
		*/
		ManBloombergVector^ GetBaseRatesDatabase(ManForwBaseRateTable^ Values, bool DownloadAll) {
			ForwardBaseRateTable FillValues = *(Values->Unmanaged);
			ManBloombergVector^ Result = gcnew ManBloombergVector(Unmanaged->GetBaseRatesDatabase(FillValues, DownloadAll));
			Values = gcnew ManForwBaseRateTable(FillValues);
			return Result;
		}
#endif
#ifndef NO_BLOOMBERG
		/*!
		\brief Returns a Bloomberg vector containing the value of the base indexes represented in this vector using Bloomberg as a source
		\arg Values A table of overrides for specific indexes.
		\details This function will use Bloomberg to fetch the values of the Indexes represented in the vector.<br/>Downloaded values will be added to the Values table too.
		\note This function will not be available if the NO_BLOOMBERG symbol is defined at compile time
		\sa CompileReferenceRateValue
		\sa GetBaseRatesDatabase
		*/
		ManBloombergVector^ GetRefRateValueFromBloomberg(ManConstBaseRateTable^ Values) {
			ConstantBaseRateTable FillValues = *(Values->Unmanaged);
			ManBloombergVector^ Result = gcnew ManBloombergVector(Unmanaged->GetRefRateValueFromBloomberg(FillValues));
			Values = gcnew ManConstBaseRateTable(FillValues);
			return Result;
		}
#endif 

		ManBaseRateVector(){Unmanaged=new BaseRateVector();}
		//! Copy constructor
		ManBaseRateVector(const ManBaseRateVector% a){Unmanaged=new BaseRateVector(*(a.Unmanaged));}
		//! Copy constructor for ref class
		ManBaseRateVector(ManBaseRateVector^ a){Unmanaged=new BaseRateVector(*(a->Unmanaged));}
		/*! 
		\brief Constructor taking the vector string
		\details If the supplied vector is not a valid Base Rate vector the class will default to an empty vector
		*/
		ManBaseRateVector(String^ Vec){Unmanaged=new BaseRateVector(String2QString(Vec));}
		/*! 
		\brief Constructor taking the vector string and the anchor date
		\details If the supplied vector is not a valid Base Rate vector the class will default to an empty vector.<br/>If the vector string starts with an anchor date, it will be overwritten by the supplied one.
		*/
		ManBaseRateVector(String^ Vec,DateTime Anchor){Unmanaged=new BaseRateVector(String2QString(Vec),DateTime2QDate(Anchor));}
		
		/*! 
		\brief Tries to set the vector to the supplied string
		\arg Vec The string containing the Base Rate vector
		\return Whether the vector was changed or not
		\details This function will try to set the vector to represent the supplied string, if it's not a valid Base Rate vector string the vector won't be changed and false is returned
		*/
		bool SetVector(String^ Vec){return Unmanaged->SetVector(String2QString(Vec));}
		/*! 
		\brief Set the anchor date of the vector
		\arg Anchor the new anchor date
		\details This function will set the anchor date of the vector to the supplied one overwriting any previous anchor date specified either manually of at the beginning of the vector string.
		*/
		void SetAnchorDate(DateTime Anchor){Unmanaged->SetAnchorDate(DateTime2QDate(Anchor));}
		/*! 
		\brief Resets anchor date of the vector
		\details This function will reset the anchor date of the vector deleting any reference to any previous anchor date specified either manually of at the beginning of the vector string.
		*/
		void RemoveAnchorDate(){Unmanaged->RemoveAnchorDate();}
		/*! 
		\brief Gets the string representation of the vector
		\details This function will return the string representation of the vector. If the anchor date is specified, it will be prepended to the vector in the format "A mm/dd/yyyy"
		*/
		String^ GetVector() {return QString2String(Unmanaged->GetVector());}
		/*! 
		\brief Gets the anchor date of the vector
		*/
		DateTime GetAnchorDate() {return QDate2DateTime(Unmanaged->GetAnchorDate());}
		/*! 
		\brief Asserts either the vector is empty or not
		*/
		bool IsEmpty(){return Unmanaged->IsEmpty();}
		/*! 
		\brief Number of elements in the vector
		\details This will return the number of monthly elements in the vector. It will be 1 plus the number of steps in the vector.<br/>e.g.:
		<ul>
		<li>EUR003M has 1 step</li>
		<li>EUR003M 12S EUR006M has 13 step</li>
		</ul>
		*/
		int NumElements() {return Unmanaged->NumElements();}
		!ManBaseRateVector()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManBaseRateVector()
		{
			this->!ManBaseRateVector();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManBaseRateVector_h__