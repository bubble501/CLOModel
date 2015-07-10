#ifndef ManTranche_h__
#define ManTranche_h__
#include "Tranche.h"
#include "ManTrancheCashFlow.h"
#include "ManagedCommons.h"
#include "ManBloombergVector.h"
#include "ManBaseRateTable.h"
namespace ManagedCLO {
	/*!
	\brief Class used to describe a single tranche of a deal
	*/
	public ref class ManTranche
	{
	private:
		static const unsigned int ClassIdentity=5;
	internal:
		Tranche* Unmanaged;
		ManTranche(const Tranche& a){Unmanaged=new Tranche(a);}
		ManTranche(const Tranche* a){Unmanaged=new Tranche(*a);}
	public:
		/*! Enum defining what type of coupon the tranche is paying*/
		enum class ManTrancheInterestType : int {
			FixedInterest, /*!< Fixed Coupon*/
			FloatingInterest /*!< Floating Rate Coupon*/
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
		\brief Returns true if this tranche is more junior than the compared one
		*/
		bool operator<(ManTranche^ a){return Unmanaged->operator<(*(a->Unmanaged));}
		/*!
		\brief Returns true if this tranche is more senior than the compared one
		*/
		bool operator>(ManTranche^ a){return Unmanaged->operator>(*(a->Unmanaged));}
		/*!
		\brief The currency code for the tranche denomination
		\details <ul><li>Euro &rarr; EUR</li><li>Sterling &rarr; GBP</li><li>US Dollars &rarr; USD</li><li>Swiss Franc &rarr; CHF</li><li>Aussie Dollar &rarr; AUD</li></ul>
		*/
		property String^ Currency{
			String^ get(){return QString2String(Unmanaged->GetCurrency());}
			void set(String^ a){Unmanaged->SetCurrency(String2QString(a));}
		}
		/*!
		\brief The ticker of the tranche
		*/
		property String^ TrancheName{
			String^ get(){return QString2String(Unmanaged->GetTrancheName());}
			void set(String^ a){Unmanaged->SetTrancheName(String2QString(a));}
		}
		/*!
		\brief The ISIN of the tranche
		*/
		property String^ TrancheISIN{
			String^ get(){return QString2String(Unmanaged->GetISIN());}
			void set(String^ a){Unmanaged->SetISIN(String2QString(a));}
		}
		/*!
		\brief The original issue nominal amount of the tranche
		\note The input must be in the tranche currency while the output will be in deal currency
		\sa BaseCurrencyOutsanding
		*/
		property double OriginalAmount{
			double get(){return Unmanaged->GetOriginalAmount();}
			void set(double a){Unmanaged->SetOriginalAmount(a);}
		}
		/*!
		\brief The current outstanding amount of the tranche
		\note The input must be in the tranche currency while the output will be in deal currency
		\sa BaseCurrencyOriginal
		*/
		property double OutstandingAmt{
			double get(){return Unmanaged->GetOutstandingAmt();}
			void set(double a){Unmanaged->SetOutstandingAmt(a);}
		}
		/*!
		\brief The current outstanding amount of the tranche (in tranche currency)
		\sa OutstandingAmt
		*/
		property double BaseCurrencyOutsanding{
			double get(){return Unmanaged->GetBaseCurrencyOutsanding();}
		}
		/*!
		\brief The original issue nominal amount of the tranche (in tranche currency)
		\sa OriginalAmount
		*/
		property double BaseCurrencyOriginal{
			double get(){return Unmanaged->GetBaseCurrencyOriginal();}
		}
		/*!
		\brief The type of interest of the tranche (fixed or floating)
		\details The index specifies what coupon you wish to set (0 is the main coupon)
		*/
		property ManTrancheInterestType InterestType[int]{
			ManTrancheInterestType get(int CoupType) { return static_cast<ManTrancheInterestType>(static_cast<int>(Unmanaged->GetInterestType(CoupType))); }
			void set(int CoupType, ManTrancheInterestType a) { Unmanaged->SetInterestType(static_cast<Tranche::TrancheInterestType>(static_cast<int>(a)), CoupType); }
		}
		/*!
		\brief Set the coupon or margin paid by the tranche
		\details If the tranche is fixed rate this is the interest paid on the note, if the tranche is floating rate this is the margin over the base rate.<br/>Accepts Bloomberg vectors. If the anchor date for the vector is not specified, the last payment date will be used.<br/>The format is in basis points 5%=500.<br/>The index specifies what coupon you wish to set (0 is the main coupon)
		*/
		property String^ Coupon[int]{
			void set(int CoupType, String^ a) { Unmanaged->SetCoupon(String2QString(a), CoupType); }
		}
		/*!
		\brief The annualised coupon paid by the tranche
		\arg index The time index for which to retrieve the tranche coupon. 0 corresponds to the Coupon vector anchor date or, if it's missing, the last payment date.
		\details If the tranche is fixed rate this is the interest paid on the note, if the tranche is floating rate this is the margin over the base rate.
		\note If the tranche is floating rate, the output of this property will be the sum of the base interest rate and the margin.
		\sa GetRawCoupon(int)
		*/
		double GetCoupon(int index, int CoupType) { return Unmanaged->GetCoupon(index, CoupType); }
		/*!
		\brief The annualised coupon paid by the tranche
		\arg index The date for which to retrieve the tranche coupon.
		\arg CoupType Specifies what coupon you are referring to (0 is the main coupon)
		\details If the tranche is fixed rate this is the interest paid on the note, if the tranche is floating rate this is the margin over the base rate.
		\note If the tranche is floating rate, the output of this property will be the sum of the base interest rate and the margin.
		\sa GetRawCoupon(DateTime)
		*/
		double GetCoupon(DateTime index, int CoupType) { return Unmanaged->GetCoupon(DateTime2QDate(index), CoupType); }
		/*!
		\brief The coupon paid by the tranche
		\arg index The time index for which to retrieve the tranche coupon. 0 corresponds to the Coupon vector anchor date or, if it's missing, the last payment date.
		\arg CoupType Specifies what coupon you are referring to (0 is the main coupon)
		\arg Frequency the frequency, in months, by which the coupon should be de-annualized. 1 for monthly de-annualization, 3 for quarterly
		\details If the tranche is fixed rate this is the interest paid on the note, if the tranche is floating rate this is the margin over the base rate.
		\note If the tranche is floating rate, the output of this property will be the sum of the base interest rate and the margin.
		\sa GetRawCoupon(int)
		*/
		double GetCoupon(int index, int CoupType, int Frequency) { return Unmanaged->GetCoupon(index, CoupType, Frequency); }
		/*!
		\brief The coupon paid by the tranche
		\arg index The date for which to retrieve the tranche coupon.
		\arg CoupType Specifies what coupon you are referring to (0 is the main coupon)
		\arg Frequency the frequency, in months, by which the coupon should be de-annualized. 1 for monthly de-annualization, 3 for quarterly
		\details If the tranche is fixed rate this is the interest paid on the note, if the tranche is floating rate this is the margin over the base rate.
		\note If the tranche is floating rate, the output of this property will be the sum of the base interest rate and the margin.
		\sa GetRawCoupon(DateTime)
		*/
		double GetCoupon(DateTime index, int CoupType, int Frequency) { return Unmanaged->GetCoupon(DateTime2QDate(index), CoupType,Frequency); }
		/*!
		\brief The coupon or margin paid by the tranche
		\arg index The time index for which to retrieve the tranche coupon. 0 corresponds to the Coupon vector anchor date or, if it's missing, the last payment date.
		\arg CoupType Specifies what coupon you are referring to (0 is the main coupon)
		\details For fixed rate tranches this is equivalent to GetCoupon, for floating rate notes this will return the margin over the base rate.
		\sa GetCoupon(int)
		*/
		double GetRawCoupon(int index, int CoupType) { return Unmanaged->GetRawCoupon(index, CoupType); }
		/*!
		\brief The coupon or margin paid by the tranche
		\arg index The date for which to retrieve the tranche coupon.
		\arg CoupType Specifies what coupon you are referring to (0 is the main coupon)
		\details For fixed rate tranches this is equivalent to GetCoupon, for floating rate notes this will return the margin over the base rate.
		\sa GetCoupon(DateTime)
		*/
		double GetRawCoupon(DateTime index, int CoupType) { return Unmanaged->GetRawCoupon(DateTime2QDate(index), CoupType); }
		/*!
		\brief The ticker of the base rate of the tranche
		\details This property contains the bloomberg ticker (*without* yellow key) of the base rate for floating rate tranche.<br/>The index specifies what coupon you wish to set (0 is the main coupon).
		\sa ReferenceRateValue
		*/
		property String^ ReferenceRate[int]{
			String^ get(int CoupType) { return QString2String(Unmanaged->GetReferenceRate(CoupType)); }
			void set(int CoupType, String^ a) { Unmanaged->SetReferenceRate(String2QString(a), CoupType); }
		}
		/*!
		\brief The value of the base rate for the tranche
		\details For floating rate tranches this property holds the value of the base rate and for fixed rate notes the value for the base rate used when calculating the discount margin.<br/>Use CompileBaseRates or GetBaseRatesDatabase to set the value for this property.<br/>The index specifies what coupon you wish to set (0 is the main coupon).
		\arg CoupType Specifies what coupon you are referring to (0 is the main coupon)
		\sa ReferenceRate
		\sa DefaultRefRate
		\sa GetReferenceRateValue(int)
		\sa GetReferenceRateValue(Datetime)
		*/
		property String^ ReferenceRateValue[int]{
			String^ get(int CoupType) { return QString2String(Unmanaged->GetReferenceRateValue(CoupType)); }
			//void set(String^ a){Unmanaged->SetReferenceRateValue(String2QString(a));}
		}
		/*!
		\brief Whether the tranches uses forward curves for the base rates or not
		*/
		property bool UseForwardCurve{
			bool get() { return Unmanaged->GetUseForwardCurve(); }
		}
		/*!
		\brief The value of the base rate for the tranche at the specified index
		\arg index The time index for which to retrieve the index value. 0 corresponds to the vector anchor date or, if it's missing, the last payment date.
		\details For floating rate tranches this represents the value of the base rate and for fixed rate notes the value for the base rate used when calculating the discount margin.
		\sa ReferenceRateValue
		\sa GetReferenceRateValue(Datetime)
		*/
		/*!
		\brief The value of the base rate for the tranche at the specified index
		\arg index The date for which to retrieve the index value.
		\details For floating rate tranches this represents the value of the base rate and for fixed rate notes the value for the base rate used when calculating the discount margin.
		\sa ReferenceRateValue
		\sa GetReferenceRateValue(int)
		*/
		double GetReferenceRateValue(DateTime index, int CoupType) { return Unmanaged->GetReferenceRateValue(DateTime2QDate(index), CoupType); }
		/*!
		\brief Uses the supplied base rates table to fill in the base rate values
		\arg Values a table containing the spot rates of the relevant base rates
		\details This function will use the values supplied in the table as the values for the relevant base index rates.<br/>If a base index is required by the model but not supplied it will be downloaded from Bloomberg and added to the Values table.
		\sa GetBaseRatesDatabase
		*/
		void CompileBaseRates(ManConstBaseRateTable^ Values){
			ConstantBaseRateTable FillValues = *(Values->Unmanaged);
			Unmanaged->CompileReferenceRateValue(FillValues);
			Values = gcnew ManConstBaseRateTable(FillValues);
		}
		/*!
		\brief Uses the supplied base rates table to fill in the base rate values
		\arg Values a table containing the forward curves of the relevant base rates
		\details This function will use the values supplied in the table as the values for the relevant base index rates.<br/>If a base index is required by the model but not supplied its spot rate will be downloaded from Bloomberg and added to the Values table.
		\sa GetBaseRatesDatabase
		*/
		void CompileBaseRates(ManForwBaseRateTable^ Values) {
			ForwardBaseRateTable FillValues = *(Values->Unmanaged);
			Unmanaged->CompileReferenceRateValue(FillValues);
			Values = gcnew ManForwBaseRateTable(FillValues);
		}

#ifndef NO_DATABASE
		/*!
		\brief Downloads the necessary base rates from the database
		\arg Values A table of overrides for specific indexes.
		\arg DownloadAll If set to true it will download all rates from the database instead of just the needed ones. Normally set to false. If Values is used for more than one model using true may result in more efficiency
		\details This function will download the base rates values from the database.<br/>The Values table can be used to override specific base rates.<br/>If a base index is required by the model but not supplied it will be downloaded from Bloomberg.<br/>All downloaded values will be added to the Values table.
		\note If NO_DATABASE is defined at compile time this method will be unavailable
		\sa CompileBaseRates
		*/
		void GetBaseRatesDatabase(ManConstBaseRateTable^ Values, bool DownloadAll) {
			ConstantBaseRateTable FillValues = *(Values->Unmanaged);
			Unmanaged->GetBaseRatesDatabase(FillValues);
			Values = gcnew ManConstBaseRateTable(FillValues);
		}
		/*!
		\brief Downloads the necessary base rates from the database
		\arg Values A table of overrides for specific indexes.
		\arg DownloadAll If set to true it will download all rates from the database instead of just the needed ones. Normally set to false. If Values is used for more than one model using true may result in more efficiency
		\details This function will download the base rates values from the database.<br/>The Values table can be used to override specific base rates.<br/>If a base index is required by the model but not supplied its spot rate will be downloaded from Bloomberg and used as constant.<br/>All downloaded values will be added to the Values table.
		\note If NO_DATABASE is defined at compile time this method will be unavailable
		\sa CompileBaseRates
		*/
		void GetBaseRatesDatabase(ManForwBaseRateTable^ Values, bool DownloadAll) {
			ForwardBaseRateTable FillValues = *(Values->Unmanaged);
			Unmanaged->GetBaseRatesDatabase(FillValues);
			Values = gcnew ManForwBaseRateTable(FillValues);
		}
#endif
		/*!
		\brief The price of the tranche
		*/
		property double Price{
			double get(){return Unmanaged->GetPrice();}
			void set(double a){Unmanaged->SetPrice(a);}
		}
		/*!
		\brief Calculates the price associated with a certain discount margin
		\arg DiscountMargin the target discount margin
		\details This function tries to calculate the price associated with a target discount margin. If the result can't be found -1 is returned
		*/
		double GetPrice(double DiscountMargin) { return Unmanaged->GetPrice(DiscountMargin); }
		/*!
		\brief The Bloomberg yellow key of the tranche.
		\details By default is set to Mtge by the constructor.
		*/
		property String^ BloombergExtension{
			String^ get(){return QString2String(Unmanaged->GetBloombergExtension());}
			void set(String^ a){Unmanaged->SetBloombergExtension(String2QString(a));}
		}
		/*!
		\brief The seniority group of the tranche
		\details This determines the seniority of the tranche in the waterfall, tranches that share the same %ProrataGroup are considered to be pro-rata.<br/>Values less or equal to 0 are invalid. The higher this value the more junior the tranche is.<br/>The most senior tranche should have this value set at 1.
		*/
		property int ProrataGroup{
			int get(){return Unmanaged->GetProrataGroup();}
			void set(int a){Unmanaged->SetProrataGroup(a);}
		}
		/*!
		\brief The cash flows of the tranche
		*/
		property ManTrancheCashFlow^ CashFlow{
			ManTrancheCashFlow^ get(){return gcnew ManTrancheCashFlow(Unmanaged->GetCashFlow());}
			void set(ManTrancheCashFlow^ a){Unmanaged->SetCashFlow(*(a->Unmanaged));}
		}
		/*!
		\brief The OC test trigger for the tranche
		*/
		property double MinOClevel{
			double get(){return Unmanaged->GetMinOClevel();}
			void set(double a){Unmanaged->SetMinOClevel(a);}
		}
		/*!
		\brief The IC test trigger for the tranche
		*/
		property double MinIClevel{
			double get(){return Unmanaged->GetMinIClevel();}
			void set(double a){Unmanaged->SetMinIClevel(a);}
		}
		/*!
		\brief The previous IPD of the tranche
		*/
		property DateTime LastPaymentDate{
			DateTime get(){return QDate2DateTime(Unmanaged->GetLastPaymentDate());}
			void set(DateTime a){Unmanaged->SetLastPaymentDate(DateTime2QDate(a));}
		}
		/*!
		\brief The day count convention used to calculate interest.
		\details By default is set to 360 by the constructor, this corresponds to Actual/360
		*/
		property ManDayCountConvention DayCount{
			ManDayCountConvention get() { return static_cast<ManDayCountConvention>(static_cast<int>(Unmanaged->GetDayCount())); }
			void set(ManDayCountConvention a) { Unmanaged->SetDayCount(static_cast<DayCountConvention>(static_cast<int>(a))); }
		}
		/*!
		\brief The ticker of the default base rate.
		\details Used only for fixed rate tranches, this property contains the bloomberg ticker (*without* yellow key) of the base rate to use when calculating the discount margin on fixed rate tranches.
		\sa ReferenceRateValue
		*/
		property String^ DefaultRefRate{
			String^ get(){return QString2String(Unmanaged->GetDefaultRefRate());}
			void set(String^ a){Unmanaged->SetDefaultRefRate(String2QString(a));}
		}
		/*!
		\brief The exchange rate applied to the tranche.
		\details This is the exchange rate to apply to convert the amounts in tranche currency to the deal currency.<br/>If the tranche is already in deal currency set this to 1
		*/
		property double ExchangeRate{
			double get(){return Unmanaged->GetExchangeRate();}
			void set(double a){Unmanaged->SetExchangeRate(a);}
		}
		/*!
		\brief The number of months between two IPDs
		\details A Bloomberg vector representing the the number of months between two payment dates
		*/
		property String^ PaymentFrequency{
			String^ get(){return QString2String(Unmanaged->GetPaymentFrequency());}
			void set(String^ a){Unmanaged->SetPaymentFrequency(String2QString(a));}
		}
		/*!
		\brief Add a cash flow to the tranche
		\arg Dte Date of the flow 
		\arg Amt amount of the flow
		\arg FlwTpe Type of flow
		*/
		void AddCashFlow(DateTime Dte,double Amt, ManTrancheCashFlow::ManTrancheFlowType FlwTpe){
			Unmanaged->AddCashFlow(DateTime2QDate(Dte),Amt,static_cast<TrancheCashFlow::TrancheFlowType>(static_cast<int>(FlwTpe)));
		}
		/*!
		\brief Adds the cash flows from another tranche to this one
		\arg a The tranche whose cash flow are to be added
		*/
		void AddCashFlow(ManTrancheCashFlow^ a){
			Unmanaged->AddCashFlow(*(a->Unmanaged));
		}
		/*!
		\brief Removes all the cash flows of the tranche
		*/
		void ResetTrancheFlows(){
			Unmanaged->SetCashFlow(TrancheCashFlow());
		}
		/*!
		\brief The tranche bond factor
		*/
		property double BondFactor{
			double get(){return Unmanaged->GetBondFactor();}
		}
		/*!
		\brief Downloads tranche data from Bloomberg
		\details Downloads from Bloomberg the following informations on the tranche:
		<ul><li>Original Nominal Amount</li>
		<li>Current Outstanding Amount</li>
		<li>Currency</li>
		<li>Last IPD</li>
		<li>Type of interest (fixed or floating)</li>
		<li>Payment Frequency</li>
		<li>Interest Accrued</li>
		<li>Settlement Date</li>
		<li>For floating rate tranches:<ul><li>Base rate ticker</li><li>Spread paid by the note</li></ul></li>
		<li>For fixed rate tranches:<ul><li>Ticker of the base rate used to calculate the discount margin</li><li>Coupon paid by the note</li></ul></li>
		</ul>
		*/
		void GetDataFromBloomberg(){Unmanaged->GetDataFromBloomberg();}
		/*!
		\brief The loss rate on the tranche calculated from cash flows
		*/
		property double LossRate{
			double get(){return Unmanaged->GetLossRate();}
		}
		/*!
		\brief The discount margin of the tranche calculated from the cash flows
		*/
		property double DiscountMargin{
			double get(){return Unmanaged->GetDiscountMargin();}
		}
		/*!
		\brief Calculates the discount margin of the tranche from the cash flows using the specified price
		\arg NewPrice the price used to calculate the discount margin
		*/
		double GetDiscountMargin(double NewPrice){return Unmanaged->GetDiscountMargin(NewPrice);}
		/*!
		\brief The internal rate of return of the tranche calculated from the cash flows
		*/
		property double IRR{
			double get(){return Unmanaged->GetIRR();}
		}
		/*!
		\brief Calculates the internal rate of return of the tranche from the cash flows using the specified price
		\arg NewPrice the price used to calculate the internal rate of return
		*/
		double GetIRR(double NewPrice){return Unmanaged->GetIRR(NewPrice);}
		/*!
		\brief Calculates weighted average life of the notes
		\arg The date from which to calculate the WAL
		*/
		double GetWALife(DateTime StartDate){return Unmanaged->GetWALife(DateTime2QDate(StartDate));}
		/*!
		\brief The tranche settlement date
		*/
		property DateTime SettlementDate{
			DateTime get(){return QDate2DateTime(Unmanaged->GetSettlementDate());}
			void set(DateTime a){Unmanaged->SetSettlementDate(DateTime2QDate(a));}
		}
		
		ManTranche(){Unmanaged=new Tranche();}
		//! Copy constructor
		ManTranche(const ManTranche% a){Unmanaged=new Tranche(*(a.Unmanaged));}
		//! Copy constructor for ref class
		ManTranche(ManTranche^ a){Unmanaged=new Tranche(*(a->Unmanaged));}
		!ManTranche()
		{
			if (Unmanaged){
				delete Unmanaged;
				Unmanaged = NULL;
			}
		}
		~ManTranche()
		{
			this->!ManTranche();
			GC::SuppressFinalize(this);
		}
	};
}
#endif // ManTranche_h__

