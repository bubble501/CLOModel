Attribute VB_Name = "Internals"
Option Explicit
Public Sub PlotBloobergVector(BbgVct As String, TargetPlot As Chart, Optional StartDate As Date = 36526, Optional Percent As Boolean = True)
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
    With TargetPlot
        .ChartType = xlLine
        .HasLegend = False
        .HasTitle = False
        .Axes(xlValue).HasTitle = False
        .Axes(xlCategory).HasTitle = False
        .Axes(xlCategory).TickLabels.NumberFormat = "mmm-yy"
        .Axes(xlValue).DisplayUnit = xlNone
        If (Percent) Then
            .Axes(xlValue).TickLabels.NumberFormat = "0.0%"
        Else
            .Axes(xlValue).TickLabels.NumberFormat = "0.0"
        End If
    End With
    If Not ValidBloombergVector(BbgVct) Then
        Exit Sub
    End If
    Dim SingleValues
    SingleValues = UnpackVect(BbgVct, 1, False)
    Dim CurrentDate As Date
    CurrentDate = StartDate
    Dim PlotValues() As Double
    Dim PlotXValues() As Date
    ReDim PlotValues(1 To Application.WorksheetFunction.Max(10, UBound(SingleValues) + 3 - LBound(SingleValues)))
    ReDim PlotXValues(1 To UBound(PlotValues))
    Dim i As Long
    For i = 1 To UBound(PlotValues)
        PlotValues(i) = (1 + ((CLng(Percent) + 1) * (CLng(Percent) + 99))) * SingleValues(Application.WorksheetFunction.Min(UBound(SingleValues), i))
        PlotXValues(i) = CLng(CurrentDate)
        CurrentDate = NextMonth(CurrentDate)
    Next i
    With TargetPlot.SeriesCollection.NewSeries
        .Name = "Vector"
        .Values = PlotValues
        .XValues = PlotXValues
    End With
