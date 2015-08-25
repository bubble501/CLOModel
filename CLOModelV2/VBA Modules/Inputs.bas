Attribute VB_Name = "Inputs"
Option Explicit
Declare Sub RunModel Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant)
Declare Function CLODiscountMargin Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant) As Double
Declare Function CLOWALife Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant) As Double
Declare Function CLOReturnRate Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant) As Double
Declare Function GetStressLoss Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant) As Double
Declare Function GetStressDM Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant) As Double
Declare Function WatFallStepEdit Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant) As String
Declare Function TriggerEdit Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant) As String
Declare Function LoadLoanScenario Lib "Z:\24AM\Analytics\Development\CLOModel2\CLOModelExcel.dll" (ArrayData() As Variant) As String

Public Sub GetInputFromStructure( _
    MortgagesSheet As String, _
    InputsSheet As String, _
    CallerPath As String, _
    FieldsLabels As Collection, _
    Optional SaveBaseCase As Boolean = True, _
    Optional RunStressTest As Boolean = False, _
    Optional StressRowDimension As Long = 0, _
    Optional StressColDimension As Long = 1, _
    Optional StressTestSheet As String _
)
    Application.ScreenUpdating = False
    Dim i As Long, j As Long, propCounter As Long
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    'Fields to search
    Dim LegalFinalCell As Range
    Dim MaturityStart As Range
    Dim CouponStart As Range
    Dim OutstandingStart As Range
    Dim AnnuityStart As Range
    Dim CPRcell As Range
    Dim CDRcell As Range
    Dim LScell As Range
    Dim CutOffcell As Range
    Dim BondStart As Range
    Dim BondRataStart As Range
    Dim BondPriceStart As Range
    Dim OCLimitStart As Range
    Dim ICLimitStart As Range
    Dim TriggerStart As Range
    Dim SeniorExpensesCell As Range
    Dim SeniorFeesCell As Range
    Dim JuniorFeesCell As Range
    Dim SeniorExpensesFixedCell As Range
    Dim SeniorFeesFixedCell As Range
    Dim JuniorFeesFixedCell As Range
    Dim CCCcurveCell As Range
    Dim CCCvalueCell As Range
    Dim CCClimitCell As Range
    Dim ReinvestPeriodCell As Range
    Dim ReinvestLimitCell As Range
    Dim ReinvestTableCell As Range
    Dim ReinvestSpreadCell As Range
    Dim ReinvestWALCell As Range
    Dim InterestBaseCell As Range
    Dim AvailPrincCell As Range
    Dim AvailIntrCell As Range
    Dim StressCPRCell As Range
    Dim StressLSStart As Range
    Dim StressCDRStart As Range
    Dim LossOutputCell As Range
    Dim ExcahngeRateTableStart As Range
    Dim ImpliedCEStart As Range
    Dim JuniorFeesCouponCell As Range
    Dim FrequencyStart As Range
    Dim CurrentOutStart As Range
    Dim OriginalOutStart As Range
    Dim CurrencyStart As Range
    Dim SettleDateCell As Range
    Dim TrancheCouponStart As Range
    Dim RefRateStart As Range
    Dim IPDfrequencyCell As Range
    Dim FirstIPDcell As Range
    Dim ReinvPaymFreqCell As Range
    Dim BaseIndexesStart As Range
    Dim PrevIPDCell As Range
    Dim CallDateCell As Range
    Dim CallValueCell As Range
    Dim UseCallStressCell As Range
    Dim LossToCallCell As Range
    Dim CallMultiplierCell As Range
    Dim CallReserveCell As Range
    'Dim AccruedIntrStart As Range
    Dim IsinFldsStart As Range
    Dim FirstResTargCell As Range
    Dim FirstResMultCell As Range
    Dim FirstResFloorCell As Range
    Dim FirstResCapCell As Range
    Dim FirstResCurrCell As Range
    Dim FirstResFreedCell As Range
    Dim FirstResDestinationCell As Range
    Dim SecondResTargCell As Range
    Dim SecondResMultCell As Range
    Dim SecondResFloorCell As Range
    Dim SecondResCapCell As Range
    Dim SecondResCurrCell As Range
    Dim SecondResFreedCell As Range
    Dim SecondResDestinationCell As Range
    Dim UseCumulativeReservesCell As Range
    Dim DealNameCell As Range
    Dim ReinvestmentDelayCell As Range
    Dim ReinvestmentPriceCell As Range
    Dim UseFastStressCell As Range
    Dim BaseIndexHead As Range
    Dim ReinvRepayCell As Range
    Dim RecovLagCell As Range
    Dim DelinqCell As Range
    Dim DelinqLagCell As Range
    Dim CurrentDeferredCell As Range
    Dim UseForwardCell As Range
    Dim DayCountHead As Range
    Dim BaseCaseCall As Range
    Dim StartingDefJunFees As Range
    Dim GICInterestCell As Range
    Dim GICBaseRateCell As Range
    Dim ReinvestementWindowCell As Range
    Dim DealDaycountCell As Range
    Dim AdditionalCouponBase As Range
    Dim AdditionalRefRateBase As Range
    Dim AdditionalDayCountBase As Range
    'loans assumptions
    Dim IssuerProperty As Range
    Dim FacilityProperty As Range
    Dim CountryProperty As Range
    Dim IndustryProperty As Range
    Dim MezzanineProperty As Range
    Dim PriceProperty As Range
    Dim StartingAdditionalProp As Range
    Dim LoanScenarioProperty As Range
    Dim CPRMultiStart As Range
    Dim HaircutVecStart As Range
    Dim LSMultiStart As Range
    'New Waterfall
    Dim WaterfallSheet As Worksheet
    Dim FirstStep As Range
    Dim LastStep As Range
    Dim SeniorityGroupHead As Long
    Dim SeniorityGroupLevelHead As Long
    Dim AdditionalCollateralShareHead As Long
    Dim RedemptionShareHead As Long
    Dim RedemptionGroupLevelHead As Long
    Dim RedemptionGroupHead As Long
    Dim SourceOfFundingHead As Long
    Dim CouponIndexHead As Long
    Dim TestTargetOverrideHead As Long
    Dim IRRtoEquityTargetHead As Long
    Dim ReserveIndexHead As Long
    Dim TriggersHead As Long
    Dim AccruePayHead As Long
    
    Set WaterfallSheet = Sheets(FieldsLabels("WaterfallSheet"))
    Set FirstStep = WaterfallSheet.Cells.Find(What:=FieldsLabels("StepHead"), LookAt:=xlWhole, LookIn:=xlFormulas)
    Set LastStep = WaterfallSheet.Cells.Find(What:=FieldsLabels("StepHead"), SearchDirection:=xlPrevious, LookAt:=xlWhole, LookIn:=xlFormulas)
    SeniorityGroupHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("SeniorityGroupHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    SeniorityGroupLevelHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("SeniorityGroupLevelHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    RedemptionGroupHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("RedemptionGroupHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    RedemptionGroupLevelHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("RedemptionGroupLevelHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    RedemptionShareHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("RedemptionShareHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    AdditionalCollateralShareHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("AdditionalCollateralShareHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    SourceOfFundingHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("SourceOfFundingHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    CouponIndexHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("CouponIndexHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    TestTargetOverrideHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("TestTargetOverrideHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    IRRtoEquityTargetHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("IRRtoEquityTargetHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    ReserveIndexHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("ReserveIndexHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    TriggersHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("TriggersHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    AccruePayHead = WaterfallSheet.Cells.Find(What:=FieldsLabels("AccruePayHead"), LookAt:=xlWhole, LookIn:=xlFormulas).Column - FirstStep.Column
    ''''''''''''''''''''''''''''''''''''''''''''''''''''
    On Error Resume Next
    Set IssuerProperty = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("IssuerProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FacilityProperty = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("FacilityProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CountryProperty = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("CountryProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set IndustryProperty = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("IndustryProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set MezzanineProperty = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("MezzanineProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set PriceProperty = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("PriceProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set StartingAdditionalProp = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("StartingAdditionalProp"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LoanScenarioProperty = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("LoanScenarioProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    ''''''''''''''''''''''''''''''''''''''''''''''''''''
    Set HaircutVecStart = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("HaircutVecHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set TriggerStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("TriggerStart"), LookAt:=xlWhole, LookIn:=xlValues)
    Set StartingDefJunFees = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("StartingDefJunFees"), LookAt:=xlWhole, LookIn:=xlValues)
    Set GICInterestCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("GICInterestCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set BaseCaseCall = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("BaseCaseCall"), LookAt:=xlWhole, LookIn:=xlValues)
    Set IsinFldsStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("IsinFldsHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResTargCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("FirstResTargCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResMultCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("FirstResMultCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResFloorCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("FirstResFloorCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResCapCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("FirstResCapCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResCurrCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("FirstResCurrCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResFreedCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("FirstResFreedCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResDestinationCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("FirstResDestinationCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResTargCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SecondResTargCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResMultCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SecondResMultCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResFloorCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SecondResFloorCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResCapCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SecondResCapCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResCurrCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SecondResCurrCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResFreedCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SecondResFreedCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResDestinationCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SecondResDestinationCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set UseCumulativeReservesCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("UseCumulativeReservesCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestmentDelayCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvestmentDelayCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestmentPriceCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvestmentPriceCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set DealNameCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("DealNameCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set UseFastStressCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("UseFastStressCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SeniorExpensesFixedCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SeniorExpensesFieldFixed"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SeniorFeesFixedCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SeniorFeesFieldFixed"), LookAt:=xlWhole, LookIn:=xlValues)
    Set JuniorFeesFixedCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("JuniorFeesFieldFixed"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvRepayCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvRepayCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set RecovLagCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("RecovLagCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set DelinqCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("DelinqCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set DelinqLagCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("DelinqLagCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CurrentDeferredCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CurrentDeferredCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set UseForwardCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("UseForwardCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set DayCountHead = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("DayCountHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set GICBaseRateCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("GICBaseRateCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set DealDaycountCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("DealDaycountCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestementWindowCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvestementWindowCell"), LookAt:=xlWhole, LookIn:=xlValues)
    On Error GoTo 0
    Set BaseIndexHead = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("BaseIndexHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set MaturityStart = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("MaturityHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CouponStart = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("CouponHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set OutstandingStart = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("OutstandingHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set AnnuityStart = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("AnnuityHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FrequencyStart = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("FrequencyHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CPRMultiStart = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("CPRMultiHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LSMultiStart = Sheets(MortgagesSheet).Cells.Find(What:=FieldsLabels("LSMultiHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CPRcell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CPRfield"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CDRcell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CDRfield"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LScell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("LSfield"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CurrentOutStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CurrentOutHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set OriginalOutStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("OriginalOutHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CutOffcell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CutOffField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set BondStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("BondsNamesHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set BondRataStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("BondRataGroupHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set BondPriceStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("BondPriceHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set OCLimitStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("OCLimitHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ICLimitStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ICLimitHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set IPDfrequencyCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("IPDfrequencyField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstIPDcell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("FirstIPDfield"), LookAt:=xlWhole, LookIn:=xlValues)
    Set PrevIPDCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("PrevIPDField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CurrencyStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CurrencyHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LossOutputCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("LossOutputHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LossToCallCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("LossToCallField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SettleDateCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SettleDateField"), LookAt:=xlWhole, LookIn:=xlValues)
    'Set AccruedIntrStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("IntrAccrHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set TrancheCouponStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("TrancheCouponHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set RefRateStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("RefRateHead"), LookAt:=xlWhole, LookIn:=xlValues)
    'Set WaterfallStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("WaterfallHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SeniorExpensesCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SeniorExpensesField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SeniorFeesCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("SeniorFeesField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set JuniorFeesCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("JuniorFeesField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CCCcurveCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CCCcurveField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CCCvalueCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CCCvalueField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CCClimitCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CCClimitField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CallDateCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CallDateField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CallValueCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CallValueField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CallMultiplierCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CallMultiplierField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CallReserveCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CallReserveField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LegalFinalCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("LegalFinalCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set UseCallStressCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("UseCallStressField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestPeriodCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvestPeriodField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestLimitCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvestLimitField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestTableCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvestTableHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestSpreadCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvestSpreadField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvPaymFreqCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvPaymFreqField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestWALCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ReinvestWALField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set InterestBaseCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("InterestBaseField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set AvailPrincCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("AvailPrincField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set AvailIntrCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("AvailIntrField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ExcahngeRateTableStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("ExcahngeRateTableHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ExcahngeRateTableStart = Range(ExcahngeRateTableStart, ExcahngeRateTableStart.End(xlDown).Offset(0, 1))
    Set JuniorFeesCouponCell = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("JuniorFeesCouponField"), LookAt:=xlWhole, LookIn:=xlValues)
    If (RunStressTest) Then
        Set StressCPRCell = Sheets(StressTestSheet).Cells.Find(What:=FieldsLabels("StressCPRField"), LookAt:=xlWhole, LookIn:=xlFormulas)
        Set StressCDRStart = Sheets(StressTestSheet).Cells.Find(What:=FieldsLabels("StressCDRHead"), LookAt:=xlWhole, LookIn:=xlFormulas)
        Set StressLSStart = Sheets(StressTestSheet).Cells.Find(What:=FieldsLabels("StressLSHead"), LookAt:=xlWhole, LookIn:=xlFormulas)
    End If
    On Error Resume Next
        Set BaseIndexesStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("BaseIndexesHead"), LookAt:=xlWhole, LookIn:=xlValues)
    On Error GoTo 0
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

    Dim tempReferenceRate As Double
    On Error GoTo ReferenceRateFromBBg
        tempReferenceRate = Application.WorksheetFunction.VLookup(InterestBaseCell.Offset(0, 1).Value, Range(BaseIndexesStart, BaseIndexesStart.End(xlDown).Offset(0, 1)), 2, False)
        If False Then
ReferenceRateFromBBg:
            'tempReferenceRate = bdp24(UCase(InterestBaseCell.Offset(0, 1).Value) + " Index", "PX_LAST") / 100
            Resume Next
        End If
    On Error GoTo 0
    
    Dim AllTheInputs As New Collection
    AllTheInputs.Add False 'Switch to true to save the inputs
    i = 1
    j = 0
    Do While (True)
        If (IsEmpty(MaturityStart.Offset(i, 0)) Or MaturityStart.Offset(i, 0).Value = "") Then Exit Do
        If (OutstandingStart.Offset(i, 0).Value > 0) Then j = j + 1
        i = i + 1
    Loop
    AllTheInputs.Add j
    i = 1
    Do While (True)
        If (IsEmpty(MaturityStart.Offset(i, 0)) Or MaturityStart.Offset(i, 0).Value = "") Then Exit Do
        If (OutstandingStart.Offset(i, 0).Value > 0) Then
            Call AddInput(AllTheInputs, Format(MaturityStart.Offset(i, 0).Value, "yyyy-mm-dd"))
            Call AddInput(AllTheInputs, OutstandingStart.Offset(i, 0).Value)
            Call AddInput(AllTheInputs, CStr(CouponStart.Offset(i, 0).Value))
            If IsEmpty(BaseIndexHead.Offset(i, 0)) Then
                Call AddInput(AllTheInputs, "ZERO")
            Else
                Call AddInput(AllTheInputs, CStr(BaseIndexHead.Offset(i, 0).Value))
            End If
            Call AddInput(AllTheInputs, CStr(AnnuityStart.Offset(i, 0).Value))
            Call AddInput(AllTheInputs, CStr(FrequencyStart.Offset(i, 0).Value))
            'Loans Properties
            Dim PropertyString As String
            PropertyString = ""
            If Not (LoanScenarioProperty Is Nothing) Then PropertyString = PropertyString & "Scenario#=#" & CStr(LoanScenarioProperty.Offset(i, 0).Value) & "#,#"
            If Not (CPRMultiStart Is Nothing) Then PropertyString = PropertyString & "PrepayMultiplier#=#" & CStr(CPRMultiStart.Offset(i, 0).Value) & "#,#"
            If Not (CPRMultiStart Is Nothing) Then PropertyString = PropertyString & "LossMultiplier#=#" & CStr(LSMultiStart.Offset(i, 0).Value) & "#,#"
            If Not (HaircutVecStart Is Nothing) Then PropertyString = PropertyString & "Haircut#=#" & CStr(HaircutVecStart.Offset(i, 0).Value) & "#,#"
            If Not (IssuerProperty Is Nothing) Then PropertyString = PropertyString & "Issuer#=#" & CStr(IssuerProperty.Offset(i, 0).Value) & "#,#"
            If Not (FacilityProperty Is Nothing) Then PropertyString = PropertyString & "Facility#=#" & CStr(FacilityProperty.Offset(i, 0).Value) & "#,#"
            If Not (CountryProperty Is Nothing) Then PropertyString = PropertyString & "Country#=#" & CStr(CountryProperty.Offset(i, 0).Value) & "#,#"
            If Not (IndustryProperty Is Nothing) Then PropertyString = PropertyString & "Industry#=#" & CStr(IndustryProperty.Offset(i, 0).Value) & "#,#"
            If Not (MezzanineProperty Is Nothing) Then PropertyString = PropertyString & "Mezzanine#=#" & CStr(MezzanineProperty.Offset(i, 0).Value) & "#,#"
            If Not (PriceProperty Is Nothing) Then PropertyString = PropertyString & "Price#=#" & CStr(PriceProperty.Offset(i, 0).Value) & "#,#"
            If Not (StartingAdditionalProp Is Nothing) Then
                propCounter = 1
                Do While (True)
                    If Not (IsEmpty(StartingAdditionalProp.Offset(i, propCounter)) Or StartingAdditionalProp.Offset(i, propCounter).Value = "") Then
                        PropertyString = PropertyString & _
                                        Replace(CStr(StartingAdditionalProp.Offset(0, propCounter).Value), " ", "") & _
                                        "#=#" & _
                                        CStr(StartingAdditionalProp.Offset(i, propCounter).Value) & "#,#"
                    End If
                    If (IsEmpty(StartingAdditionalProp.Offset(0, propCounter)) Or StartingAdditionalProp.Offset(0, propCounter).Value = "") Then Exit Do
                    propCounter = propCounter + 1
                Loop
            End If
            Call AddInput(AllTheInputs, Mid(PropertyString, 1, Len(PropertyString) - 3))
        End If
        i = i + 1
    Loop
    If (IsEmpty(MaturityStart.Offset(1, 0))) Then
        Call AddInput(AllTheInputs, CLng(0))
    Else
        Call AddInput(AllTheInputs, CLng(BondStart.End(xlDown).Row - BondStart.Row))
    End If
    Dim NumOfTrancheCoupon As Long
    NumOfTrancheCoupon = 1
    Do While (True)
        On Error Resume Next
        Set AdditionalCouponBase = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("AdditionalCouponBase") + Format(NumOfTrancheCoupon + 1, "0"), LookAt:=xlWhole, LookIn:=xlValues)
        Set AdditionalRefRateBase = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("AdditionalRefRateBase") + Format(NumOfTrancheCoupon + 1, "0"), LookAt:=xlWhole, LookIn:=xlValues)
        Set AdditionalDayCountBase = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("AdditionalDayCountBase") + Format(NumOfTrancheCoupon + 1, "0"), LookAt:=xlWhole, LookIn:=xlValues)
        On Error GoTo 0
        If AdditionalCouponBase Is Nothing Or AdditionalRefRateBase Is Nothing Or AdditionalDayCountBase Is Nothing Then Exit Do
        NumOfTrancheCoupon = NumOfTrancheCoupon + 1
    Loop
    
    Dim AdditionalIter As Long
    i = 1
    Do While (True)
        If IsEmpty(BondStart.Offset(i, 0)) Then Exit Do
        Call AddInput(AllTheInputs, CStr(BondStart.Offset(i, 0).Value))
        If (IsinFldsStart Is Nothing) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(IsinFldsStart.Offset(i, 0).Value))
        End If
        Call AddInput(AllTheInputs, CStr(BondRataStart.Offset(i, 0).Value))
        Call AddInput(AllTheInputs, OriginalOutStart.Offset(i, 0).Value)
        Call AddInput(AllTheInputs, CStr(CurrencyStart.Offset(i, 0).Value))
        Call AddInput(AllTheInputs, CurrentOutStart.Offset(i, 0).Value)
        Call AddInput(AllTheInputs, NumOfTrancheCoupon)
        If CStr(TrancheCouponStart.Offset(i, 0).Value) = "" Then
            Call AddInput(AllTheInputs, "0")
        Else
            Call AddInput(AllTheInputs, CStr(TrancheCouponStart.Offset(i, 0).Value))
        End If
        AdditionalIter = 2
        While (AdditionalIter <= NumOfTrancheCoupon)
            Set AdditionalCouponBase = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("AdditionalCouponBase") + Format(AdditionalIter, "0"), LookAt:=xlWhole, LookIn:=xlValues)
            If CStr(AdditionalCouponBase.Offset(i, 0).Value) = "" Then
                Call AddInput(AllTheInputs, "0")
            Else
                Call AddInput(AllTheInputs, CStr(AdditionalCouponBase.Offset(i, 0).Value))
            End If
            AdditionalIter = AdditionalIter + 1
        Wend
        Call AddInput(AllTheInputs, NumOfTrancheCoupon)
        If CStr(RefRateStart.Offset(i, 0).Value) = "" Then
            Call AddInput(AllTheInputs, "ZERO")
        Else
            Call AddInput(AllTheInputs, CStr(RefRateStart.Offset(i, 0).Value))
        End If
        AdditionalIter = 2
        While (AdditionalIter <= NumOfTrancheCoupon)
            Set AdditionalRefRateBase = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("AdditionalRefRateBase") + Format(AdditionalIter, "0"), LookAt:=xlWhole, LookIn:=xlValues)
            If CStr(AdditionalRefRateBase.Offset(i, 0).Value) = "" Then
                Call AddInput(AllTheInputs, "ZERO")
            Else
                Call AddInput(AllTheInputs, CStr(AdditionalRefRateBase.Offset(i, 0).Value))
            End If
            AdditionalIter = AdditionalIter + 1
        Wend
        Call AddInput(AllTheInputs, Format(PrevIPDCell.Offset(0, 1).Value, "yyyy-mm-dd"))
        Call AddInput(AllTheInputs, CStr(InterestBaseCell.Offset(0, 1).Value))
        Call AddInput(AllTheInputs, CStr(IPDfrequencyCell.Offset(0, 1)))
        Call AddInput(AllTheInputs, OCLimitStart.Offset(i, 0).Value)
        Call AddInput(AllTheInputs, ICLimitStart.Offset(i, 0).Value)
        Call AddInput(AllTheInputs, CDbl(BondPriceStart.Offset(i, 0).Value))
        On Error GoTo DefaultExchange
        Call AddInput(AllTheInputs, Application.WorksheetFunction.VLookup(CurrencyStart.Offset(i, 0).Value, ExcahngeRateTableStart, 2, False))
        If False Then
DefaultExchange:
            Call AddInput(AllTheInputs, 1#)
            Resume Next
        End If
        On Error GoTo 0
        Call AddInput(AllTheInputs, Format(SettleDateCell.Offset(0, 1).Value, "yyyy-mm-dd"))
        'Call AddInput(AllTheInputs, AccruedIntrStart.Offset(i, 0).Value)
        Call AddInput(AllTheInputs, CLng(NumOfTrancheCoupon))
        If (DayCountHead Is Nothing) Then
            Call AddInput(AllTheInputs, "102")
        Else
            Call AddInput(AllTheInputs, CStr(DayCountHead.Offset(i, 0).Value))
        End If
        AdditionalIter = 2
        While (AdditionalIter <= NumOfTrancheCoupon)
            Set AdditionalDayCountBase = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("AdditionalDayCountBase") + Format(AdditionalIter, "0"), LookAt:=xlWhole, LookIn:=xlValues)
            If CStr(AdditionalDayCountBase.Offset(i, 0).Value) = "" Then
                Call AddInput(AllTheInputs, "102")
            Else
                Call AddInput(AllTheInputs, CStr(AdditionalDayCountBase.Offset(i, 0).Value))
            End If
            AdditionalIter = AdditionalIter + 1
        Wend
        If (CurrentDeferredCell Is Nothing) Then
            Call AddInput(AllTheInputs, 0#)
        Else
            Call AddInput(AllTheInputs, CurrentDeferredCell.Offset(i, 0).Value)
        End If
        
        i = i + 1
    Loop

    'New Waterfall
    If (IsEmpty(FirstStep.Offset(1, 0)) And IsEmpty(LastStep.Offset(1, 0))) Then
        Call AddInput(AllTheInputs, CLng(0))
    Else
        Call AddInput(AllTheInputs, CLng(FirstStep.End(xlDown).Row - FirstStep.Row + LastStep.End(xlDown).Row - LastStep.Row))
    End If
    i = 1
    'Interest Waterfall
    Do While (True)
        If IsEmpty(FirstStep.Offset(i, 0)) Then Exit Do
        Call AddInput(AllTheInputs, FromStringToPriorty(FirstStep.Offset(i, 0).Value))
        If IsEmpty(FirstStep.Offset(i, SeniorityGroupHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, SeniorityGroupHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, SeniorityGroupLevelHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, SeniorityGroupLevelHead).Value))
        End If
         If IsEmpty(FirstStep.Offset(i, RedemptionGroupHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, RedemptionGroupHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, RedemptionGroupLevelHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, RedemptionGroupLevelHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, RedemptionShareHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, RedemptionShareHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, AdditionalCollateralShareHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, AdditionalCollateralShareHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, SourceOfFundingHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, SourceOfFundingHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, CouponIndexHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, CouponIndexHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, TestTargetOverrideHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, TestTargetOverrideHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, IRRtoEquityTargetHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, IRRtoEquityTargetHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, ReserveIndexHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, ReserveIndexHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, TriggersHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(FirstStep.Offset(i, TriggersHead).Value))
        End If
        If IsEmpty(FirstStep.Offset(i, AccruePayHead)) Then
            Call AddInput(AllTheInputs, CLng(0))
        Else
            Call AddInput(AllTheInputs, CLng(FirstStep.Offset(i, AccruePayHead).Value))
        End If
        i = i + 1
    Loop
    'Principal waterfall
    i = 1
    Do While (True)
        If IsEmpty(LastStep.Offset(i, 0)) Then Exit Do
        Call AddInput(AllTheInputs, FromStringToPriorty(LastStep.Offset(i, 0).Value))
        If IsEmpty(LastStep.Offset(i, SeniorityGroupHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, SeniorityGroupHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, SeniorityGroupLevelHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, SeniorityGroupLevelHead).Value))
        End If
         If IsEmpty(LastStep.Offset(i, RedemptionGroupHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, RedemptionGroupHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, RedemptionGroupLevelHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, RedemptionGroupLevelHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, RedemptionShareHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, RedemptionShareHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, AdditionalCollateralShareHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, AdditionalCollateralShareHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, SourceOfFundingHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, SourceOfFundingHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, CouponIndexHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, CouponIndexHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, TestTargetOverrideHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, TestTargetOverrideHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, IRRtoEquityTargetHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, IRRtoEquityTargetHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, ReserveIndexHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, ReserveIndexHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, TriggersHead)) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(LastStep.Offset(i, TriggersHead).Value))
        End If
        If IsEmpty(LastStep.Offset(i, AccruePayHead)) Then
            Call AddInput(AllTheInputs, CLng(0))
        Else
            Call AddInput(AllTheInputs, CLng(LastStep.Offset(i, AccruePayHead).Value))
        End If
        i = i + 1
    Loop
    
    'Triggers
    i = 2
    Do While True
        If IsEmpty(TriggerStart.Offset(i, 0)) Then Exit Do
        i = i + 1
    Loop
    Call AddInput(AllTheInputs, CLng(i - 2))
    i = 2
    Do While True
        If IsEmpty(TriggerStart.Offset(i, 0)) Then Exit Do
        Select Case FromStringToTriggerType(TriggerStart.Offset(i, 0).Value)
            Case 0 'Date Trigger
                Call AddInput(AllTheInputs, CLng(0))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, Format(TriggerStart.Offset(i, 2).Value, "yyyy-mm-dd"))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 3).Value))
            Case 1 'Vector Trigger
                Call AddInput(AllTheInputs, CLng(1))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
            Case 2 'Pool Size Trigger
                Call AddInput(AllTheInputs, CLng(2))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 3).Value))
            Case 3 'Tranche Trigger
                Call AddInput(AllTheInputs, CLng(3))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 3).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 4).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 5).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 6).Value))
                Call AddInput(AllTheInputs, CDbl(TriggerStart.Offset(i, 7).Value))
            Case 4 'Delinquency Trigger
                Call AddInput(AllTheInputs, CLng(4))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
            Case 5 'During Stress Test Trigger
                Call AddInput(AllTheInputs, CLng(5))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
            Case 6 'Cumulative Loss Trigger
                Call AddInput(AllTheInputs, CLng(6))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 3).Value))
            Case 7 'Deferred Interest Trigger
                Call AddInput(AllTheInputs, CLng(7))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 3).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 4).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 5).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 6).Value))
                Call AddInput(AllTheInputs, CDbl(TriggerStart.Offset(i, 7).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 8).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 9).Value))
            Case 8 'PDL Trigger
                Call AddInput(AllTheInputs, CLng(8))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 3).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 4).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 5).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 6).Value))
                Call AddInput(AllTheInputs, CDbl(TriggerStart.Offset(i, 7).Value))
            Case Else
                Call MsgBox("Unhandled Trigger Type", vbCritical, "Error")
                Exit Sub
        End Select
        i = i + 1
    Loop
