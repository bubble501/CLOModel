#include "AnnuityVector.h"
#include <QByteArray>
#include <QDataStream>
#include "ManagedCommons.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Boolean scenario vector.
	\details This class handles vectors that follow the Bloomberg conventions in SCEN <GO> but accepting 'Y' and 'N' as levels instead of numbers.<br/>Given the nature of the data, ramps are not allowed.
	 */
	public ref class ManAnnuityVector
	{
	private:
		static const unsigned int ClassIdentity=9;
	internal:
		AnnuityVector* Unmanaged;
		ManAnnuityVector(const AnnuityVector& a){Unmanaged=new AnnuityVector(a);}
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
		

		ManAnnuityVector(){Unmanaged=new AnnuityVector();}
		//! Copy constructor
		ManAnnuityVector(const ManAnnuityVector% a){Unmanaged=new AnnuityVector(*(a.Unmanaged));}

		//! Copy constructor for ref class
		ManAnnuityVector(ManAnnuityVector^ a){Unmanaged=new AnnuityVector(*(a->Unmanaged));}

		/*! 
		\brief Constructor taking the vector string
		\details If the supplied vector is not a valid annuity vector the class will default to an empty vector
		*/
		ManAnnuityVector(String^ Vec){Unmanaged=new AnnuityVector(String2QString(Vec));}
		/*! 
		\brief Constructor taking the vector string and the anchor date
		\details If the supplied vector is not a valid annuity vector the class will default to an empty vector.<br/>If the vector string starts with an anchor date, it will be overwritten by the supplied one.
		*/
		ManAnnuityVector(String^ Vec,DateTime Anchor){Unmanaged=new AnnuityVector(String2QString(Vec),DateTime2QDate(Anchor));}
		/*! 
		\brief Gets the vector value for the specified date
		\arg index the date for which to get the value of the vector
		\returns 'Y'(89) if the vector at the supplied index is an annuity, 'N'(78) if it's interest only and 0 if the vector or the index are invalid
		\details Returns the value of the vector corresponding to the specific date.<br/>If no anchor date is specified the first day of the vector will be assumed to be today.<br/>If the date provided is anterior to the anchor date or the vector is empty, 0 is returned.
		\sa GetValue(int)
		*/
		char GetValue(DateTime index){return Unmanaged->GetValue(DateTime2QDate(index));}
		/*! 
		\brief Gets the vector value for the specified index
		\arg index The number of months after the start of the vector for which the value is retrieved
		\returns 'Y'(89) if the vector at the supplied index is an annuity, 'N'(78) if it's interest only and 0 if the vector or the index are invalid
		\details Returns the value of the vector corresponding to the specific index.<br/>If the vector is empty or the index supplied is negative, 0 is returned.
		\sa GetValue(DateTime)
		*/
		char GetValue(int index){return Unmanaged->GetValue(index);}
		/*! 
		\brief Tries to set the vector to the supplied string
		\arg Vec The string containing the Bloomberg vector
		\return Whether the vector was changed or not
		\details This function will try to set the vector to represent the supplied string, if it's not a valid annuity vector string the vector won't be changed and false is returned
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
		<li>Y has 1 step</li>
		<li>Y 12S N has 13 step</li>
		</ul>
		*/
		int NumElements() {return Unmanaged->NumElements();}

		!ManAnnuityVector()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManAnnuityVector()
		{
			this->!ManAnnuityVector();
			GC::SuppressFinalize(this);
		}
	};
}