End Sub
Public Sub WaterfallStepChanged(TargetAllCell As Range)
    Dim TempRange As Range
    Set TempRange = TargetAllCell.Parent.Cells.Find(what:="Pro rata group", LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0)
    Dim TargetCell
    Application.ScreenUpdating = False
    For Each TargetCell In TargetAllCell
        TargetCell.Offset(0, 1).Resize(1, 3).ClearComments
        TargetCell.Offset(0, 1).Resize(1, 3).Validation.Delete
        TargetCell.Offset(0, 1).Resize(1, 2).Validation.Add Type:=xlValidateWholeNumber, AlertStyle:=xlValidAlertStop, Operator:= _
                    xlBetween, Formula1:="1", Formula2:="=max(" + Range(TempRange, TempRange.End(xlDown)).Address + ")"
        TargetCell.Offset(0, 1).NumberFormat = "0"
        TargetCell.Offset(0, 2).Resize(1, 2).ClearContents
        TargetCell.Offset(0, 2).Resize(1, 2).Locked = True
        TargetCell.Offset(0, 1).Locked = False
        Select Case UCase(TargetCell.Value)
            Case UCase("Senior expenses"), UCase("Senior management fees"), UCase("Junior management fees")
                TargetCell.Offset(0, 1).ClearContents
                TargetCell.Offset(0, 1).Locked = True
                TargetCell.Offset(0, 2).Locked = False
                TargetCell.Offset(0, 2).NumberFormat = "[=1]""From Interest"";[=2]""From Principal"""
                TargetCell.Offset(0, 2).Validation.Delete
                TargetCell.Offset(0, 2).Validation.Add Type:=xlValidateWholeNumber, AlertStyle:=xlValidAlertStop, Operator:= _
                    xlBetween, Formula1:="1", Formula2:="2"
                TargetCell.Offset(0, 2).Value = 1
                TargetCell.Offset(0, 2).AddComment "1 if they are payed using interest, 2 if they are payed using principal"
            Case UCase("Replenish Reserve")
                TargetCell.Offset(0, 1).Locked = False
                TargetCell.Offset(0, 2).Locked = False
                TargetCell.Offset(0, 1).NumberFormat = 0
                TargetCell.Offset(0, 2).NumberFormat = "[=1]""From Interest"";[=2]""From Principal"""
                TargetCell.Offset(0, 1).Validation.Delete
                TargetCell.Offset(0, 2).Validation.Delete
                TargetCell.Offset(0, 1).Validation.Add Type:=xlValidateWholeNumber, AlertStyle:=xlValidAlertStop, Operator:= _
                    xlBetween, Formula1:="1", Formula2:="2"
                TargetCell.Offset(0, 2).Validation.Add Type:=xlValidateWholeNumber, AlertStyle:=xlValidAlertStop, Operator:= _
                    xlBetween, Formula1:="1", Formula2:="2"
                TargetCell.Offset(0, 1).Value = 1
                TargetCell.Offset(0, 2).Value = 1
                TargetCell.Offset(0, 1).AddComment "Index of the reserve fund to use"
                TargetCell.Offset(0, 2).AddComment "1 if it's replenished using interest, 2 if it's replenished using principal"
            Case UCase("Interest"), UCase("Deferred"), UCase("Deferred from Principal"), UCase("IC"), UCase("IC from Principal")
                TargetCell.Offset(0, 2).NumberFormat = ";;;"
                TargetCell.Offset(0, 2).Value = 1
            Case UCase("Reinvestment")
                TargetCell.Offset(0, 1).ClearContents
                TargetCell.Offset(0, 1).Locked = True
                TargetCell.Offset(0, 2).Locked = False
                TargetCell.Offset(0, 2).Validation.Delete
                TargetCell.Offset(0, 2).Validation.Add Type:=xlValidateWholeNumber, AlertStyle:=xlValidAlertStop, Operator:= _
                    xlBetween, Formula1:="0", Formula2:="2"
                TargetCell.Offset(0, 2).NumberFormat = "[=1]""Unscheduled"";[=2]""Scheduled"";""All Principal"""
                TargetCell.Offset(0, 1).ClearContents
                TargetCell.Offset(0, 2).Value = 0
            Case UCase("Excess")
                TargetCell.Offset(0, 1).ClearContents
                TargetCell.Offset(0, 1).Locked = True
                TargetCell.Offset(0, 2).Locked = False
                TargetCell.Offset(0, 2).AddComment ("Tranche that will recieve excess spread payments." + vbCrLf + "Leave blank if no bond should recieve it")
                TargetCell.Offset(0, 2).NumberFormat = "0"
                TargetCell.Offset(0, 2).Validation.Delete
                TargetCell.Offset(0, 2).Validation.Add Type:=xlValidateWholeNumber, AlertStyle:=xlValidAlertStop, Operator:= _
                    xlBetween, Formula1:="0", Formula2:="=max(" + Range(TempRange, TempRange.End(xlDown)).Address + ")"
            Case UCase("Reinvestment test")
                TargetCell.Offset(0, 1).AddComment ("Most junior tranche that will be considered for reinvestment test")
                TargetCell.Offset(0, 2).Locked = False
                TargetCell.Offset(0, 2).NumberFormat = "0"
                TargetCell.Offset(0, 2).AddComment ("Tranche that will be redeemed if the test fails and turbo is on" + vbCrLf + "Leave Blank for Sequential")
            Case UCase("OC"), UCase("OC from Principal")
                TargetCell.Offset(0, 2).Resize(1, 2).Locked = False
                TargetCell.Offset(0, 1).AddComment ("Most junior tranche that will be considered for the test level")
                TargetCell.Offset(0, 2).AddComment ("Tranche that will be redeemed if the test fails. Leave blank for sequential")
                TargetCell.Offset(0, 3).AddComment ("Share that will redeem that tranche")
                TargetCell.Offset(0, 3).Value = 0
                TargetCell.Offset(0, 3).NumberFormat = "0%"
                TargetCell.Offset(0, 2).NumberFormat = "0"
            Case UCase("Cure PDL")
                TargetCell.Offset(0, 2).Resize(1, 2).Locked = False
                TargetCell.Offset(0, 1).AddComment ("Tranche whose PDL will be cured")
                TargetCell.Offset(0, 2).AddComment ("Tranche that will recieve PDL cure. Leave blank for sequential")
                TargetCell.Offset(0, 3).AddComment ("Share that will redeem that tranche")
                TargetCell.Offset(0, 3).Value = 0
                TargetCell.Offset(0, 3).NumberFormat = "0%"
                TargetCell.Offset(0, 2).NumberFormat = "0"
            Case UCase("Turbo")
                TargetCell.Offset(0, 2).Resize(1, 2).Locked = False
                TargetCell.Offset(0, 1).Locked = True
                TargetCell.Offset(0, 1).Value = ""
                TargetCell.Offset(0, 2).AddComment ("Class of notes that will be redeemed")
                TargetCell.Offset(0, 3).AddComment ("Share of interest that will redeem that tranche")
                TargetCell.Offset(0, 3).Value = 1#
                TargetCell.Offset(0, 3).NumberFormat = "0%"
                TargetCell.Offset(0, 2).NumberFormat = "0"
            Case UCase("Redeem Pro-Rata")
                TargetCell.Offset(0, 2).Locked = False
                TargetCell.Offset(0, 1).AddComment ("First Seniority Level To Redeem")
                TargetCell.Offset(0, 2).AddComment ("Last Seniority Level To Redeem")
                TargetCell.Offset(0, 1).Resize(1, 2).NumberFormat = "0"
            Case UCase("Fees from XS")
                TargetCell.Offset(0, 1).Value = ""
                TargetCell.Offset(0, 2).Value = 3
                TargetCell.Offset(0, 3).Value = 0
                TargetCell.Offset(0, 2).Locked = False
                TargetCell.Offset(0, 3).Locked = False
                TargetCell.Offset(0, 1).Locked = True
                TargetCell.Offset(0, 2).AddComment ("1 Excess Interest, 2 Excess Principal, 3 All excess")
                TargetCell.Offset(0, 3).AddComment ("Share of Excess Toward Fees")
                TargetCell.Offset(0, 2).NumberFormat = "[=1]""XS Interst"";[=2]""XS Principal"";""All XS"""
                TargetCell.Offset(0, 3).NumberFormat = "0%"
            Case ""
                TargetCell.Offset(0, 1).Resize(1, 3).ClearContents
                TargetCell.Offset(0, 1).Resize(1, 3).Locked = True
        End Select
    Next TargetCell
    Application.ScreenUpdating = True