'    If (IsEmpty(WaterfallStart.Offset(2, 0))) Then
'        Call AddInput(AllTheInputs, CLng(0))
'    Else
'        Call AddInput(AllTheInputs, CLng(WaterfallStart.Offset(2, 0).End(xlDown).Row - WaterfallStart.Row - 1))
'    End If
'    i = 1
'    Do While (True)
'        If IsEmpty(WaterfallStart.Offset(i + 1, 0)) Then Exit Do
'        Call AddInput(AllTheInputs, FromStringToPriorty(WaterfallStart.Offset(i + 1, 0).Value))
'        Call AddInput(AllTheInputs, CLng(WaterfallStart.Offset(i + 1, 1).Value))
'        Call AddInput(AllTheInputs, CLng(WaterfallStart.Offset(i + 1, 2).Value))
'        Call AddInput(AllTheInputs, WaterfallStart.Offset(i + 1, 3).Value)
'        i = i + 1
'    Loop
    
    
    If (DealNameCell Is Nothing) Then
        Call AddInput(AllTheInputs, "")
    Else
        Call AddInput(AllTheInputs, CStr(DealNameCell.Offset(0, 1).Value))
    End If
    If (StartingDefJunFees Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(StartingDefJunFees.Offset(0, 1).Value))
    End If
    If (GICInterestCell Is Nothing) Then
        Call AddInput(GICInterestCell, "0")
    Else
        Call AddInput(AllTheInputs, CStr(GICInterestCell.Offset(0, 1).Value))
    End If
    If (GICBaseRateCell Is Nothing) Then
        Call AddInput(AllTheInputs, "ZERO")
    Else
        Call AddInput(AllTheInputs, CStr(GICBaseRateCell.Offset(0, 1).Value))
    End If
    If (DealDaycountCell Is Nothing) Then
        Call AddInput(AllTheInputs, "102")
    Else
        Call AddInput(AllTheInputs, CStr(DealDaycountCell.Offset(0, 1).Value))
    End If
    Call AddInput(AllTheInputs, CStr(SeniorExpensesCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(SeniorFeesCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(JuniorFeesCell.Offset(0, 1).Value))
    If (SeniorExpensesFixedCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(SeniorExpensesFixedCell.Offset(0, 1).Value))
    End If
    If (SeniorFeesFixedCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(SeniorFeesFixedCell.Offset(0, 1).Value))
    End If
    If (JuniorFeesFixedCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(JuniorFeesFixedCell.Offset(0, 1).Value))
    End If
    Call AddInput(AllTheInputs, JuniorFeesCouponCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CStr(IPDfrequencyCell.Offset(0, 1)))
    Call AddInput(AllTheInputs, Format(FirstIPDcell.Offset(0, 1), "yyyy-mm-dd"))
    Call AddInput(AllTheInputs, Format(PrevIPDCell.Offset(0, 1), "yyyy-mm-dd"))
    Call AddInput(AllTheInputs, CStr(CCCcurveCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CCCvalueCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CCClimitCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, Format(CallDateCell.Offset(0, 1).Value, "yyyy-mm-dd"))
    Call AddInput(AllTheInputs, CallMultiplierCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CallReserveCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CStr(CallValueCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, Format(LegalFinalCell.Offset(0, 1).Value, "yyyy-mm-dd"))
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    'Reserve fund
    Call AddInput(AllTheInputs, CLng(2))
    If (FirstResTargCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(FirstResTargCell.Offset(0, 1).Value))
    End If
    If (FirstResMultCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(FirstResMultCell.Offset(0, 1).Value))
    End If
    If (FirstResFloorCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(FirstResFloorCell.Offset(0, 1).Value))
    End If
    If (FirstResCapCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(FirstResCapCell.Offset(0, 1).Value))
    End If
    If (FirstResCurrCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(FirstResCurrCell.Offset(0, 1).Value))
    End If
    If (FirstResFreedCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0)
    Else
        Call AddInput(AllTheInputs, CLng(FirstResFreedCell.Offset(0, 1).Value))
    End If
    If (FirstResDestinationCell Is Nothing) Then
        Call AddInput(AllTheInputs, True)
    Else
        Call AddInput(AllTheInputs, 1 = FirstResDestinationCell.Offset(0, 1).Value)
    End If
    If (SecondResTargCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(SecondResTargCell.Offset(0, 1).Value))
    End If
    If (SecondResMultCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(SecondResMultCell.Offset(0, 1).Value))
    End If
    If (SecondResFloorCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(SecondResFloorCell.Offset(0, 1).Value))
    End If
    If (SecondResCapCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(SecondResCapCell.Offset(0, 1).Value))
    End If
     If (SecondResCurrCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(SecondResCurrCell.Offset(0, 1).Value))
    End If
    If (SecondResFreedCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0)
    Else
        Call AddInput(AllTheInputs, CLng(SecondResFreedCell.Offset(0, 1).Value))
    End If
    If (SecondResDestinationCell Is Nothing) Then
        Call AddInput(AllTheInputs, True)
    Else
        Call AddInput(AllTheInputs, 1 = SecondResDestinationCell.Offset(0, 1).Value)
    End If
    If (UseCumulativeReservesCell Is Nothing) Then
        Call AddInput(AllTheInputs, False)
    Else
        Call AddInput(AllTheInputs, CBool(UseCumulativeReservesCell.Offset(0, 1).Value))
    End If
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    If (BaseCaseCall Is Nothing) Then
        Call AddInput(AllTheInputs, True)
    Else
        Call AddInput(AllTheInputs, CBool(BaseCaseCall.Offset(0, 1).Value))
    End If
    Call AddInput(AllTheInputs, SaveBaseCase)
    Call AddInput(AllTheInputs, Format(ReinvestPeriodCell.Offset(0, 1).Value, "yyyy-mm-dd"))
    'Call AddInput(AllTheInputs, ReinvestLimitCell.Offset(0, 1).Value)
    'Call AddInput(AllTheInputs, ReinvestTableCell.Offset(1, 1).Value)
    'Call AddInput(AllTheInputs, ReinvestTableCell.Offset(1, 2).Value)
    'Call AddInput(AllTheInputs, ReinvestTableCell.Offset(2, 1).Value)
    'Call AddInput(AllTheInputs, ReinvestTableCell.Offset(2, 2).Value)
    Call AddInput(AllTheInputs, CStr(ReinvestSpreadCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(CPRcell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(CDRcell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(LScell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(ReinvestWALCell.Offset(0, 1).Value))
    If (ReinvestmentDelayCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(ReinvestmentDelayCell.Offset(0, 1).Value))
    End If
    If (ReinvestementWindowCell Is Nothing) Then
        Call AddInput(AllTheInputs, "1")
    Else
        Call AddInput(AllTheInputs, CStr(ReinvestementWindowCell.Offset(0, 1).Value))
    End If
    
    If (ReinvestmentPriceCell Is Nothing) Then
        Call AddInput(AllTheInputs, "100")
    Else
        Call AddInput(AllTheInputs, CStr(ReinvestmentPriceCell.Offset(0, 1).Value))
    End If
    Call AddInput(AllTheInputs, CStr(ReinvPaymFreqCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(InterestBaseCell.Offset(0, 1).Value))
    If (ReinvRepayCell Is Nothing) Then
        Call AddInput(AllTheInputs, "I")
    Else
        Call AddInput(AllTheInputs, CStr(ReinvRepayCell.Offset(0, 1).Value))
    End If
    If (RecovLagCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(RecovLagCell.Offset(0, 1).Value))
    End If
    If (DelinqCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(DelinqCell.Offset(0, 1).Value))
    End If
    If (DelinqLagCell Is Nothing) Then
        Call AddInput(AllTheInputs, "0")
    Else
        Call AddInput(AllTheInputs, CStr(DelinqLagCell.Offset(0, 1).Value))
    End If
    Call AddInput(AllTheInputs, CDbl(AvailPrincCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CDbl(AvailIntrCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, Format(CutOffcell.Offset(0, 1).Value, "yyyy-mm-dd"))
    Dim RunCallScenario As Boolean
    RunCallScenario = _
        (Not IsEmpty(CallDateCell.Offset(0, 1))) _
        Or ((Not IsEmpty(CallReserveCell.Offset(0, 1))) And (Not IsEmpty(CallMultiplierCell.Offset(0, 1))))
    Call AddInput(AllTheInputs, RunCallScenario)
    Call AddInput(AllTheInputs, FieldsLabels("MtgOutputCell"))
    Call AddInput(AllTheInputs, FieldsLabels("TranchesOutputCell"))
    Call AddInput(AllTheInputs, CStr(FieldsLabels("PlotsSheet")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("MtgPlotIndex")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("OutstandingPlotIndex")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("OCPlotIndex")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("ICPlotIndex")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("CostFundingPlotIndex")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("AnnExcessPlotIndex")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("CPRLSPlotIndex")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("CallToEquityPlotIndex")))
    Call AddInput(AllTheInputs, CLng(FieldsLabels("EquityReturnPlotIndex")))
    Call AddInput(AllTheInputs, InputsSheet + "!" + Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("LossOutputCell"), LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0).Address)
    Call AddInput(AllTheInputs, InputsSheet + "!" + Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("LossCallOutputCell"), LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0).Address)
    Call AddInput(AllTheInputs, InputsSheet + "!" + Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("CEcell"), LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0).Address)
    Call AddInput(AllTheInputs, CallerPath)
    
    Dim LocalUseFwd As Boolean
    If (UseForwardCell Is Nothing) Then
        Call AddInput(AllTheInputs, False)
        LocalUseFwd = False
    Else
        Call AddInput(AllTheInputs, CBool(UseForwardCell.Offset(0, 1).Value))
        LocalUseFwd = CBool(UseForwardCell.Offset(0, 1).Value)
    End If
    
    On Error GoTo ExtraSheetMissing
    If (Sheets(FieldsLabels("ForwardCurvesSheet")).Visible = xlSheetVisible) Then
        Dim CountFwd As Long
        Dim countDates As Long
        CountFwd = 0
        Do While (True)
            If (IsEmpty(Sheets(FieldsLabels("ForwardCurvesSheet")).Cells(3, (CountFwd * 2) + 1))) Then Exit Do
            CountFwd = CountFwd + 1
        Loop
        Call AddInput(AllTheInputs, CountFwd)
        CountFwd = 0
        Do While (True)
            If (IsEmpty(Sheets(FieldsLabels("ForwardCurvesSheet")).Cells(3, (CountFwd * 2) + 1))) Then Exit Do
            Call AddInput(AllTheInputs, CStr(Sheets(FieldsLabels("ForwardCurvesSheet")).Cells(1, (CountFwd * 2) + 1)))
            If LocalUseFwd Then
                countDates = 0
                Do While (True)
                    If IsEmpty(Sheets(FieldsLabels("ForwardCurvesSheet")).Cells(4 + countDates, (CountFwd * 2) + 1)) Then Exit Do
                    countDates = countDates + 1
                Loop
                Call AddInput(AllTheInputs, countDates)
                countDates = 0
                Do While (True)
                    If IsEmpty(Sheets(FieldsLabels("ForwardCurvesSheet")).Cells(4 + countDates, (CountFwd * 2) + 1)) Then Exit Do
                    Call AddInput(AllTheInputs, Format(Sheets(FieldsLabels("ForwardCurvesSheet")).Cells(4 + countDates, (CountFwd * 2) + 1).Value, "yyyy-mm-dd"))
                    Call AddInput(AllTheInputs, CDbl(Sheets(FieldsLabels("ForwardCurvesSheet")).Cells(4 + countDates, (CountFwd * 2) + 2).Value))
                    countDates = countDates + 1
                Loop
            Else
                Call AddInput(AllTheInputs, CDbl(Sheets(FieldsLabels("ForwardCurvesSheet")).Cells(3, (CountFwd * 2) + 2).Value))
            End If
            CountFwd = CountFwd + 1
        Loop
    Else
        Call AddInput(AllTheInputs, CLng(0))
    End If
    If False Then
ExtraSheetMissing:
        Call AddInput(AllTheInputs, CLng(0))
        Resume Next
    End If
    On Error GoTo 0
    Call AddInput(AllTheInputs, RunStressTest)
    If RunStressTest Then
        Call AddInput(AllTheInputs, CStr(StressCPRCell.Offset(1, 0).Value))
        Call AddInput(AllTheInputs, StressRowDimension)
        Call AddInput(AllTheInputs, StressColDimension)
        i = 0
        Do While (True)
            If IsEmpty(StressCDRStart.Offset(0, 1).Offset(i, 0)) Then Exit Do
            i = i + 1
        Loop
        Call AddInput(AllTheInputs, i)
        i = 0
        Do While (True)
            If IsEmpty(StressCDRStart.Offset(0, 1).Offset(i, 0)) Then Exit Do
            Call AddInput(AllTheInputs, CStr(StressCDRStart.Offset(0, 1).Offset(i, 0).Value))
            i = i + 1
        Loop
        i = 0
        Do While (True)
            If IsEmpty(StressLSStart.Offset(1, 0).Offset(0, i)) Then Exit Do
            i = i + 1
        Loop
        Call AddInput(AllTheInputs, i)
        i = 0
        Do While (True)
            If IsEmpty(StressLSStart.Offset(1, 0).Offset(0, i)) Then Exit Do
            Call AddInput(AllTheInputs, CStr(StressLSStart.Offset(1, 0).Offset(0, i).Value))
            i = i + 1
        Loop
        Call AddInput(AllTheInputs, UseCallStressCell.Offset(0, 1).Value)
        If (UseFastStressCell Is Nothing) Then
            Call AddInput(AllTheInputs, False)
        Else
            Call AddInput(AllTheInputs, UseFastStressCell.Offset(0, 1).Value)
        End If
    End If
    Dim result() As Variant
    ReDim result(0 To AllTheInputs.Count - 1)
    For i = 1 To AllTheInputs.Count
        result(i - 1) = AllTheInputs(i)
    Next i
    Call RunModel(result)
End Sub

'Private Function FromCollToVector(ByRef TheCol As Collection) As Variant()
'    Dim Result() As Variant
'    ReDim Result(0 To TheCol.Count - 1, 0 To 0)
'    Dim i As Long
'    For i = 1 To TheCol.Count
'        Result(i - 1 + LBound(Result, 1), LBound(Result, 2)) = TheCol(i)
'    Next i
'    FromCollToVector = Result
'End Function
Private Sub AddInput(ByRef DestVect As Collection, NewVal As Variant)
    DestVect.Add NewVal
End Sub


Public Sub PopulateDafaultLabels(ByRef a As Collection, Optional ClearAll As Boolean = True)
    If (ClearAll) Then
        While (a.Count > 0)
            a.Remove 1
        Wend
    End If
    a.Add "Adjusted Maturity", "MaturityHeader"
    a.Add "Coupon", "CouponHeader"
    a.Add "Adjusted Size", "OutstandingHeader"
    a.Add "Annuity", "AnnuityHeader"
    a.Add "Payment Frequency (in Months)", "FrequencyHeader"
   
    a.Add "CPR", "CPRfield"
    a.Add "CDR", "CDRfield"
    a.Add "Original Amount", "OriginalOutHead"
    a.Add "LS", "LSfield"
    a.Add "Pool cut off date", "CutOffField"
    a.Add "Outstanding Amount", "CurrentOutHead"
    a.Add "Bond", "BondsNamesHeader"
    a.Add "ISIN", "IsinFldsHeader"
    a.Add "Pro rata group", "BondRataGroupHeader"
    a.Add "Price", "BondPriceHeader"
    a.Add "OC test", "OCLimitHeader"
    a.Add "IC test", "ICLimitHeader"
    a.Add "Trigger Type", "TriggerStart"
    a.Add "Exchange Rates", "ExcahngeRateTableHead"
    a.Add "Senior expenses rate", "SeniorExpensesField"
    a.Add "Senior management fees rate", "SeniorFeesField"
    a.Add "Junior management fees rate", "JuniorFeesField"
    a.Add "Senior expenses fixed", "SeniorExpensesFieldFixed"
    a.Add "Senior management fees fixed", "SeniorFeesFieldFixed"
    a.Add "Junior management fees fixed", "JuniorFeesFieldFixed"
    a.Add "Junior fees deferred coupon", "JuniorFeesCouponField"
    a.Add "CCC curve", "CCCcurveField"
    a.Add "CCC value", "CCCvalueField"
    a.Add "CCC limit", "CCClimitField"
    a.Add "Reinvestment period", "ReinvestPeriodField"
    a.Add "Reinvestment test limit", "ReinvestLimitField"
    a.Add "Upon Reinv test breach", "ReinvestTableHead"
    a.Add "Reinvestment Spread", "ReinvestSpreadField"
    a.Add "Reinvestment WAL", "ReinvestWALField"
    a.Add "Interest base", "InterestBaseField"
    a.Add "Principal Available", "AvailPrincField"
    a.Add "Interest Available", "AvailIntrField"
    a.Add "Currency", "CurrencyHead"
    a.Add "Settlement Date", "SettleDateField"
    a.Add "Coupon", "TrancheCouponHead"
    a.Add "Reference Rate", "RefRateHead"
    a.Add "IPD Frequency (In Months)", "IPDfrequencyField"
    a.Add "Next IPD", "FirstIPDfield"
    a.Add "Reinvest Bond Payment Frequ", "ReinvPaymFreqField"
    a.Add "Rating MDY|SP|FTCH", "RatingsHead"
    a.Add "Base Indexes", "BaseIndexesHead"
    a.Add "Last IPD", "PrevIPDField"
    a.Add "Call Date", "CallDateField"
    a.Add "Pool Value at Call", "CallValueField"
    a.Add "Use Call for Stress Test", "UseCallStressField"
    a.Add "Loss to Call", "LossToCallField"
    a.Add "Loss in BC", "LossOutputHeader"
    a.Add "Call Multiplier", "CallMultiplierField"
    a.Add "Reserve Fund Target", "CallReserveField"
    a.Add "Mortgages Results!A1", "MtgOutputCell"
    a.Add "Tranches Results!A1", "TranchesOutputCell"
    a.Add "Stress Test Results!C3", "FirstStressOutputCell"
    a.Add "Stress Test Results!Q3", "SecondStressOutputCell"
    a.Add "Stress Test Results!AE3", "ThirdStressOutputCell"
    a.Add "Loss in BC", "LossOutputCell"
    a.Add "Loss to Call", "LossCallOutputCell"
    a.Add "Implied CE", "CEcell"
    a.Add "Graphical Output", "PlotsSheet"
    a.Add 1, "MtgPlotIndex"
    a.Add 2, "OutstandingPlotIndex"
    a.Add 3, "OCPlotIndex"
    a.Add 4, "ICPlotIndex"
    a.Add 5, "CostFundingPlotIndex"
    a.Add 6, "AnnExcessPlotIndex"
    a.Add 8, "CPRLSPlotIndex"
    a.Add 9, "CallToEquityPlotIndex"
    a.Add 7, "EquityReturnPlotIndex"
    a.Add "Legal Final", "LegalFinalCell"
    a.Add "First Fund Target", "FirstResTargCell"
    a.Add "First Fund Multiple", "FirstResMultCell"
    a.Add "First Fund Floor", "FirstResFloorCell"
    a.Add "First Fund Cap", "FirstResCapCell"
    a.Add "First Fund Current Amount", "FirstResCurrCell"
    a.Add "First Fund Free After", "FirstResFreedCell"
    a.Add "First Fund In Waterfall", "FirstResDestinationCell"
    a.Add "Second Fund Target", "SecondResTargCell"
    a.Add "Second Fund Multiple", "SecondResMultCell"
    a.Add "Second Fund Floor", "SecondResFloorCell"
    a.Add "Second Fund Cap", "SecondResCapCell"
    a.Add "Second Fund Current Amount", "SecondResCurrCell"
    a.Add "Second Fund Free After", "SecondResFreedCell"
    a.Add "Second Fund In Waterfall", "SecondResDestinationCell"
    a.Add "Cumulative Reserve", "UseCumulativeReservesCell"
    a.Add "Deal Name", "DealNameCell"
    a.Add "Reinvestment Delay", "ReinvestmentDelayCell"
    a.Add "Reinvestment Price", "ReinvestmentPriceCell"
    a.Add "Use Fast Stress", "UseFastStressCell"
    a.Add "Base Index", "BaseIndexHead"
    a.Add "Reinvestment Repayment", "ReinvRepayCell"
    a.Add "Recovery Lag", "RecovLagCell"
    a.Add "Delinquency", "DelinqCell"
    a.Add "Delinquency Lag", "DelinqLagCell"
    a.Add "Current Deferred", "CurrentDeferredCell"
    a.Add "Use Forward Curves", "UseForwardCell"
    a.Add "Day Count", "DayCountHead"
    a.Add "Base Case To Call", "BaseCaseCall"
    a.Add "Current deferred junior fees", "StartingDefJunFees"
    a.Add "GIC Interest", "GICInterestCell"
    a.Add "GIC Base Rate", "GICBaseRateCell"
    a.Add "Forward Curves", "ForwardCurvesSheet"
    a.Add "Deal Day Count Convention", "DealDaycountCell"
    a.Add "Day count", "LoanDayCountHead"
    'Loans Properties
    a.Add "Issuer", "IssuerProperty"
    a.Add "Facility", "FacilityProperty"
    a.Add "Country", "CountryProperty"
    a.Add "Industry", "IndustryProperty"
    a.Add "Mezzanine", "MezzanineProperty"
    a.Add "Price", "PriceProperty"
    a.Add "Additional Properties", "StartingAdditionalProp"
    a.Add "Reinvestement Window", "ReinvestementWindowCell"
    a.Add "CPR Multiplier", "CPRMultiHead"
    a.Add "Loss Multiplier", "LSMultiHead"
    a.Add "Haircut", "HaircutVecHeader"
    a.Add "Loan Scenario", "LoanScenarioProperty"
    'New waterfall steps
    a.Add "Waterfall", "WaterfallSheet"
    a.Add "Interest Waterfall", "InterestWaterfallHead"
    a.Add "Principal Waterfall", "PrincipalWaterfallHead"
    a.Add "Step", "StepHead"
    a.Add "Seniority Group", "SeniorityGroupHead"
    a.Add "Seniority Group Level", "SeniorityGroupLevelHead"
    a.Add "Redemption Group", "RedemptionGroupHead"
    a.Add "Redemption Group Level", "RedemptionGroupLevelHead"
    a.Add "Redemption Share", "RedemptionShareHead"
    a.Add "Additional Collateral Share", "AdditionalCollateralShareHead"
    a.Add "Source of Funding", "SourceOfFundingHead"
    a.Add "Coupon Index", "CouponIndexHead"
    a.Add "Test Target Override", "TestTargetOverrideHead"
    a.Add "IRR to Equity Target", "IRRtoEquityTargetHead"
    a.Add "Reserve Index", "ReserveIndexHead"
    a.Add "Triggers", "TriggersHead"
    a.Add "Accrue/Pay", "AccruePayHead"
    a.Add "Coupon ", "AdditionalCouponBase"
    a.Add "Ref rate ", "AdditionalRefRateBase"
    a.Add "Day count ", "AdditionalDayCountBase"
End Sub

Private Function FromStringToInterestType(a As String) As Long
    If (UCase(a) = "FIXED") Then
        FromStringToInterestType = 0
    Else
        FromStringToInterestType = 1
    End If
End Function

Public Function FromStringToTriggerType(a As String) As Long
    On Error GoTo FromStringToTriggerType_Error
    Select Case UCase(a)
        Case "DATE TRIGGER"
            FromStringToTriggerType = 0
        Case "VECTOR TRIGGER"
            FromStringToTriggerType = 1
        Case "POOL SIZE TRIGGER"
            FromStringToTriggerType = 2
        Case "TRANCHE TRIGGER"
            FromStringToTriggerType = 3
        Case "DELINQUENCY TRIGGER"
            FromStringToTriggerType = 4
        Case "DURING STRESS TEST TRIGGER"
            FromStringToTriggerType = 5
        Case "CUMULATIVE LOSS TRIGGER"
            FromStringToTriggerType = 6
        Case UCase("Deferred Interest Trigger")
            FromStringToTriggerType = 7
        Case UCase("PDL Trigger")
            FromStringToTriggerType = 8
        Case ""
            Exit Function
        Case Else
            GoTo FromStringToTriggerType_Error
    End Select
    On Error GoTo 0
    Exit Function
FromStringToTriggerType_Error:
    Call MsgBox("Invalid trigger type." _
                & vbCrLf & "Aborting" _
                , vbCritical, "Error")
End Function
Public Function FromTriggerTypeToString(a As Long) As String
    On Error GoTo FromTriggerTypeToString_Error
    Select Case a
        Case 0
            FromTriggerTypeToString = "Date Trigger"
        Case 1
            FromTriggerTypeToString = "Vector Trigger"
        Case 2
            FromTriggerTypeToString = "Pool Size Trigger"
        Case 3
            FromTriggerTypeToString = "Tranche Trigger"
        Case 4
            FromTriggerTypeToString = "Delinquency Trigger"
        Case 5
            FromTriggerTypeToString = "During Stress Test Trigger"
        Case 6
            FromTriggerTypeToString = "Cumulative Loss Trigger"
        Case 7
            FromTriggerTypeToString = "Deferred Interest Trigger"
        Case 8
            FromTriggerTypeToString = "PDL Trigger"
        Case ""
            Exit Function
        Case Else
            GoTo FromTriggerTypeToString_Error
    End Select
    On Error GoTo 0
    Exit Function
FromTriggerTypeToString_Error:
    Call MsgBox("Invalid trigger type." _
                & vbCrLf & "Aborting" _
                , vbCritical, "Error")
End Function

Private Function FromStringToPriorty(a As String) As Long
    
   On Error GoTo FromStringToPriorty_Error
    Select Case UCase(a)
        Case UCase("Senior expenses")
            FromStringToPriorty = 0
        Case UCase("Senior management fees")
            FromStringToPriorty = 1
        Case UCase("Reinvestment")
            FromStringToPriorty = 13
        Case UCase("Interest")
            FromStringToPriorty = 2
        Case UCase("Principal")
            FromStringToPriorty = 3
        Case UCase("OC")
            FromStringToPriorty = 4
'        Case UCase("OC from Principal")
'            FromStringToPriorty = 5
        Case UCase("IC")
            FromStringToPriorty = 6
'        Case UCase("IC from Principal")
'            FromStringToPriorty = 7
        Case UCase("Deferred")
            FromStringToPriorty = 8
'        Case UCase("Deferred from Principal")
'            FromStringToPriorty = 9
        Case UCase("Excess")
            FromStringToPriorty = 12
        Case UCase("Junior management fees")
            FromStringToPriorty = 10
        Case UCase("Reinvestment test")
            FromStringToPriorty = 11
        Case UCase("Replenish Reserve")
            FromStringToPriorty = 14
'        Case UCase("Redeem Pro-Rata")
'            FromStringToPriorty = 15
        Case UCase("Turbo")
            FromStringToPriorty = 16
        Case UCase("Cure PDL")
            FromStringToPriorty = 17
        Case UCase("Fees From XS")
            FromStringToPriorty = 18
        Case UCase("Allocate Prepay fees")
            FromStringToPriorty = 19
        Case Else
            GoTo FromStringToPriorty_Error
    End Select
   On Error GoTo 0
   Exit Function

FromStringToPriorty_Error:
    Call MsgBox("Invalid step in the Waterfall." _
                & vbCrLf & "Aborting" _
                , vbCritical, "Error")
    End
End Function
Private Function FromPriortyToString(a As Long) As String
   On Error GoTo FromPriortyToString_Error
    Select Case UCase(a)
        Case 0
            FromPriortyToString = "Senior expenses"
        Case 1
            FromPriortyToString = "Senior management fees"
        Case 13
            FromPriortyToString = "Reinvestment"
        Case 2
            FromPriortyToString = "Interest"
        Case 3
            FromPriortyToString = "Principal"
        Case 4
            FromPriortyToString = "OC"
        Case 6
            FromPriortyToString = "IC"
        Case 8
            FromPriortyToString = "Deferred"
        Case 12
            FromPriortyToString = "Excess"
        Case 10
            FromPriortyToString = "Junior management fees"
        Case 11
            FromPriortyToString = "Reinvestment test"
        Case 14
            FromPriortyToString = "Replenish Reserve"
        Case 16
            FromPriortyToString = "Turbo"
        Case 17
            FromPriortyToString = "Cure PDL"
        Case 18
            FromPriortyToString = "Fees From XS"
        Case 19
            FromPriortyToString = "Allocate Prepay fees"
        Case Else
            GoTo FromPriortyToString_Error
    End Select
   On Error GoTo 0
   Exit Function

FromPriortyToString_Error:
    Call MsgBox("Invalid step in the Waterfall." _
                & vbCrLf & "Aborting" _
                , vbCritical, "Error")
    End
End Function
Public Function GetDM(TrancheName As String, DealName As String, Price As Double, Optional ToCall As Boolean = False)
    Dim result(0 To 4) As Variant
    result(0) = Mid(ActiveWorkbook.FullName, 1, InStrRev(ActiveWorkbook.FullName, "\"))
    result(2) = TrancheName
    result(1) = DealName
    result(3) = ToCall
    result(4) = Price
    GetDM = CLODiscountMargin(result)
End Function
Public Function GetIRR(TrancheName As String, DealName As String, Price As Double, Optional ToCall As Boolean = False)
    Dim result(0 To 4) As Variant
    result(0) = Mid(ActiveWorkbook.FullName, 1, InStrRev(ActiveWorkbook.FullName, "\"))
    result(2) = TrancheName
    result(1) = DealName
    result(3) = ToCall
    result(4) = Price
    GetIRR = CLOReturnRate(result)
End Function
Public Function GetWAL(TrancheName As String, DealName As String, CutOffDate As Date, Price As Double, Optional ToCall As Boolean = False)
    Dim result(0 To 4) As Variant
    result(0) = Mid(ActiveWorkbook.FullName, 1, InStrRev(ActiveWorkbook.FullName, "\"))
    result(2) = TrancheName
    result(1) = DealName
    result(3) = Format(CutOffDate, "yyyy-mm-dd")
    result(4) = ToCall
    GetWAL = CLOWALife(result)
End Function
Public Sub StressTargetEvent(NewTrancheName As String, StressSheet As String, StressTargetCell As String, Xvar As Long, Yvar As Long, NewPrice As Double, Optional PlotSheet As String = "", Optional PlotIndex As Long = 1)
    Dim Xspan As Range
    Dim Yspan As Range
    Dim i As Long, j As Long, k As Long
    Dim LossInput(0 To 7) As Variant
    Dim DMInput(0 To 8)
    DMInput(8) = NewPrice
    LossInput(0) = Mid(ActiveWorkbook.FullName, 1, InStrRev(ActiveWorkbook.FullName, "\"))
    LossInput(1) = NewTrancheName
    LossInput(5) = "0"
    LossInput(6) = "0"
    LossInput(7) = "0"
    Call ShowProgress("Updating Stress Values")
    Set Xspan = Range(Sheets(StressSheet).Range(StressTargetCell).Offset(0, -1), Sheets(StressSheet).Range(StressTargetCell).Offset(0, -1).End(xlDown))
    Set Yspan = Range(Sheets(StressSheet).Range(StressTargetCell).Offset(-1, 0), Sheets(StressSheet).Range(StressTargetCell).Offset(-1, 0).End(xlToRight))
    Dim LossValues() As Double
    Dim MarginValues() As Double
    ReDim LossValues(0 To Xspan.Count - 1, 0 To Yspan.Count - 1)
    ReDim MarginValues(0 To Xspan.Count - 1, 0 To Yspan.Count - 1)
    If (Xvar + Yvar = 1) Then
        LossInput(2) = CStr(Sheets(StressSheet).Range(StressTargetCell).Offset(-1, -1).Value)
    ElseIf (Xvar + Yvar = 2) Then
        LossInput(4) = CStr(Sheets(StressSheet).Range(StressTargetCell).Offset(-1, -1).Value)
    Else
        LossInput(3) = CStr(Sheets(StressSheet).Range(StressTargetCell).Offset(-1, -1).Value)
    End If
    For i = 1 To Xspan.Count
        If (Xvar = 0) Then
            LossInput(3) = CStr(Xspan.Cells(i, 1).Value)
        ElseIf (Xvar = 1) Then
            LossInput(4) = CStr(Xspan.Cells(i, 1).Value)
        Else
            LossInput(2) = CStr(Xspan.Cells(i, 1).Value)
        End If
        
        For j = 1 To Yspan.Count
            If (Yvar = 0) Then
                LossInput(3) = CStr(Yspan.Cells(1, j).Value)
            ElseIf (Yvar = 1) Then
                LossInput(4) = CStr(Yspan.Cells(1, j).Value)
            Else
                LossInput(2) = CStr(Yspan.Cells(1, j).Value)
            End If
            For k = 0 To 7
                DMInput(k) = LossInput(k)
            Next k
            LossValues(i - 1, j - 1) = GetStressLoss(LossInput)
            MarginValues(i - 1, j - 1) = GetStressDM(DMInput)
            Call SetProgress((((i - 1) * Yspan.Count) + j) / (Xspan.Count * Yspan.Count))
        Next j
    Next i
    On Error GoTo no_plot_found
    Sheets(StressSheet).Range(StressTargetCell).Resize(Xspan.Count, Yspan.Count) = LossValues
    On Error GoTo 0
    Dim SingleCell
    For Each SingleCell In Sheets(StressSheet).Range(StressTargetCell).Resize(Xspan.Count, Yspan.Count)
        If (SingleCell.Value = -1#) Then SingleCell.Value = CVErr(xlErrValue)
    Next SingleCell
    Dim TargetPlot As Chart
    Set TargetPlot = Sheets(PlotSheet).ChartObjects(PlotIndex).Chart
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
    Dim TempValue() As Double
    Dim TempXVal() As Double
    ReDim TempValue(1 To Xspan.Count)
    ReDim TempXVal(1 To Xspan.Count)
    For j = 1 To Xspan.Count
        TempXVal(j) = Xspan.Cells(j, 1).Value
    Next j
    For i = 1 To Yspan.Count
        For j = 1 To Xspan.Count
            TempValue(j) = MarginValues(j - 1, i - 1)
        Next j
        With TargetPlot.SeriesCollection.NewSeries
            .Name = Yspan.Cells(1, i)
            .Values = TempValue
            .XValues = TempXVal
        End With
    Next i
    With TargetPlot
        .ChartType = xlSurface
        .HasLegend = False
        .HasTitle = True
        .Axes(xlValue).HasTitle = True
        .Axes(xlCategory).HasTitle = True
        .Axes(xlTimeScale).HasTitle = True
        .ChartTitle.Text = "Discount Margin Sensitivity"
        .Axes(xlValue).AxisTitle.Caption = "Discount Margin"
        .Axes(xlCategory).ReversePlotOrder = False
        .Axes(xlTimeScale).ReversePlotOrder = True
    End With
    Call HideProgress
no_plot_found:
End Sub
Public Sub SeparateWaterfall( _
    InputsSheet As String, _
    FieldsLabels As Collection _
)
    Dim WaterfallStart As Range
    Set WaterfallStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("WaterfallHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Dim i As Long, index As Long
    i = 1
    Do While (True)
        If IsEmpty(WaterfallStart.Offset(i + 1, 0)) Then Exit Do
        i = i + 1
    Loop
    ReDim Params(1 To ((4 * (i - 1)) + 1)) As Variant
    Params(1) = i - 1
    i = 1
    index = 2
    Do While (True)
        If IsEmpty(WaterfallStart.Offset(i + 1, 0)) Then Exit Do
        Params(index) = FromStringToPriorty(WaterfallStart.Offset(i + 1, 0).Value)
        index = index + 1
        Params(index) = CLng(WaterfallStart.Offset(i + 1, 1).Value)
        index = index + 1
        Params(index) = CLng(WaterfallStart.Offset(i + 1, 2).Value)
        index = index + 1
        Params(index) = CDbl(WaterfallStart.Offset(i + 1, 3).Value)
        index = index + 1
        i = i + 1
    Loop
    'Call InspectWaterfall(Params)
End Sub
Public Sub EditWaterfallStep(InputsSheet As String, Target As Range, FieldsLabels As Collection, IntrWF As Boolean)
     Dim AllTheInputs As New Collection
     Dim TriggerStart As Range
     Dim i As Long
     'Call AddInput(AllTheInputs, Target.Parent.Name)
     'Call AddInput(AllTheInputs, Sheets(Target.Parent.Name).Cells(Target.Row, 1).Address)
     Dim CurrentStepStruct As String
     If (Target.Value = "") Then
        CurrentStepStruct = ""
     Else
        CurrentStepStruct = CStr(FromStringToPriorty(Target.Value))
        For i = 1 To 13
           CurrentStepStruct = CurrentStepStruct & "#" & Target.Offset(0, i).Value
        Next i
     End If
     Call AddInput(AllTheInputs, IntrWF)
     Call AddInput(AllTheInputs, CurrentStepStruct)
     Set TriggerStart = Sheets(InputsSheet).Cells.Find(What:=FieldsLabels("TriggerStart"), LookAt:=xlWhole, LookIn:=xlValues)
    i = 1
    Do While True
        If IsEmpty(TriggerStart.Offset(i, 0)) Then Exit Do
        i = i + 1
    Loop
    Call AddInput(AllTheInputs, CLng(i - 1))
    i = 1
    Do While True
        If IsEmpty(TriggerStart.Offset(i, 0)) Then Exit Do
        Select Case FromStringToTriggerType(TriggerStart.Offset(i, 0).Value)
            Case 0 'Date Trigger
                Call AddInput(AllTheInputs, CLng(0))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, Format(TriggerStart.Offset(i, 2).Value, "yyyy-mm-dd"))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 3).Value))
            Case 1 'Vector Trigger
                Call AddInput(AllTheInputs, CLng(1))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
            Case 2 'Pool Size Trigger
                Call AddInput(AllTheInputs, CLng(2))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 3).Value))
            Case 3 'Tranche Trigger
                Call AddInput(AllTheInputs, CLng(3))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 3).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 4).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 5).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 6).Value))
                Call AddInput(AllTheInputs, CDbl(TriggerStart.Offset(i, 7).Value))
            Case 4 'Delinquency Trigger
                Call AddInput(AllTheInputs, CLng(4))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
            Case 5 'During Reinvestment Test Trigger
                Call AddInput(AllTheInputs, CLng(5))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
            Case 6 'Pool Size Trigger
                Call AddInput(AllTheInputs, CLng(6))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 3).Value))
            Case 7 'Deferred Interest Trigger
                Call AddInput(AllTheInputs, CLng(7))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 3).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 4).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 5).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 6).Value))
                Call AddInput(AllTheInputs, CDbl(TriggerStart.Offset(i, 7).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 8).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 9).Value))
            Case 8 'PDL Trigger
                Call AddInput(AllTheInputs, CLng(3))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 1).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 2).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 3).Value))
                Call AddInput(AllTheInputs, CStr(TriggerStart.Offset(i, 4).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 5).Value))
                Call AddInput(AllTheInputs, CLng(TriggerStart.Offset(i, 6).Value))
                Call AddInput(AllTheInputs, CDbl(TriggerStart.Offset(i, 7).Value))
            Case Else
                Call MsgBox("Unhandled Trigger Type", vbCritical, "Error")
                Exit Sub
        End Select
        i = i + 1
    Loop
     Dim result() As Variant
    ReDim result(0 To AllTheInputs.Count - 1)
    For i = 1 To AllTheInputs.Count
        result(i - 1) = AllTheInputs(i)
    Next i
    Dim EditedStep As String
    EditedStep = WatFallStepEdit(result)
    If EditedStep = "" Then Exit Sub
    Dim StepParts
    StepParts = Split(EditedStep, "#")
    If StepParts(LBound(StepParts)) = "" Then
        Target.ClearContents
    Else
        Target.Value = FromPriortyToString(CLng(StepParts(LBound(StepParts))))
    End If
    For i = LBound(StepParts) + 1 To UBound(StepParts)
        With Target.Offset(0, i - LBound(StepParts))
            If StepParts(i) = "" Then
                .ClearContents
                .NumberFormat = ";;;"
                .Interior.Color = RGB(191, 191, 191)
                .ClearComments
            Else
                .Value = StepParts(i)
                .NumberFormat = "General"
                .Interior.Color = RGB(235, 241, 222)
            End If
            If StepParts(i) = "" Or (i = 7 + LBound(StepParts) And StepParts(LBound(StepParts)) <> "13") Then
                .NumberFormat = ";;;"
                .Interior.Color = RGB(191, 191, 191)
            End If
            If (i = 7 + LBound(StepParts) And StepParts(LBound(StepParts)) = "13") Then
                .NumberFormat = "[=1]""Unscheduled"";[=2]""Scheduled"";""All Principal"""
            End If
        End With
    Next i
    Target.Offset(1, 0).EntireRow.Hidden = False
