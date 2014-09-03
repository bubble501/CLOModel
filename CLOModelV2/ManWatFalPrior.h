#ifndef ManWatFalPrior_h__
#define ManWatFalPrior_h__
#include "WatFalPrior.h"
#include "ManagedCommons.h"
using namespace System;
namespace ManagedCLO {
	/*!
	\brief Class that represents a step in the payment waterfall
	\details This class is used to define a single step in the waterfall. Depending on the type of step the different parameters are used differently according to the following table:
	<table border="1" cellpadding="3">
	<tr>
	<th>Step Type</th>
	<th>Waterfall</th>
	<th>PriorityType</th>
	<th>GroupTarget</th>
	<th>RedemptionGroup</th>
	<th>RedemptionShare</th>
	</tr>
	<tr>
	<td>Senior Expenses</td>
	<td>Depending on RedemptionGroup</td>
	<td>wst_SeniorExpenses</td>
	<td>Ignored</td>
	<td>SourceOfFunding::sof_Interest to make this step part of the interest waterfall<br>
	SourceOfFunding::sof_Principal to make this step part of the principal waterfall<br>
	Any other value will cause the step to be ignored.</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Senior Fees</td>
	<td>Depending on RedemptionGroup</td>
	<td>wst_SeniorFees</td>
	<td>Ignored</td>
	<td>SourceOfFunding::sof_Interest to make this step part of the interest waterfall<br>
	SourceOfFunding::sof_Principal to make this step part of the principal waterfall<br>
	Any other value will cause the step to be ignored.</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Senior Fees</td>
	<td>Depending on RedemptionGroup</td>
	<td>wst_juniorFees</td>
	<td>Ignored</td>
	<td>SourceOfFunding::sof_Interest to make this step part of the interest waterfall<br>
	SourceOfFunding::sof_Principal to make this step part of the principal waterfall<br>
	Any other value will cause the step to be ignored.</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Interest</td>
	<td>Interest Waterfall</td>
	<td>wst_Interest</td>
	<td>Seniority group that will receive the interest payment</td>
	<td>Ignored</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Deferred Interest</td>
	<td>Interest Waterfall</td>
	<td>wst_DeferredInterest</td>
	<td>Seniority group that should receive payments for deferred
	interests.</td>
	<td>Ignored</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Deferred Interest</td>
	<td>Principal Waterfall</td>
	<td>wst_DeferredPrinc</td>
	<td>Seniority group that should receive payments for deferred
	interests.</td>
	<td>Ignored</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Reinvestment of principal during reinvestment period</td>
	<td>Principal Waterfall</td>
	<td>wst_ReinvestPrincipal</td>
	<td>Ignored</td>
	<td>What principal is allowed to be reinvested. 0 for all principal available, 1 for just unscheduled principal proceeds, 2 for just scheduled redemptions</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Principal</td>
	<td>Principal Waterfall</td>
	<td>wst_Principal</td>
	<td>Seniority group that will be redeemed</td>
	<td>Ignored</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>OC Test</td>
	<td>Interest Waterfall</td>
	<td>wst_OCTest</td>
	<td>Seniority group on which the test will be performed</td>
	<td>Seniority group that will be redeemed to cure the test.<br>
	A value less than 1 will cause the payment to be made to the most
	senior group outstanding.</td>
	<td>The share of the payments made to cure the test that will be used
	to redeemed the group indicated in RedemptionGroup.<br>
	The rest will go toward the most senior one</td>
	</tr>
	<tr>
	<td>OC Test</td>
	<td>Principal Waterfall</td>
	<td>wst_OCTestPrinc</td>
	<td>Seniority group on which the test will be performed</td>
	<td>Seniority group that will be redeemed to cure the test.<br>
	A value less than 1 will cause the payment to be made to the most
	senior group outstanding.</td>
	<td>The share of the payments made to cure the test that will be used
	to redeemed the group indicated in RedemptionGroup.<br>
	The rest will go toward the most senior one</td>
	</tr>
	<tr>
	<td>IC Test</td>
	<td>Interest Waterfall</td>
	<td>wst_ICTest</td>
	<td>Seniority group on which the test will be performed</td>
	<td>Ignored</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>IC Test</td>
	<td>Principal Waterfall</td>
	<td>wst_ICTestPrinc</td>
	<td>Seniority group on which the test will be performed</td>
	<td>Ignored</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Reinvestment Test</td>
	<td>Principal Waterfall</td>
	<td>wst_ReinvestmentTest</td>
	<td>Seniority group on which the test will be performed</td>
	<td>Seniority group that will be redeemed to cure the test.</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Excess Spread</td>
	<td>Both</td>
	<td>wst_Excess</td>
	<td>Ignored</td>
	<td>Seniority group that will be receiving the excess spread.<br>Leave blank if payed outside the deal</td>
	<td>Ignored</td>
	</tr>
	<tr>
	<td>Replenishment of Reserve Fund</td>
	<td>Depending on RedemptionGroup</td>
	<td>wst_ReserveReplenish</td>
	<td>The index of the reserve to replenish. Currently 2 reserves are modelled so it can be either 1 or 2</td>
	<td>SourceOfFunding::sof_Interest to make this step part of the interest waterfall<br>
	SourceOfFunding::sof_Principal to make this step part of the principal waterfall<br>
	Any other value will cause the step to be ignored.</td>
	<td>Ignored</td>
	</tr>
	</table>
	\note Seniority group means a group of tranches that receive interest and principal pro-rata
	*/
	public ref class ManWatFalPrior
	{
	private:
		static const unsigned int ClassIdentity=2;
	internal:
		WatFalPrior* Unmanaged;
		ManWatFalPrior(const WatFalPrior& a){Unmanaged=new WatFalPrior(a);}
		ManWatFalPrior(const WatFalPrior* a){Unmanaged=new WatFalPrior(*a);}
	public:
		/*! Enum defining the different types of waterfall steps */
		enum class ManWaterfallStepType : int{
			wst_SeniorExpenses, /*!< Senior Expenses*/
			wst_SeniorFees, /*!< Senior Fees*/
			wst_Interest, /*!< Interest Payment*/
			wst_Principal,  /*!< Notes Redemption*/
			wst_OCTest,  /*!< Over Collateralization test. Considered part of the interest waterfall*/
			wst_OCTestPrinc, /*!< Over Collateralization test. Considered part of the principal waterfall*/
			wst_ICTest, /*!< Interest Coverage test. Considered part of the interest waterfall*/
			wst_ICTestPrinc, /*!< Interest Coverage test. Considered part of the principal waterfall*/
			wst_DeferredInterest, /*!< Payment of deferred interest. Considered part of the interest waterfall*/
			wst_DeferredPrinc, /*!< Payment of deferred interest. Considered part of the principal waterfall*/
			wst_juniorFees, /*!< Junior Fees*/
			wst_ReinvestmentTest, /*!< Reinvestment Test*/
			wst_Excess, /*!< Excess Spread*/
			wst_ReinvestPrincipal, /*!< Reinvest all available principal during the reinvestment period*/
			wst_ReserveReplenish /*!< Replenish the reserve fund*/
		};
		/*! 
		\brief Determines the source of funding for expenses and fees
		\details This enum is used as GroupTarget for expenses and fees step to determine whether they should be paid using interest proceeds or princiapl proceeds
		*/
		enum class SourceOfFunding : int{
			sof_Interest = 1, /*!< Interest waterfall*/
			sof_Principal = 2 /*!< Principal Waterfall*/
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
		\brief The seniority group referenced by the step
		\details This is the seniority group to which the step refers to.<br/>For steps that don't require it, like wst_SeniorExpenses, it's just ignored
		*/
		property int GroupTarget{
			int get(){return Unmanaged->GetGroupTarget();}
			void set(int a){Unmanaged->SetGroupTarget(a);}
		}
		/*!
		\brief The index defining where payments triggered by this step will go or are coming from
		\details For steps like wst_OCTest this represents the seniority group that will be redeemed in case the test fails, for steps like wst_SeniorExpenses it will be 1 if the payment of the expense is taken from the interest available amount and 2 if it's taken from principal.<br/>For steps that don't require it, it's just ignored.
		*/
		property int RedemptionGroup{
			int get(){return Unmanaged->GetRedemptionGroup();}
			void set(int a){Unmanaged->SetRedemptionGroup(a);}
		}
		/*!
		\brief The share of payments toward the RedemptionGroup
		\details This is used just by the OC test and represent the share (from 0 to 1) of payments made to cure the test that will go toward the seniority group indicated in RedemptionGroup.<br/>The complement to 1 of this share will go toward the most senior group.
		*/
		property double RedemptionShare{
			double get(){return Unmanaged->GetRedemptionShare();}
			void set(double a){Unmanaged->SetRedemptionShare(a);}
		}
		/*!
		\brief The type of step
		\details This property defines the type of step in the waterfall
		*/
		property ManWaterfallStepType PriorityType{
			ManWaterfallStepType get() {return static_cast<ManWaterfallStepType>(static_cast<int>(Unmanaged->GetPriorityType()));}
			void set(ManWaterfallStepType a){Unmanaged->SetPriorityType(static_cast<WatFalPrior::WaterfallStepType>(static_cast<int>(a)));}
		}

		ManWatFalPrior(){Unmanaged=new WatFalPrior();}
		//! Copy constructor
		ManWatFalPrior(const ManWatFalPrior% a){Unmanaged=new WatFalPrior(*(a.Unmanaged));}
		
		//! Copy constructor for ref class
		ManWatFalPrior(ManWatFalPrior^ a){Unmanaged=new WatFalPrior(*(a->Unmanaged));}
		!ManWatFalPrior()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = 0;
			}
		}
		~ManWatFalPrior()
		{
			this->!ManWatFalPrior();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManWatFalPrior_h__

