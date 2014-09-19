VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Sheet15"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Private Sub Worksheet_BeforeDoubleClick(ByVal Target As Range, Cancel As Boolean)
    Dim returnedValue As String
    If (Target.Count = 1) And ( _
        (Not Intersect(Target, Me.Cells.Find("Day Count", LookAt:=xlWhole, LookIn:=xlValues).Offset(1, 0).Resize(500, 1)) Is Nothing) _
    ) Then
        On Error GoTo DefaulttargetValue
        returnedValue = ShowDayCount(Me.Cells(Target.Row, Me.Cells.Find("Facility", LookAt:=xlWhole, LookIn:=xlValues).Column).Address, CInt(Target.Value))
        If False Then
DefaulttargetValue:
        returnedValue = ShowDayCount(Me.Cells(Target.Row, Me.Cells.Find("Facility", LookAt:=xlWhole, LookIn:=xlValues).Column).Address, 0)
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