End Sub

Public Sub EditTrigger(Target As Range)
    Dim i As Long
    Dim CurrentTrigStruct As String
    If (Target.Value = "") Then
        CurrentTrigStruct = ""
    Else
        CurrentTrigStruct = CStr(FromStringToTriggerType(Target.Value))
        For i = 1 To 9
           CurrentTrigStruct = CurrentTrigStruct & "#" & Target.Offset(0, i).Value
        Next i
    End If
    Dim result() As Variant
    ReDim result(0 To 0)
    result(0) = CurrentTrigStruct
    Dim EditedStep As String
    EditedStep = TriggerEdit(result)
    If EditedStep = "" Then Exit Sub
    Dim StepParts
    StepParts = Split(EditedStep, "#")
    If (StepParts(LBound(StepParts)) = "") Then
        Target.Value = ""
    Else
        Target.Value = FromTriggerTypeToString(CLng(StepParts(LBound(StepParts))))
    End If
    For i = LBound(StepParts) + 1 To LBound(StepParts) + 9
        With Target.Offset(0, i - LBound(StepParts))
            If i > UBound(StepParts) Then
                .ClearContents
            ElseIf StepParts(i) = "" Then
                .ClearContents
            Else
                .Value = StepParts(i)
                .NumberFormat = "General"
            End If
        End With
    Next i