End Sub
Public Function ValidBloombergVector(BloombergVector As String) As Boolean
    Dim BloombergVecRegExp As New RegExp
    BloombergVecRegExp.Pattern = "^(A\s+\d{1,2}/\d{1,2}/\d{2,4}\s+)?\d*\.?\d+(\s+\d+[RS]\s+\d*\.?\d+)*$"
    ValidBloombergVector = BloombergVecRegExp.Test(UCase(Trim(BloombergVector)))
End Function
Private Function UnpackVect(ByVal Vect As String, Optional PaymFreq As Long = 1, Optional AdjustFreq As Boolean = True)
    Dim result As New Collection
    Dim ResultVect As Variant
    If Not ValidBloombergVector(Vect) Then Exit Function
    Vect = UCase(Trim(Vect))
    Dim AnchorRegExp As New RegExp
    AnchorRegExp.Pattern = "A\s+\d{1,2}/\d{1,2}/\d{2,4}\s+"
    Vect = AnchorRegExp.Replace(Vect, "")
    
    Dim StringParts
    StringParts = Split(Vect, " ")
    Dim i As Long, j As Long, StepLen As Long
    i = 2
    While (i <= UBound(StringParts))
        StepLen = CLng((Left(StringParts(i - 1), Len(StringParts(i - 1)) - 1)) / PaymFreq)
        If (Right(StringParts(i - 1), 1) = "S") Then
            For j = 1 To StepLen
                result.Add CDbl(StringParts(i - 2))
            Next j
        ElseIf (Right(StringParts(i - 1), 1) = "R") Then
            For j = 0 To StepLen - 1
                result.Add CDbl(StringParts(i - 2)) + (CDbl(StringParts(i)) - CDbl(StringParts(i - 2))) * j / StepLen
            Next j
        End If
        i = i + 2
    Wend
    result.Add CDbl(StringParts(i - 2))
    ReDim ResultVect(1 To result.Count)
    i = 1
    Dim tmpVal
    For Each tmpVal In result
        If (AdjustFreq) Then
            ResultVect(i) = ((1 + (tmpVal / 100)) ^ (PaymFreq / 12)) - 1
        Else
            ResultVect(i) = tmpVal / 100
        End If
        i = i + 1
    Next tmpVal
    UnpackVect = ResultVect
