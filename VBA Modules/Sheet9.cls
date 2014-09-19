VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Sheet9"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Private Sub Worksheet_Change(ByVal Target As Range)
    If Not Intersect(Target, Me.Range("E25:E150")) Is Nothing Then
        Me.Unprotect
        Call WaterfallStepChanged(Target)
        Me.Protect AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    ElseIf Not Intersect(Target, Me.Range("A4:A19")) Is Nothing Then
        Me.Shapes("GhostButton").Visible = _
            Application.WorksheetFunction.CountBlank(Me.Range("A4:A19")) = 16
        Me.Shapes("ImportFromOldButton").Visible = _
            Application.WorksheetFunction.CountBlank(Me.Range("A4:A19")) = 16
    End If
End Sub

Private Sub Worksheet_SelectionChange(ByVal Target As Range)
    If (Target.Count = 1) And (Not IsEmpty(Target)) And ( _
        (Not Intersect(Target, Me.Cells.Find("CPR", LookAt:=xlWhole, LookIn:=xlValues).Offset(0, 1)) Is Nothing) Or _
        (Not Intersect(Target, Me.Cells.Find("CDR", LookAt:=xlWhole, LookIn:=xlValues).Offset(0, 1)) Is Nothing) Or _
        (Not Intersect(Target, Me.Cells.Find("LS", LookAt:=xlWhole, LookIn:=xlValues).Offset(0, 1)) Is Nothing) Or _
        (Not Intersect(Target, Me.Cells.Find("CCC curve", LookAt:=xlWhole, LookIn:=xlValues).Offset(0, 1)) Is Nothing) Or _
        (Not Intersect(Target, Me.Cells.Find("Reinvestment WAL", LookAt:=xlWhole, LookIn:=xlValues).Offset(0, 1)) Is Nothing) Or _
        (Not Intersect(Target, Me.Cells.Find("Reinvestment Spread", LookAt:=xlWhole, LookIn:=xlValues).Offset(0, 1)) Is Nothing) _
    ) Then
        Me.Unprotect
        Me.ChartObjects("BBgVectorPlot").Top = Target.Top - 10
        Me.ChartObjects("BBgVectorPlot").Left = Target.Left + Target.Width + 10
        Me.ChartObjects("BBgVectorPlot").Visible = True
        Call PlotBloobergVector(Target.Value, Me.ChartObjects("BBgVectorPlot").Chart, _
            Application.WorksheetFunction.Max(36526, Me.Cells.Find("Pool cut off date").Offset(0, 1).Value))
        Me.Protect AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Else
        Me.ChartObjects("BBgVectorPlot").Visible = False
    End If

End Sub
Private Sub Worksheet_BeforeDoubleClick(ByVal Target As Range, Cancel As Boolean)
    Dim returnedValue As String
    If (Target.Count = 1) And ( _
        (Not Intersect(Target, Me.Cells.Find("Day Count", LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0).Resize(16, 1)) Is Nothing) Or _
        (Not Intersect(Target, Me.Cells.Find("Deal Day Count Convention", LookAt:=xlWhole, LookIn:=xlValues).Offset(0, 1)) Is Nothing) _
    ) Then
        On Error GoTo DefaulttargetValue
        returnedValue = ShowDayCount(Me.Cells(Target.Row, Me.Cells.Find("Bond", LookAt:=xlWhole, LookIn:=xlValues).Column).Address, CInt(Target.Value))
        If False Then
DefaulttargetValue:
        returnedValue = ShowDayCount(Me.Cells(Target.Row, Me.Cells.Find("Bond", LookAt:=xlWhole, LookIn:=xlValues).Column).Address, 0)
        Resume Next
        End If
        If (returnedValue <> "") Then
            Me.Unprotect
            Application.Calculation = xlCalculationManual
            Me.Range(Target.Address).Formula = returnedValue
            Application.Calculation = xlCalculationAutomatic
            Me.Protect AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
        End If
        Cancel = True
    End If
End Sub
