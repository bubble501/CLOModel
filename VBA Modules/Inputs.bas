Attribute VB_Name = "Inputs"
Option Explicit
Declare Sub RunModel Lib "Z:\24AM\Analytics\CLO Model\ExcelDllCLOModel.dll" (ArrayData() As Variant)
'Declare Sub RunModel Lib "Z:\24AM\Analytics\CLO Model\ExcelDllCLOModelLoanTape.dll" (ArrayData() As Variant) 'This will also save the loan pool file
Declare Function CLODiscountMargin Lib "Z:\24AM\Analytics\CLO Model\ExcelDllCLOModel.dll" (ArrayData() As Variant) As Double
Declare Function CLOWALife Lib "Z:\24AM\Analytics\CLO Model\ExcelDllCLOModel.dll" (ArrayData() As Variant) As Double
Declare Sub StressTargetChanged Lib "Z:\24AM\Analytics\CLO Model\ExcelDllCLOModel.dll" (ArrayData() As Variant)
Declare Sub InspectStress Lib "Z:\24AM\Analytics\CLO Model\ExcelDllCLOModel.dll" (ArrayData() As Variant)
Declare Sub InspectWaterfall Lib "Z:\24AM\Analytics\CLO Model\ExcelDllCLOModel.dll" (ArrayData() As Variant)
Public Sub GetInputFromStructure( _
    MortgagesSheet As String, _
    InputsSheet As String, _
    CallerPath As String, _
    FieldsLabels As Collection, _
    Optional RunStressTest As Boolean = False, _
    Optional StressRowDimension As Long = 0, _
    Optional StressColDimension As Long = 1, _
    Optional StressTestSheet As String _
)
    Application.ScreenUpdating = False
    Dim i As Long, j As Long
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    'Fields to search
    Dim MaturityStart As Range
    Dim CouponStart As Range
    Dim OutstandingStart As Range
    Dim AnnuityStart As Range
    Dim HaircutVecStart As Range
    Dim CPRcell As Range
    Dim CDRcell As Range
    Dim LScell As Range
    Dim CutOffcell As Range
    Dim BondStart As Range
    Dim BondRataStart As Range
    Dim BondPriceStart As Range
    Dim OCLimitStart As Range
    Dim ICLimitStart As Range
    Dim WaterfallStart As Range
    Dim SeniorExpensesCell As Range
    Dim SeniorFeesCell As Range
    Dim JuniorFeesCell As Range
    Dim UseTurboCell As Range
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
    Dim FixFloatStart As Range
    Dim FrequencyStart As Range
    Dim CPRMultiStart As Range
    Dim LSMultiStart As Range
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
    Dim AccruedIntrStart As Range
    Dim IsinFldsStart As Range
    Dim FirstResTargCell As Range
    Dim FirstResMultCell As Range
    Dim FirstResFloorCell As Range
    Dim FirstResCurrCell As Range
    Dim SecondResTargCell As Range
    Dim SecondResMultCell As Range
    Dim SecondResFloorCell As Range
    Dim SecondResCurrCell As Range
    On Error Resume Next
    Set HaircutVecStart = Sheets(MortgagesSheet).Cells.Find(what:=FieldsLabels("HaircutVecHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set IsinFldsStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("IsinFldsHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResTargCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("FirstResTargCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResMultCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("FirstResMultCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResFloorCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("FirstResFloorCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstResCurrCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("FirstResCurrCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResTargCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("SecondResTargCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResMultCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("SecondResMultCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResFloorCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("SecondResFloorCell"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SecondResCurrCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("SecondResCurrCell"), LookAt:=xlWhole, LookIn:=xlValues)
    On Error GoTo 0
    Set MaturityStart = Sheets(MortgagesSheet).Cells.Find(what:=FieldsLabels("MaturityHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CouponStart = Sheets(MortgagesSheet).Cells.Find(what:=FieldsLabels("CouponHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set OutstandingStart = Sheets(MortgagesSheet).Cells.Find(what:=FieldsLabels("OutstandingHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set AnnuityStart = Sheets(MortgagesSheet).Cells.Find(what:=FieldsLabels("AnnuityHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FrequencyStart = Sheets(MortgagesSheet).Cells.Find(what:=FieldsLabels("FrequencyHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CPRMultiStart = Sheets(MortgagesSheet).Cells.Find(what:=FieldsLabels("CPRMultiHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LSMultiStart = Sheets(MortgagesSheet).Cells.Find(what:=FieldsLabels("LSMultiHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CPRcell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CPRfield"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CDRcell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CDRfield"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LScell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("LSfield"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CurrentOutStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CurrentOutHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set OriginalOutStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("OriginalOutHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CutOffcell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CutOffField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set BondStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("BondsNamesHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set BondRataStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("BondRataGroupHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set BondPriceStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("BondPriceHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set OCLimitStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("OCLimitHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ICLimitStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("ICLimitHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set IPDfrequencyCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("IPDfrequencyField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FirstIPDcell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("FirstIPDfield"), LookAt:=xlWhole, LookIn:=xlValues)
    Set PrevIPDCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("PrevIPDField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CurrencyStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CurrencyHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LossOutputCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("LossOutputHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set LossToCallCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("LossToCallField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SettleDateCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("SettleDateField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set AccruedIntrStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("IntrAccrHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set TrancheCouponStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("TrancheCouponHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set RefRateStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("RefRateHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set WaterfallStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("WaterfallHeader"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SeniorExpensesCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("SeniorExpensesField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set SeniorFeesCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("SeniorFeesField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set JuniorFeesCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("JuniorFeesField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set UseTurboCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("UseTurboField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CCCcurveCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CCCcurveField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CCCvalueCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CCCvalueField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CCClimitCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CCClimitField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CallDateCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CallDateField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CallValueCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CallValueField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CallMultiplierCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CallMultiplierField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set CallReserveCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CallReserveField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set UseCallStressCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("UseCallStressField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestPeriodCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("ReinvestPeriodField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestLimitCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("ReinvestLimitField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestTableCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("ReinvestTableHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestSpreadCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("ReinvestSpreadField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvPaymFreqCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("ReinvPaymFreqField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ReinvestWALCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("ReinvestWALField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set InterestBaseCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("InterestBaseField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set AvailPrincCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("AvailPrincField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set AvailIntrCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("AvailIntrField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ExcahngeRateTableStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("ExcahngeRateTableHead"), LookAt:=xlWhole, LookIn:=xlValues)
    Set ExcahngeRateTableStart = Range(ExcahngeRateTableStart, ExcahngeRateTableStart.End(xlDown).Offset(0, 1))
    Set JuniorFeesCouponCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("JuniorFeesCouponField"), LookAt:=xlWhole, LookIn:=xlValues)
    Set FixFloatStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("FixFloatHead"), LookAt:=xlWhole, LookIn:=xlValues)
    If (RunStressTest) Then
        Set StressCPRCell = Sheets(StressTestSheet).Cells.Find(what:=FieldsLabels("StressCPRField"), LookAt:=xlWhole, LookIn:=xlFormulas)
        Set StressCDRStart = Sheets(StressTestSheet).Cells.Find(what:=FieldsLabels("StressCDRHead"), LookAt:=xlWhole, LookIn:=xlFormulas)
        Set StressLSStart = Sheets(StressTestSheet).Cells.Find(what:=FieldsLabels("StressLSHead"), LookAt:=xlWhole, LookIn:=xlFormulas)
    End If
    On Error Resume Next
        Set BaseIndexesStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("BaseIndexesHead"), LookAt:=xlWhole, LookIn:=xlValues)
    On Error GoTo 0
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

    Dim tempReferenceRate As Double
    On Error GoTo ReferenceRateFromBBg
        tempReferenceRate = Application.WorksheetFunction.VLookup(InterestBaseCell.Offset(0, 1).Value, Range(BaseIndexesStart, BaseIndexesStart.End(xlDown).Offset(0, 1)), 2, False)
        If False Then
ReferenceRateFromBBg:
            tempReferenceRate = bdp24(UCase(InterestBaseCell.Offset(0, 1).Value) + " Index", "PX_LAST") / 100
            Resume Next
        End If
    On Error GoTo 0
    
    Dim AllTheInputs As New Collection
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
            Call AddInput(AllTheInputs, CStr(AnnuityStart.Offset(i, 0).Value))
            Call AddInput(AllTheInputs, CLng(FrequencyStart.Offset(i, 0).Value))
            Call AddInput(AllTheInputs, CStr(CPRMultiStart.Offset(i, 0).Value))
            Call AddInput(AllTheInputs, CStr(LSMultiStart.Offset(i, 0).Value))
            If (HaircutVecStart Is Nothing) Then
                Call AddInput(AllTheInputs, "0")
            Else
                Call AddInput(AllTheInputs, CStr(HaircutVecStart.Offset(i, 0).Value))
            End If
        End If
        i = i + 1
    Loop
    If (IsEmpty(MaturityStart.Offset(1, 0))) Then
        Call AddInput(AllTheInputs, CLng(0))
    Else
        Call AddInput(AllTheInputs, CLng(BondStart.End(xlDown).Row - BondStart.Row))
    End If
    i = 1
    Do While (True)
        If IsEmpty(BondStart.Offset(i, 0)) Then Exit Do
        Call AddInput(AllTheInputs, CStr(BondStart.Offset(i, 0).Value))
        If (IsinFldsStart Is Nothing) Then
            Call AddInput(AllTheInputs, "")
        Else
            Call AddInput(AllTheInputs, CStr(IsinFldsStart.Offset(i, 0).Value))
        End If
        Call AddInput(AllTheInputs, CLng(BondRataStart.Offset(i, 0).Value))
        Call AddInput(AllTheInputs, OriginalOutStart.Offset(i, 0).Value)
        Call AddInput(AllTheInputs, CStr(CurrencyStart.Offset(i, 0).Value))
        Call AddInput(AllTheInputs, CurrentOutStart.Offset(i, 0).Value)
        Call AddInput(AllTheInputs, FromStringToInterestType(FixFloatStart.Offset(i, 0).Value))
        'Call AddInput(AllTheInputs, TrancheCouponStart.Offset(i, 0).Value / 10000)
        Call AddInput(AllTheInputs, CStr(TrancheCouponStart.Offset(i, 0).Value))
        Call AddInput(AllTheInputs, CStr(RefRateStart.Offset(i, 0).Value))
        Call AddInput(AllTheInputs, Format(PrevIPDCell.Offset(0, 1).Value, "yyyy-mm-dd"))
        Call AddInput(AllTheInputs, CStr(InterestBaseCell.Offset(0, 1).Value))
        Call AddInput(AllTheInputs, CLng(IPDfrequencyCell.Offset(0, 1)))
        On Error GoTo SpreadFromBBg
            If (IsEmpty(RefRateStart.Offset(i, 0)) Or RefRateStart.Offset(i, 0) = "") Then
                Call AddInput(AllTheInputs, Application.WorksheetFunction.VLookup(InterestBaseCell.Offset(0, 1).Value, Range(BaseIndexesStart, BaseIndexesStart.End(xlDown).Offset(0, 1)), 2, False))
            Else
                Call AddInput(AllTheInputs, Application.WorksheetFunction.VLookup(RefRateStart.Offset(i, 0).Value, Range(BaseIndexesStart, BaseIndexesStart.End(xlDown).Offset(0, 1)), 2, False))
            End If
        If False Then
SpreadFromBBg:
            Call AddInput(AllTheInputs, -1#)
            Resume Next
        End If
        On Error GoTo 0
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
        Call AddInput(AllTheInputs, AccruedIntrStart.Offset(i, 0).Value)
        i = i + 1
    Loop
    If (IsEmpty(WaterfallStart.Offset(2, 0))) Then
        Call AddInput(AllTheInputs, CLng(0))
    Else
        Call AddInput(AllTheInputs, CLng(WaterfallStart.Offset(2, 0).End(xlDown).Row - WaterfallStart.Row - 1))
    End If
    i = 1
    Do While (True)
        If IsEmpty(WaterfallStart.Offset(i + 1, 0)) Then Exit Do
        Call AddInput(AllTheInputs, FromStringToPriorty(WaterfallStart.Offset(i + 1, 0).Value))
        Call AddInput(AllTheInputs, CLng(WaterfallStart.Offset(i + 1, 1).Value))
        Call AddInput(AllTheInputs, CLng(WaterfallStart.Offset(i + 1, 2).Value))
        Call AddInput(AllTheInputs, WaterfallStart.Offset(i + 1, 3).Value)
        i = i + 1
    Loop
    Call AddInput(AllTheInputs, SeniorExpensesCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, SeniorFeesCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, JuniorFeesCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, JuniorFeesCouponCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CLng(IPDfrequencyCell.Offset(0, 1)))
    Call AddInput(AllTheInputs, Format(FirstIPDcell.Offset(0, 1), "yyyy-mm-dd"))
    Call AddInput(AllTheInputs, Format(PrevIPDCell.Offset(0, 1), "yyyy-mm-dd"))
    Call AddInput(AllTheInputs, UseTurboCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CStr(CCCcurveCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CCCvalueCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CCClimitCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, Format(CallDateCell.Offset(0, 1).Value, "yyyy-mm-dd"))
    Call AddInput(AllTheInputs, CallMultiplierCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CallReserveCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, CallValueCell.Offset(0, 1).Value)
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    'Reserve fund
    If (FirstResTargCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(FirstResTargCell.Offset(0, 1).Value))
    End If
    If (FirstResMultCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(FirstResMultCell.Offset(0, 1).Value))
    End If
    If (FirstResFloorCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(FirstResFloorCell.Offset(0, 1).Value))
    End If
     If (FirstResCurrCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(FirstResCurrCell.Offset(0, 1).Value))
    End If
    If (SecondResTargCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(SecondResTargCell.Offset(0, 1).Value))
    End If
    If (SecondResMultCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(SecondResMultCell.Offset(0, 1).Value))
    End If
    If (SecondResFloorCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(SecondResFloorCell.Offset(0, 1).Value))
    End If
     If (SecondResCurrCell Is Nothing) Then
        Call AddInput(AllTheInputs, 0#)
    Else
        Call AddInput(AllTheInputs, CDbl(SecondResCurrCell.Offset(0, 1).Value))
    End If
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Call AddInput(AllTheInputs, Format(ReinvestPeriodCell.Offset(0, 1).Value, "yyyy-mm-dd"))
    Call AddInput(AllTheInputs, ReinvestLimitCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, ReinvestTableCell.Offset(1, 1).Value)
    Call AddInput(AllTheInputs, ReinvestTableCell.Offset(1, 2).Value)
    Call AddInput(AllTheInputs, ReinvestTableCell.Offset(2, 1).Value)
    Call AddInput(AllTheInputs, ReinvestTableCell.Offset(2, 2).Value)
    Call AddInput(AllTheInputs, CStr(ReinvestSpreadCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(CPRcell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(CDRcell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(LScell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CStr(ReinvestWALCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, CLng(ReinvPaymFreqCell.Offset(0, 1).Value))
    Call AddInput(AllTheInputs, tempReferenceRate)
    Call AddInput(AllTheInputs, AvailPrincCell.Offset(0, 1).Value)
    Call AddInput(AllTheInputs, AvailIntrCell.Offset(0, 1).Value)
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
    Call AddInput(AllTheInputs, InputsSheet + "!" + Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("LossOutputCell"), LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0).Address)
    Call AddInput(AllTheInputs, InputsSheet + "!" + Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("LossCallOutputCell"), LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0).Address)
    Call AddInput(AllTheInputs, InputsSheet + "!" + Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CEcell"), LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0).Address)
    Call AddInput(AllTheInputs, CallerPath)
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
    a.Add "CPR Multiplier", "CPRMultiHead"
    a.Add "Loss Multiplier", "LSMultiHead"
    a.Add "Haircut", "HaircutVecHeader"
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
    a.Add "Fixed/Floating", "FixFloatHead"
    a.Add "OC test", "OCLimitHeader"
    a.Add "IC test", "ICLimitHeader"
    a.Add "Waterfall", "WaterfallHeader"
    a.Add "Exchange Rates", "ExcahngeRateTableHead"
    a.Add "Senior expenses rate", "SeniorExpensesField"
    a.Add "Senior management fees rate", "SeniorFeesField"
    a.Add "Junior management fees rate", "JuniorFeesField"
    a.Add "Junior fees deferred coupon", "JuniorFeesCouponField"
    a.Add "Use Turbo feature", "UseTurboField"
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
    a.Add "Accrued Interest", "IntrAccrHead"
    a.Add "First Fund Target", "FirstResTargCell"
    a.Add "First Fund Multiple", "FirstResMultCell"
    a.Add "First Fund Floor", "FirstResFloorCell"
    a.Add "First Fund Current Amount", "FirstResCurrCell"
    a.Add "Second Fund Target", "SecondResTargCell"
    a.Add "Second Fund Multiple", "SecondResMultCell"
    a.Add "Second Fund Floor", "SecondResFloorCell"
    a.Add "Second Fund Current Amount", "SecondResCurrCell"
End Sub

Private Function FromStringToInterestType(a As String) As Long
    If (UCase(a) = "FIXED") Then
        FromStringToInterestType = 0
    Else
        FromStringToInterestType = 1
    End If
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
        Case UCase("OC from Principal")
            FromStringToPriorty = 5
        Case UCase("IC")
            FromStringToPriorty = 6
        Case UCase("IC from Principal")
            FromStringToPriorty = 7
        Case UCase("Deferred")
            FromStringToPriorty = 8
        Case UCase("Deferred from Principal")
            FromStringToPriorty = 9
        Case UCase("Excess")
            FromStringToPriorty = 12
        Case UCase("Junior management fees")
            FromStringToPriorty = 10
        Case UCase("Reinvestment test")
            FromStringToPriorty = 11
        Case UCase("Replenish Reserve")
            FromStringToPriorty = 14
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

Public Function GetDM(TrancheName As String, Price As Double, Optional ToCall As Boolean = False)
    Dim result(0 To 3) As Variant
    result(0) = Left(ActiveWorkbook.FullName, InStrRev(ActiveWorkbook.FullName, "\"))
    result(1) = TrancheName
    result(2) = ToCall
    result(3) = Price
    GetDM = CLODiscountMargin(result)
End Function
Public Function GetWAL(TrancheName As String, CutOffDate As Date, Price As Double, Optional ToCall As Boolean = False)
    Dim result(0 To 4) As Variant
    result(0) = Left(ActiveWorkbook.FullName, InStrRev(ActiveWorkbook.FullName, "\"))
    result(1) = TrancheName
    result(2) = Format(CutOffDate, "yyyy-mm-dd")
    result(3) = ToCall
    GetWAL = CLOWALife(result)
End Function
Public Sub StressTargetEvent(NewTrancheName As String, StressTargetCell As String, Xvar As Long, Yvar As Long, NewPrice As Double, Optional PlotSheet As String = "", Optional PlotIndex As Long = 1)
    Dim result(0 To 7) As Variant
    result(0) = Left(ActiveWorkbook.FullName, InStrRev(ActiveWorkbook.FullName, "\"))
    result(1) = NewTrancheName
    result(2) = StressTargetCell
    result(3) = Xvar
    result(4) = Yvar
    result(5) = NewPrice
    result(6) = PlotSheet
    result(7) = PlotIndex
    Call StressTargetChanged(result)
End Sub
Public Sub SeparateWaterfall( _
    InputsSheet As String, _
    FieldsLabels As Collection _
)
    Dim WaterfallStart As Range
    Set WaterfallStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("WaterfallHeader"), LookAt:=xlWhole, LookIn:=xlValues)
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
    Call InspectWaterfall(Params)
End Sub