End Function
Private Function NextMonth(Source As Date) As Date
    If (Application.WorksheetFunction.EoMonth(Source, 0) = Source) Then
        NextMonth = Application.WorksheetFunction.EoMonth(Source, 1)
    Else
        NextMonth = DateValue( _
            Format(Source, "dd-") + _
            Format(Application.WorksheetFunction.EoMonth(Source, 1), "mmmm-yyyy") _
        )
    End If
End Function
Public Sub StandardLoanpoolPartChanged(Target As Range)
    Dim SingleCell
    Dim HeaderString As String
    For Each SingleCell In Target
        If (IsEmpty(SingleCell)) Then
            HeaderString = UCase(Target.Parent.Cells(1, SingleCell.Column).Value)
            If HeaderString = UCase("CPR Multiplier") Or HeaderString = UCase("Price") Or HeaderString = UCase("Loss Multiplier") Then
                SingleCell.Value = 100
            ElseIf HeaderString = UCase("Mezzanine") Or HeaderString = "CCC" Or HeaderString = UCase("Default") Then
                SingleCell.Value = "No"
            End If
        End If
    Next SingleCell
End Sub
Public Function CountEmpty(Arr As Variant) As Long
    Dim i As Long, result As Long
    result = 0
    For i = LBound(Arr) To UBound(Arr)
        If (IsEmpty(Arr(i))) Then result = result + 1
    Next i
    CountEmpty = result
