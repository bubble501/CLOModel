#ifndef ManReinvestmentTest_h__
#define ManReinvestmentTest_h__
#include "ManagedCommons.h"
#include "ReinvestmentTest.h"
#include "ManMortgage.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Class defining the reinvestment test
	\details This class defines the details of reinvestments and the reinvestment test.
	\note The details of the reinvestment bond are used not only by the reinvestment test but also by reinvestment of principal
	*/
	public ref class ManReinvestmentTest
	{
	private:
		static const unsigned int ClassIdentity=4;
	internal:
		ReinvestmentTest* Unmanaged;
		ManReinvestmentTest(const ReinvestmentTest& a){Unmanaged=new ReinvestmentTest(a);}
	public:
		/*! Enum defining the use of funds when the test is broken*/
		enum class ManShareType : int{
			InReinvShare, /*!< Share toward acquiring new collateral during reinvestment period*/
			InRedempShare, /*!< Share redemption of notes during reinvestment period*/
			OutReinvShare, /*!< Share toward acquiring new collateral after reinvestment period*/
			OutRedempShare /*!< Share redemption of notes after reinvestment period*/
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
		\brief The date of expiration of the reinvestment period.
		\details Returns the date at which the reinvestment period will end.
		*/
		property DateTime ReinvestmentPeriod{
			DateTime get(){return QDate2DateTime(Unmanaged->GetReinvestmentPeriod());}
		}
		/*!
		\brief The level of the test trigger.
		\details The minimum required level for the test not to be broken in a scalar form (1=100%, 1.1=110%)
		*/
		property double TestLevel{
			double get(){return Unmanaged->GetTestLevel();}
		}
		/*!
		\brief Returns the share of funds diverted toward Tpe
		\arg Tpe The type of share of funds to retrieve
		\return The requested share in the interval [0;1]
		*/
		double GetShare(ManShareType Tpe){return Unmanaged->GetShare(static_cast<ReinvestmentTest::ShareType>(static_cast<int>(Tpe)));}
		/*!
		\brief The CPR vector used for the reinvestment bond
		\details Defines the CPR vector used when calculating the cash flows of the collateral bought with reinvestment funds
		*/
		property String^ CPRAssumption{
			String^ get(){return QString2String(Unmanaged->GetCPRAssumption());}
			void set(String^ a){Unmanaged->SetCPR(String2QString(a));}
		}
		/*!
		\brief The CDR vector used for the reinvestment bond
		\details Defines the CDR vector used when calculating the cash flows of the collateral bought with reinvestment funds
		*/
		property String^ CDRAssumption{
			String^ get(){return QString2String(Unmanaged->GetCDRAssumption());}
			void set(String^ a){Unmanaged->SetCDR(String2QString(a));}
		}
		/*!
		\brief The LS vector used for the reinvestment bond
		\details Defines the LS vector used when calculating the cash flows of the collateral bought with reinvestment funds
		*/
		property String^ LSAssumption{
			String^ get(){return QString2String(Unmanaged->GetLSAssumption());}
			void set(String^ a){Unmanaged->SetLS(String2QString(a));}
		}
		/*!
		\brief The WAL vector used for the reinvestment bond
		\details Defines the weighted average life vector used when calculating the cash flows of the collateral bought with reinvestment funds
		*/
		property String^ WALAssumption{
			String^ get(){return QString2String(Unmanaged->GetWALAssumption());}
		}
		/*!
		\brief The price of new bonds
		\details A Bloomberg vector defining at what price are new investment bought. "100" means reinvestment at par.
		*/
		property String^ ReinvestmentPrice{
			String^ get() { return QString2String(Unmanaged->GetReinvPrice()); }
		}
		/*!
		\brief The delay in reinvesting
		\details Defines the weighted average life vector used to determine how much time it will take to reinvest the funds. "0" means immediate reinvestment.
		*/
		property String^ ReinvestmentDelay{
			String^ get() { return QString2String(Unmanaged->GetReinvDelay()); }
		}
		/*!
		\brief Define the characteristics of the bond bought using the reinvestment funds
		\arg IntrVec A Bloomberg vector defining the coupon or the margin paid by the bond
		\arg CPRVec The CPR vector used for the reinvestment bond
		\arg CDRVec The CDR vector used for the reinvestment bond
		\arg LSVec The LS vector used for the reinvestment bond
		\arg WALval The WAL vector used for the reinvestment bond
		\arg PayFreq The number of months between two interest payments from the bond 
		\arg AnnuityVec A Bloomberg-style vector defining whether the bond is an annuity.<br/>Refer to ManRepaymentVector for valid input values
		\arg ReinvPric A Bloomberg vector defining at what price are new investment bought. Set to "100" to reinvest at par
		\arg ReinvDelay A vector of integers that define how much time it will take to reinvest the funds. Set to "0" for immediate reinvestment
		\arg FloatingBase the base rate value to be added to IntrVec if it's a margin vector. Use 0 for fixed rate.
		*/
		void SetupReinvBond(String^ IntrVec, String^ CPRVec, String^ CDRVec, String^ LSVec, String^ WALval, String^ PayFreq, String^ AnnuityVec, String^ ReinvPric, String^ ReinvDelay, String^ FloatingBase) {
			Unmanaged->SetupReinvBond(String2QString(IntrVec), String2QString(CPRVec), String2QString(CDRVec), String2QString(LSVec), String2QString(WALval), String2QString(PayFreq), String2QString(AnnuityVec), String2QString(ReinvPric), String2QString(ReinvDelay), String2QString(FloatingBase));
		}
		/*!
		\brief Define the characteristics of the reinvestment test
		\arg ReinvPer The date on which the reinvestment period will end
		\arg TstLvl The minimum level of the test for it to pass in a scalar form (1=100%, 1.1=110%)
		\arg IIshare Share toward acquiring new collateral during reinvestment period in a scalar form (0.5=50%)
		\arg IRshare Share redemption of notes during reinvestment period in a scalar form (0.5=50%)
		\arg OIshare Share toward acquiring new collateral after reinvestment period in a scalar form (0.5=50%)
		\arg ORshare Share redemption of notes after reinvestment period in a scalar form (0.5=50%)
		*/
		void SetupTest(DateTime ReinvPer,double TstLvl, double IIshare,double IRshare,double OIshare,double ORshare){
			Unmanaged->SetupTest(DateTime2QDate(ReinvPer),TstLvl,IIshare,IRshare,OIshare,ORshare);
		}
		/*!
		\brief Calculate the reinvestment bond cash flows
		\arg Size The outstanding amount of the reinvestment bond
		\arg StartDate the cut off date of the bond
		\arg Period the number of months from the the start time of the vectors to the cut off date
		\details This function calculates the cash flows of the reinvestment bond, they can then be retrieved using BondCashFlow
		\note An enhancement is currently under development to make the Period argument redundant
		*/
		void CalculateBondCashFlows(double Size, DateTime StartDate, int Period){
			Unmanaged->CalculateBondCashFlows(Size,DateTime2QDate(StartDate),Period);
		}
		/*!
		\brief Gets the cash flows of the reinvestment bond
		\details Returns a copy of the latest calculated cash flows of the reinvestment bond.<br/>If CalculateBondCashFlows is not run before calling this property, an empty cash flow is returned.
		*/
		property ManMtgCashFlows^ BondCashFlow{
			ManMtgCashFlows^ get(){return gcnew ManMtgCashFlows(Unmanaged->GetBondCashFlow());}
		}
		/*!
		\brief Gets the reinvestment bond
		\details Returns a copy of the reinvestment bond
		*/
		property ManMortgage^ ReinvestmentBond{
			ManMortgage^ get(){return gcnew ManMortgage(Unmanaged->GetReinvestmentBond());}
		}
		ManReinvestmentTest(){Unmanaged=new ReinvestmentTest();}
		//! Copy constructor
		ManReinvestmentTest(const ManReinvestmentTest% a){Unmanaged=new ReinvestmentTest(*(a.Unmanaged));}
		//! Copy constructor for ref class
		ManReinvestmentTest(ManReinvestmentTest^ a){Unmanaged=new ReinvestmentTest(*(a->Unmanaged));}
		!ManReinvestmentTest()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManReinvestmentTest()
		{
			this->!ManReinvestmentTest();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManReinvestmentTest_h__

