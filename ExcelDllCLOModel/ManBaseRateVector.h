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
	\details This class handles vectors that follow the Bloomberg conventions in SCEN <GO> but accepting index rates as levels instead of numbers.<br/>Given the nature of the data, ramps are not allowed.<br/>Supported base rates are described in the ManAvailableRates enumerator.
	 */
	public ref class ManBaseRateVector
	{
	private:
		static const unsigned int ClassIdentity=10;
	internal:
		BaseRateVector* Unmanaged;
		ManBaseRateVector(const BaseRateVector& a){Unmanaged=new BaseRateVector(a);}
	public:			
		/*! Enum defining the different types of base rates supported.*/
		enum class ManAvailableRates : int{
			Invalid=-1, /*!< Invalid Index Rate*/
			EUR001W, /*!< 1 Week EURIBOR in Euros*/
			EUR002W, /*!< 2 Weeks EURIBOR in Euros*/
			EUR003W, /*!< 3 Weeks EURIBOR in Euros*/
			EUR001M, /*!< 1 Month EURIBOR in Euros*/
			EUR002M, /*!< 2 Months EURIBOR in Euros*/
			EUR003M, /*!< 3 Months EURIBOR in Euros*/
			EUR004M, /*!< 4 Months EURIBOR in Euros*/
			EUR005M, /*!< 5 Months EURIBOR in Euros*/
			EUR006M, /*!< 6 Months EURIBOR in Euros*/
			EUR007M, /*!< 7 Months EURIBOR in Euros*/
			EUR008M, /*!< 8 Months EURIBOR in Euros*/
			EUR009M, /*!< 9 Months EURIBOR in Euros*/
			EUR010M, /*!< 10 Months EURIBOR in Euros*/
			EUR011M, /*!< 11 Months EURIBOR in Euros*/
			EUR012M, /*!< 12 Months EURIBOR in Euros*/
			BP00ON, /*!< Overnight LIBOR in Pound sterling*/
			BP0001W, /*!< 1 Week LIBOR in Pound sterling*/
			BP0001M, /*!< 1 Month LIBOR in Pound sterling*/
			BP0002M, /*!< 2 Months LIBOR in Pound sterling*/
			BP0003M, /*!< 3 Months LIBOR in Pound sterling*/
			BP0006M, /*!< 6 Months LIBOR in Pound sterling*/
			BP0012M, /*!< 12 Months LIBOR in Pound sterling*/
			US00ON, /*!< Overnight LIBOR in US Dollars*/
			US0001W, /*!< 1 Week LIBOR in US Dollars*/
			US0001M, /*!< 1 Month LIBOR in US Dollars*/
			US0002M, /*!< 2 Months LIBOR in US Dollars*/
			US0003M, /*!< 3 Months LIBOR in US Dollars*/
			US0006M, /*!< 6 Months LIBOR in US Dollars*/
			US0012M, /*!< 12 Months LIBOR in US Dollars*/
			LIBOR01W, /*!< 1 Week LIBOR in US Dollars*/
			LIBOR01M, /*!< 1 Month LIBOR in US Dollars*/
			LIBOR02M, /*!< 2 Months LIBOR in US Dollars*/
			LIBOR03M, /*!< 3 Months LIBOR in US Dollars*/
			LIBOR06M, /*!< 6 Months LIBOR in US Dollars*/
			LIBOR12M, /*!< 12 Months LIBOR in US Dollars*/
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
		\warning If the first element in the stream is not of this class, the stream will be advanced by 4 bytes anyway
		\return Whether the read operation was successful
		*/
		bool Read(IO::Stream^ Source){
			if(!Source->CanRead) return false;
			quint32 BlockSize;
			quint32 MagicNumber=((ClassIdentity << 16) | VersionNumber);
			try{
				IO::BinaryReader SizeReader(Source);
				BlockSize=SizeReader.ReadUInt32();
				if(BlockSize!=MagicNumber) return false;
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
		\brief Gets the vector value for the specified date
		\arg index the date for which to get the value of the vector
		\details Returns the value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, ManAvailableRates::Invalid is returned.
		\sa GetValue(int)
		\sa GetValueString(DateTime)
		\sa GetValueString(int)
		*/
		ManAvailableRates GetValue(DateTime index) {return static_cast<ManAvailableRates>(static_cast<int>(Unmanaged->GetValue(DateTime2QDate(index))));}
		/*! 
		\brief Gets the vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\details Returns the value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, ManAvailableRates::Invalid is returned.
		\sa GetValue(DateTime)
		\sa GetValueString(DateTime)
		\sa GetValueString(int)
		*/
		ManAvailableRates GetValue(int index) {return static_cast<ManAvailableRates>(static_cast<int>(Unmanaged->GetValue(index)));}
		/*! 
		\brief Gets the string representation of the vector value for the specified date
		\arg index the date for which to get the value of the vector
		\details Returns the string representation of the value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, ManAvailableRates::Invalid is returned.
		\sa GetValueString(int)
		\sa GetValue(int)
		\sa GetValue(DateTime)
		*/
		String^ GetValueString(DateTime index){return QString2String(Unmanaged->GetValueString(DateTime2QDate(index)));}
		/*! 
		\brief Gets the string representation of the vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\details Returns the string representation of the value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, ManAvailableRates::Invalid is returned.
		\sa GetValueString(DateTime)
		\sa GetValue(DateTime)
		\sa GetValue(int)
		*/
		String^ GetValueString(int index){return QString2String(Unmanaged->GetValueString(index));}
		/*! 
		\brief Returns a Bloomberg vector containing the value of the base indexes represented in this vector using Values as a source
		\arg Values a dictionary containing the Bloomberg tickers (without yellow keys) of the relevant base index as key and it's value as as value.
		\details This function will use the values supplied in the dictionary as the values for the relevant base index rates.<br/>If a base index is required by the model but not supplied it will be downloaded from Bloomberg using GetRefRateValueFromBloomberg.
		\sa GetRefRateValueFromBloomberg
		*/
		ManBloombergVector^ CompileReferenceRateValue(Collections::Generic::Dictionary<String^,double>^ Values){
			return gcnew ManBloombergVector(Unmanaged->CompileReferenceRateValue(Dictionary2QHash(Values)));
		}
		/*! 
		\brief Returns a Bloomberg vector containing the value of the base indexes represented in this vector using Bloomberg as a source
		\details This function will use Bloomberg to fetch the values of the Indexes represented in the vector.
		\sa CompileReferenceRateValue
		*/
		ManBloombergVector^ GetRefRateValueFromBloomberg(){return gcnew ManBloombergVector(Unmanaged->GetRefRateValueFromBloomberg());}


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