End Function
'Public Sub StructureFromBloomberg(FieldsLabels As Collection, InputsSheet As String, Optional BloombergExtension As String = "Mtge")
'    Dim Identifier As String
'    Identifier = Application.InputBox(Prompt:="Enter the Ticker or ISIN of one of the Bonds in the Deal", _
'          Title:="Enter an Identifier", Type:=2)
'    Application.ScreenUpdating = False
'    Dim i As Long
'    Dim DealName As String
'    DealName = bdp24(UCase(Trim(Identifier)) + " " + BloombergExtension, "MTG_DEAL_NAME")
'    If (Left(DealName, 4) = "#N/A") Then
'        Call MsgBox("Impossible to get required data from Bloomberg" _
'                    & vbCrLf & "Make sure you specified the right security Identifier" _
'                    , vbCritical, "Error")
'        Exit Sub
'    End If
'    Dim TrancheExtensions() As String
'    'Dim InterestTypeString As String
'    Dim BbgResponse
'    BbgResponse = bds24(UCase(Trim(Identifier)) + " " + BloombergExtension, "MTGE_CMO_GROUP_LIST")
'    ReDim TrancheExtensions(LBound(BbgResponse) To UBound(BbgResponse))
'    For i = LBound(TrancheExtensions) To UBound(TrancheExtensions)
'        TrancheExtensions(i) = DealName + " " + CStr(BbgResponse(i, 1)) + " " + BloombergExtension
'    Next i
'    Dim BBgInformations
'    Dim RequestedFields(1 To 4) As String
'    RequestedFields(1) = "CRNCY"
'    RequestedFields(2) = "MTG_ORIG_AMT"
'    RequestedFields(3) = "MTG_TYP"
'    RequestedFields(4) = "ID_ISIN"
'    BBgInformations = bdp24(TrancheExtensions, RequestedFields)
'
'    Dim BondStart As Range
'    Dim TickerAddress As String
'    Dim ProRataGroupStart As Range
'    Dim CurrencyStart As Range
'    Dim OriginalAmtStart As Range
'    Dim CurrentAmtStart As Range
'    Dim RatingsStart As Range
'    Dim RefRateStart As Range
'    Dim CouponStart As Range
'    Dim FixFloatStart As Range
'    Dim PrevIPDCell As Range
'    Dim AccrIntrCell As Range
'    Dim ISINCell As Range
'    Set BondStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("BondsNamesHeader"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set ProRataGroupStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("BondRataGroupHeader"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set CurrencyStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CurrencyHead"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set OriginalAmtStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("OriginalOutHead"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set CurrentAmtStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("CurrentOutHead"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set RatingsStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("RatingsHead"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set RefRateStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("RefRateHead"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set CouponStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("TrancheCouponHead"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set FixFloatStart = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("FixFloatHead"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set AccrIntrCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("IntrAccrHead"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set ISINCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("IsinFldsHeader"), LookAt:=xlWhole, LookIn:=xlValues)
'    For i = LBound(TrancheExtensions) To UBound(TrancheExtensions)
'        TrancheExtensions(i) = CStr(BbgResponse(i, 1))
'        'InterestTypeString = CStr(bdp24(DealName + " " + TrancheExtensions(i) + " " + BloombergExtension, "MTG_TYP"))
'        BondStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Value = DealName + " " + TrancheExtensions(i)
'        TickerAddress = Replace(BondStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Address, "$", "")
'        ProRataGroupStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Value = i - LBound(TrancheExtensions) + 1
'        CurrencyStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Value = _
'            BBgInformations(i - LBound(TrancheExtensions))
'            '"=IF(" + TickerAddress + "="""","""",BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""CRNCY""))"
'        OriginalAmtStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Value = _
'            BBgInformations(UBound(TrancheExtensions) + 1 + i - LBound(TrancheExtensions))
'            '"=IF(" + TickerAddress + "="""","""",BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""MTG_ORIG_AMT""))"
'        ISINCell.Offset(i - LBound(TrancheExtensions) + 1, 0).Value = _
'            BBgInformations((3 * (UBound(TrancheExtensions) + 1)) + i - LBound(TrancheExtensions))
'        CurrentAmtStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Formula = _
'            "=IF(" + TickerAddress + "="""","""",BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""AMT_OUTSTANDING""))"
'        AccrIntrCell.Offset(i - LBound(TrancheExtensions) + 1, 0).Formula = _
'            "=IF(" + TickerAddress + "="""","""",BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""INT_ACC""))"
'        RefRateStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Formula = _
'            "=IF(" + TickerAddress + "="""","""",IF(LEFT(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""RESET_IDX""),4)=""#N/A"","""",BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""RESET_IDX"")))"
'        CouponStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Formula = _
'            "=if(" + FixFloatStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Address + "=""Float""," + _
'                "IF(" + TickerAddress + "="""","""",IF(LEFT(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""FLT_SPREAD""),4)=""#N/A"",0,BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""FLT_SPREAD"")))" _
'            + "," + _
'                "IF(" + TickerAddress + "="""","""",IF(LEFT(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""CPN""),4)=""#N/A"",0,100*BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""CPN"")))" _
'            + ")"
'        RatingsStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Formula = _
'            "=IF(" + TickerAddress + "="""",""""," + _
'                "IF(LEFT(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""RTG_MOODY""),4)=""#N/A"",""NR"",BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""RTG_MOODY""))" _
'                + " & "" | "" & " + "IF(LEFT(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""RTG_SP""),4)=""#N/A"",""NR"",BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""RTG_SP""))" _
'                + " & "" | "" & " + "IF(LEFT(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""RTG_FITCH""),4)=""#N/A"",""NR"",BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""RTG_FITCH""))" _
'            + ")"
'        If (InStr(1, BBgInformations(2 * (UBound(TrancheExtensions) + 1) + i - LBound(TrancheExtensions)), "FLT", vbTextCompare) > 0) Then
'            FixFloatStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Value = "Float"
'        Else
'            FixFloatStart.Offset(i - LBound(TrancheExtensions) + 1, 0).Value = "Fixed"
'        End If
'    Next i
'    Dim SettleDateCell As Range
'    Dim NextIPDCell As Range
'    Dim IDPFreqCell As Range
'    Set SettleDateCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("SettleDateField"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set NextIPDCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("FirstIPDfield"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set IDPFreqCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("IPDfrequencyField"), LookAt:=xlWhole, LookIn:=xlValues)
'    Set PrevIPDCell = Sheets(InputsSheet).Cells.Find(what:=FieldsLabels("PrevIPDField"), LookAt:=xlWhole, LookIn:=xlValues)
'    TickerAddress = Replace(BondStart.Offset(1, 0).Address, "$", "")
'    SettleDateCell.Offset(0, 1).Formula = "=IF(" + TickerAddress + "="""","""",DATEVALUE(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""SETTLE_DT"")))"
'    NextIPDCell.Offset(0, 1).Formula = "=IF(" + TickerAddress + "="""","""",DATEVALUE(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""MTG_NXT_PAY_DT_SET_DT"")))"
'    PrevIPDCell.Offset(0, 1).Formula = "=IF(" + TickerAddress + "="""","""",DATEVALUE(BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""MTG_START_ACC_DT_SET_DT"")))"
'    IDPFreqCell.Offset(0, 1).Formula = "=IF(" + TickerAddress + "="""","""",12/BDP(" + TickerAddress + " & "" " + BloombergExtension + """,""CPN_FREQ""))"
'    Application.ScreenUpdating = True
'End Sub
Public Sub AddAccruedColumn()
Dim AccrIntrCell As Range
Dim BondNameCell As Range
On Error Resume Next
Set AccrIntrCell = Cells.Find(what:="Accrued Interest", LookAt:=xlWhole, LookIn:=xlValues)
On Error GoTo 0
If (Not AccrIntrCell Is Nothing) Then Exit Sub
Set BondNameCell = Cells.Find(what:="Bond", LookAt:=xlWhole, LookIn:=xlValues)
Set AccrIntrCell = Cells.Find(what:="IC test", LookAt:=xlWhole, LookIn:=xlValues).Offset(0, 1)
With AccrIntrCell
    .Value = "Accrued Interest"
    With .Borders
        .LineStyle = xlContinuous
        .Weight = xlMedium
        .ColorIndex = xlAutomatic
    End With
    .Font.Bold = True
    .HorizontalAlignment = xlLeft
    .Interior.Color = RGB(118, 147, 60)
