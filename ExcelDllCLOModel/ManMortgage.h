#ifndef ManMortgage_h__
#define ManMortgage_h__
#include "ManagedCommons.h"
#include "ManMtgCashFlow.h"
#include "Mortgage.h"
namespace ManagedCLO {
	/*!
	\brief Class that describes a loan
	\details This class is used to describe a loan characteristics and to calculate the expected cash flows generated by the loan.
	*/
	public ref class ManMortgage
	{
	private:
		static const unsigned int ClassIdentity=3;
	internal:
		Mortgage* Unmanaged;
		ManMortgage(const Mortgage& a){Unmanaged=new Mortgage(a);}
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
		\warning If the first element in the stream is not of this class, the stream will be advanced by 8 bytes anyway
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
		\brief The number of months between two interest payments made by the loan.
		*/
		property int PaymentFreq{
			int get(){return Unmanaged->GetPaymentFreq();}
			void set(int a){Unmanaged->SetPaymentFreq(a);}
		}
		/*!
		\brief The value of the base interest rate index. Set to 0 for fixed rate loans.
		\sa setFixedRate()
		*/
		property double FloatingRateBase{
			double get(){return Unmanaged->GetFloatingRateBase();}
			void set(double a){Unmanaged->SetFloatingRateBase(a);}
		}
		/*!
		\brief Sets the value of the base interest rate index to 0.
		\sa FloatingRateBase
		*/
		void setFixedRate(){Unmanaged->setFixedRate();}
		/*!
		\brief The maturity date of the loan.
		*/
		property DateTime MaturityDate{
			DateTime get(){return QDate2DateTime(Unmanaged->GetMaturityDate());}
			void set(DateTime a){return Unmanaged->SetMaturityDate(DateTime2QDate(a));}
		}
		/*!
		\brief The multiplier for the loss amount.
		\details This multiplier scales the losses incurred on the loans.<br/>The base is 100. Values greater than 100 denotes greater losses on the loan and viceversa.<br/>The formula to calculate the loss will be Outstanding * CDR * LS * (Loss Multiplier/100)<br/>Bloomber vectors are accepted
		*/
		property String^ LossMultiplier{
			String^ get(){return QString2String(Unmanaged->GetLossMultiplier());}
			void set(String^ a){Unmanaged->SetLossMultiplier(String2QString(a));}
		}
		/*!
		\brief The multiplier for the prepayment amount.
		\details This multiplier scales the prepayments of the loans.<br/>The base is 100. Values greater than 100 denotes higher prepayments on the loan and viceversa.<br/>The formula to calculate the loss will be Outstanding * CPR * (Prepay Multiplier/100)<br/>Bloomber vectors are accepted
		*/
		property String^ PrepayMultiplier{
			String^ get(){return QString2String(Unmanaged->GetPrepayMultiplier());}
			void set(String^ a){Unmanaged->SetPrepayMultiplier(String2QString(a));}
		}
		/*!
		\brief The Haircut.
		\details This vector represents haircuts to apply on the loan in percentage points.<br/>Examples:<ul><li>10 12S 20 will apply a 10% haircut on day 1 and a further 10% after one year.</li><li>0 24R 50 will gradually haircut the loan up to a total of 50% of the original value.</li><li>0 12S 30 12S 10 will cut by 30% the value of the loan after one year and then recover 20% on the second year</li></ul>
		*/
		property String^ HaircutVector{
			String^ get(){return QString2String(Unmanaged->GetHaircutVector());}
			void set(String^ a){Unmanaged->SetHaircutVector(String2QString(a));}
		}
		/*!
		\brief The loan cash flows
		\details Gets the calculated cash flows of the loan.<br/>If CalculateCashFlows was never called before, an empty cash flow is returned
		*/
		property ManMtgCashFlows^ CashFlow{
			 ManMtgCashFlows^ get(){return gcnew ManMtgCashFlows(Unmanaged->GetCashFlow());}
		}
		/*!
		\brief Vector defining if a loan is annuity or interest only
		\details Bloomberg-style vector defining whether the bond is an annuity.<br/>The vector can assume the value N for interest only and Y for annuity and can only move in steps.<br/>Examples of *valid* vectors:<br/>Y<br/>N 24S Y<br/>Examples of *invalid* vectors:<br/>5 24S N<br/>N 24R Y<br/>
		*/
		property String^ Annuity{
			String^ get(){return QString2String(Unmanaged->GetAnnuity());}
			void set(String^ a){Unmanaged->SetAnnuity(String2QString(a));}
		}
		/*!
		\brief Vector defining the coupon rate or margin of the bond
		\details Bloomberg vector defining the coupon or margin of the bond in percentage format (5%=5, 200bps=2)
		*/
		property String^ Interest{
			String^ get(){return QString2String(Unmanaged->GetInterest());}
			void set(String^ a){Unmanaged->SetInterest(String2QString(a));}
		}
		/*!
		\brief principal amount outstanding of the loan
		*/
		property double Size{
			double get(){return Unmanaged->GetSize();}
			void set(double a){Unmanaged->SetSize(a);}
		}
		/*!
		\brief Calculates the loans cash flows
		\arg CPRVec Bloomberg vector defining the CPR assumption
		\arg CDRVec Bloomberg vector defining the CDR assumption
		\arg LossVec Bloomberg vector defining the loss severity assumption
		\arg StartDate the loan cut off date
		\details Calculates the cash flows of the loans according to the supplied assumptions on losses and prepayments.<br/>The cash flows can then be retrieved using CashFlow
		*/
		void CalculateCashFlows(String^ CPRVec,String^ CDRVec,String^ LossVec,DateTime StartDate){
			Unmanaged->CalculateCashFlows(String2QString(CPRVec),String2QString(CDRVec),String2QString(LossVec),DateTime2QDate(StartDate));
		}
		/*!
		\brief Determines if all the parameters needed for the cash flow calculation are correct.
		\return An empty string if all the parameters are correct otherwise a list of the the invalid parameters (one for each line).
		\details <table border="1" cellpadding="3">
		<tr>
		<th>Check Performed</th>
		<th>String returned if failed</th>
		</tr>
		<tr>
		<td>Maturity date valid and greater than 1/1/2000</td>
		<td>Loan Maturity Date</td>
		</tr>
		<tr>
		<td>Valid Annuity Vector</td>
		<td>Loan Annuity Vector</td>
		</tr>
		<tr>
		<td>Outstanding principal amount of the loan not negative</td>
		<td>Loan Size</td>
		</tr>
		<tr>
		<td>Prepayment multiplier not negative</td>
		<td>Prepay Multiplier</td>
		</tr>
		<tr>
		<td>Loss multiplier not negative</td>
		<td>Loss Multiplier</td>
		</tr>
		<tr>
		<td>Valid Interest Vector</td>
		<td>Loan Coupon</td>
		</tr>
		<tr>
		<td>Valid Haircut Vector</td>
		<td>Haircut Vector</td>
		</tr>
		<tr>
		<td>Base interest rate not negative</td>
		<td>Loan Base Rate</td>
		</tr>
		<tr>
		<td>Not less than one month between two interest payments</td>
		<td>Loan Payment Frequency</td>
		</tr>
		</table>
		*/
		String^ ReadyToCalculate(){return QString2String(Unmanaged->ReadyToCalculate());}
		/*!
		\brief Deletes all the cash flows from the last calculation
		*/
		void ResetFlows(){Unmanaged->ResetFlows();}

		ManMortgage(){Unmanaged=new Mortgage();}
		//! Copy constructor
		ManMortgage(const ManMortgage% a){Unmanaged=new Mortgage(*(a.Unmanaged));}
		//! Copy constructor for ref class
		ManMortgage(ManMortgage^ a){Unmanaged=new Mortgage(*(a->Unmanaged));}
		!ManMortgage()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManMortgage()
		{
			this->!ManMortgage();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManMortgage_h__