End Sub
Public Sub EditLoanScenarios()
    Application.ScreenUpdating = False
    Application.Calculation = xlCalculationManual
    Dim FieldsLabels As New Collection
    Call PopulateDafaultLabels(FieldsLabels)
    Dim IssuerProperty As Range
    Dim FacilityProperty As Range
    Dim LoanScenarioProperty As Range
    Dim MaturityStart As Range
    Set IssuerProperty = Sheets("Loans Pool").Cells.Find(What:=FieldsLabels("IssuerProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FacilityProperty = Sheets("Loans Pool").Cells.Find(What:=FieldsLabels("FacilityProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LoanScenarioProperty = Sheets("Loans Pool").Cells.Find(What:=FieldsLabels("LoanScenarioProperty"), LookAt:=xlWhole, LookIn:=xlValues)
    Set MaturityStart = Sheets("Loans Pool").Cells.Find(What:=FieldsLabels("MaturityHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Dim i As Long
    i = 0
    Do While (True)
        If (IsEmpty(MaturityStart.Offset(i + 1, 0))) Then Exit Do
        If (MaturityStart.Offset(i + 1, 0).Value = "") Then Exit Do
        i = i + 1
    Loop
    Dim result() As Variant
    ReDim result(0 To (3 * i))
    result(0) = i
    Dim j As Long
    i = 0
    j = 1
    Do While (True)
        If (IsEmpty(MaturityStart.Offset(i + 1, 0))) Then Exit Do
        If (MaturityStart.Offset(i + 1, 0).Value = "") Then Exit Do
        result(j) = CStr(IssuerProperty.Offset(i + 1, 0).Value)
        j = j + 1
        result(j) = CStr(FacilityProperty.Offset(i + 1, 0).Value)
        j = j + 1
        result(j) = CStr(LoanScenarioProperty.Offset(i + 1, 0).Value)
        j = j + 1
        i = i + 1
    Loop
    Dim NewScenarios As String
    NewScenarios = LoadLoanScenario(result)
    If (NewScenarios = "") Then Exit Sub
    Dim Singlescenario
    Singlescenario = Split(NewScenarios, "#,#")
    For i = LBound(Singlescenario) To UBound(Singlescenario)
        LoanScenarioProperty.Offset(1 + i - LBound(Singlescenario), 0).Value = Singlescenario(i)
    Next i
    Application.Calculation = xlCalculationAutomatic
    Application.ScreenUpdating = True
End Sub
