#ifndef ManBloombergVector_h__
#define ManBloombergVector_h__
#include "BloombergVector.h"
#include <QByteArray>
#include <QDataStream>
#include "ManagedCommons.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Bloomberg scenarios vector
	\details This class manages vector that follow the convention of Bloomberg scenarios vector as in the SCEN \<GO\> screen.<br/>The values in the vector will be considered as percentages if not otherwise specified through SetDivisor
	 */
	public ref class ManBloombergVector
	{
	private:
		static const unsigned int ClassIdentity=8;
	internal:
		BloombergVector* Unmanaged;
		ManBloombergVector(const BloombergVector& a){Unmanaged=new BloombergVector(a);}
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
		
		ManBloombergVector(){Unmanaged=new BloombergVector();}
		//! Copy constructor
		ManBloombergVector(const ManBloombergVector% a){Unmanaged=new BloombergVector(*(a.Unmanaged));}
		//! Copy constructor for ref class
		ManBloombergVector(ManBloombergVector^ a){Unmanaged=new BloombergVector(*(a->Unmanaged));}
		/*! 
		\brief Constructor taking the vector string
		\details If the supplied vector is not a valid Bloomberg vector the class will default to an empty vector
		*/
		ManBloombergVector(String^ Vec){Unmanaged=new BloombergVector(String2QString(Vec));}
		/*! 
		\brief Constructor taking the vector string and the anchor date
		\details If the supplied vector is not a valid Bloomberg vector the class will default to an empty vector.<br/>If the vector string starts with an anchor date, it will be overwritten by the supplied one.
		*/
		ManBloombergVector(String^ Vec,DateTime Anchor){Unmanaged=new BloombergVector(String2QString(Vec),DateTime2QDate(Anchor));}
		/*!
		\brief Set the divisor for the value in the vector
		\details Sets the divisor by which the values in the vector will be divided. By default it's set to 100 so 1 12S 2 will be translated in 0.01 12S 0.02
		*/
		void SetDivisor(double NewDiv) { Unmanaged->SetDivisor(NewDiv); }
		/*! 
		\brief Gets the annualized vector value for the specified date
		\arg index the date for which to get the value of the vector
		\details Returns the annualized value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, 0 is returned.
		\sa GetValue(DateTime,int)
		\sa GetValue(int)
		\sa GetValue(int,int)
		\sa GetSMM(DateTime,int)
		\sa GetSMM(int,int)
		*/
		double GetValue(DateTime index){return Unmanaged->GetValue(DateTime2QDate(index));}
		/*! 
		\brief Gets the de-annualized vector value for the specified date
		\arg index the date for which to get the value of the vector
		\arg Frequency the frequency, in months, by which the vector should be de-annualized. 1 for monthly de-annualization, 3 for quarterly
		\details Returns the de-annualized value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, 0 is returned.
		\note This method will de-annualise the values based on the normal compound formula \f$(1+r)^{f/12}-1\f$ where r is the annual rate and f is the required frequency.<br/>If you want values to be treated using the SMM formula, use GetSMM() instead
		\sa GetValue(DateTime)
		\sa GetValue(int)
		\sa GetValue(int,int)
		\sa GetSMM(DateTime,int)
		\sa GetSMM(int,int)
		*/
		double GetValue(DateTime index,int Frequency){return Unmanaged->GetValue(DateTime2QDate(index),Frequency);}
		/*! 
		\brief Gets the annualized vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\details Returns the annualized value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, 0 is returned.
		\sa GetValue(int,int)
		\sa GetValue(DateTime)
		\sa GetValue(DateTime,int)
		\sa GetSMM(DateTime,int)
		\sa GetSMM(int,int)
		*/
		double GetValue(int index){return Unmanaged->GetValue(index);}
		/*! 
		\brief Gets the de-annualized vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\arg Frequency the frequency, in months, by which the vector should be de-annualized. 1 for monthly de-annualization, 3 for quarterly
		\details Returns the de-annualized value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, 0 is returned.
		\note This method will de-annualise the values based on the normal compound formula \f$(1+r)^{f/12}-1\f$ where r is the annual rate and f is the required frequency.<br/>If you want values to be treated using the SMM formula, use GetSMM() instead
		\sa GetValue(int)
		\sa GetValue(DateTime)
		\sa GetValue(DateTime,int)
		\sa GetSMM(DateTime,int)
		\sa GetSMM(int,int)
		*/
		double GetValue(int index,int Frequency){return Unmanaged->GetValue(index,Frequency);}
		/*!
		\brief Gets the de-annualized vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\arg Frequency the frequency, in months, by which the vector should be de-annualized. 1 for monthly de-annualization, 3 for quarterly
		\details Returns the de-annualized value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, 0 is returned.
		\note This method will de-annualise the values based on the SMM formula \f$1-(1-r)^{f/12} \f$ where r is the annual rate and f is the required frequency.<br/>If you want values to be treated using the normal compound formula, use GetValue() instead
		\sa GetSMM(DateTime,int)
		\sa GetValue(int)
		\sa GetValue(DateTime)
		\sa GetValue(int,int)
		\sa GetValue(DateTime,int)
		*/
		double GetSMM(int index, int Frequency) { return Unmanaged->GetSMM(index, Frequency); }
		/*!
		\brief Gets the de-annualized vector value for the specified date
		\arg index the date for which to get the value of the vector
		\arg Frequency the frequency, in months, by which the vector should be de-annualized. 1 for monthly de-annualization, 3 for quarterly
		\details Returns the de-annualized value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, 0 is returned.
		\note This method will de-annualise the values based on the SMM formula \f$1-(1-r)^{f/12} \f$ where r is the annual rate and f is the required frequency.<br/>If you want values to be treated using the normal compound formula, use GetValue() instead
		\sa GetSMM(int,int)
		\sa GetValue(DateTime)
		\sa GetValue(int)
		\sa GetValue(int,int)
		\sa GetValue(DateTime,int)
		*/
		double GetSMM(DateTime index, int Frequency) { return Unmanaged->GetSMM(DateTime2QDate(index), Frequency); }

		/*! 
		\brief Tries to set the vector to the supplied string
		\arg Vec The string containing the Bloomberg vector
		\return Whether the vector was changed or not
		\details This function will try to set the vector to represent the supplied string, if it's not a valid Bloomberg vector string the vector won't be changed and false is returned
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
		\brief Equivalent to BloombergSafeVector(DateTime) using today as the start date
		*/
		String^ BloombergSafeVector() { return QString2String(Unmanaged->BloombergSafeVector()); }
		/*!
		\brief Gets a version of the vector that it's safe to pass to Bloomberg.
		\arg CurrentDate the date from which Bloomberg expects the vector to start. It should be the cut off-of the pool.
		\details As Bloomberg discontinued support for anchored vectors, this function will convert the vector to a supported one without losing the anchor information.
		*/
		String^ BloombergSafeVector(DateTime CurrentDate) { return QString2String(Unmanaged->BloombergSafeVector(DateTime2QDate(CurrentDate))); }
		/*! 
		\brief Gets the anchor date of the vector
		*/
		DateTime GetAnchorDate() {return QDate2DateTime(Unmanaged->GetAnchorDate());}
		/*! 
		\brief Number of elements in the vector
		\details This will return the number of monthly elements in the vector. It will be 1 plus the number of steps and ramps in the vector.<br/>e.g.:
		<ul>
		<li>5 has 1 step</li>
		<li>5 12S 6 has 13 step</li>
		<li>5 12S 6 12R 7 has 25 step</li>
		</ul>
		*/
		int NumElements() {return Unmanaged->NumElements();}
		/*! 
		\brief Asserts either the vector is empty or not
		*/
		bool IsEmpty(){return Unmanaged->IsEmpty();}
		/*!
		\brief Asserts either the vector is empty or not and if the values are in a certain band
		\details This function will return false if the vector is valid and all the values in it are in the closed interval from Lbound to Ubound
		*/
		bool IsEmpty(double Lbound, double Ubound) { return Unmanaged->IsEmpty(Lbound, Ubound); }
		/*! 
		\brief Creates a vector that is the sum of the two.
		\details Creates a ManBloombergVector whose values are the sum of the values of the two vectors.<br/>If the vectors have different anchor dates, the values in the period between the two dates are dropped and the anchor date of the resulting vector is set to the most recent of the two.
		*/
		ManBloombergVector^ operator+(ManBloombergVector^ Vec){return gcnew ManBloombergVector(Unmanaged->operator+(*(Vec->Unmanaged)));}
		/*! 
		\brief Creates a vector that is the original one shifted upward by the specified value
		*/
		ManBloombergVector^ operator+(double a) {return gcnew ManBloombergVector(Unmanaged->operator+(a));}
		/*! 
		\brief Creates a vector that is the original one shifted donward by the specified value
		*/
		ManBloombergVector^ operator-(double a) {return gcnew ManBloombergVector(Unmanaged->operator-(a));}
		!ManBloombergVector()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManBloombergVector()
		{
			this->!ManBloombergVector();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManBloombergVector_h__