End With
With AccrIntrCell.Offset(1, 0).Resize(16, 1)
    .Interior.Color = RGB(216, 228, 188)
    With .Borders
        .LineStyle = xlContinuous
        .Weight = xlMedium
        .ColorIndex = xlAutomatic
    End With
    .Borders(xlInsideVertical).LineStyle = xlNone
    .Borders(xlInsideHorizontal).LineStyle = xlNone
End With
With AccrIntrCell.Offset(17, 0)
    With .Borders
        .LineStyle = xlContinuous
        .Weight = xlMedium
        .ColorIndex = xlAutomatic
    End With
    .HorizontalAlignment = xlLeft
    .Borders(xlEdgeLeft).LineStyle = xlNone
    .Interior.Color = RGB(118, 147, 60)
    .NumberFormat = "0.00"
End With
Dim i As Long
For i = 1 To 16
    AccrIntrCell.Offset(i, 0).Formula = _
        "=IF(" + BondNameCell.Offset(i, 0).Address + "="""","""",BDP(" + BondNameCell.Offset(i, 0).Address + " & "" " + "Mtge" + """,""INT_ACC""))"
Next i
End Sub
Public Sub RunCLOModel(SaveBase As Boolean)
    Application.ScreenUpdating = False
    Dim FieldsLabels As New Collection
    Sheets("Tranches Results").Unprotect
    Sheets("Mortgages Results").Unprotect
    Sheets("Graphical Output").Unprotect
    Sheets("Liabilities + input").Unprotect
    Call PopulateDafaultLabels(FieldsLabels)
    Sheets("Mortgages Results").Cells.Clear
    Sheets("Tranches Results").Cells.Clear
    Call GetInputFromStructure( _
        "Loans Pool" _
        , "Liabilities + input" _
        , Left(ActiveWorkbook.FullName, InStrRev(ActiveWorkbook.FullName, "\")) _
        , FieldsLabels _
        , SaveBase _
    )
    With Sheets("Liabilities + input")
      .EnableCalculation = False
      .EnableCalculation = True
      .Calculate
    End With
    Sheets("Tranches Results").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Sheets("Mortgages Results").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Sheets("Graphical Output").Protect
    Sheets("Graphical Output").EnableSelection = xlUnlockedCells
    Sheets("Liabilities + input").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True
End Sub
Public Function ShowDayCount(Target As String, CurrentDcnt As Integer) As String
    Call DayCountForm.SetDayCount(CurrentDcnt)
    DayCountForm.TargetAddress = Target
    ShowDayCount = DayCountForm.ShowForm
End Function
