VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Sheet8"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Private Sub Worksheet_Change(ByVal Target As Range)
    Dim PriceCell As Range
    Set PriceCell = Me.Cells.Find(what:="Price", LookAt:=xlPart, LookIn:=xlValues).Offset(0, 1)
    If (Not Intersect(Target, Me.Range("A1")) Is Nothing) Or (Not Intersect(Target, PriceCell) Is Nothing) Then
        If Not IsEmpty(Me.Range("A1")) Then
            Me.Unprotect
            Dim NewPrice As Double
            Dim tempstring As String
            NewPrice = PriceCell.Value
            Application.ScreenUpdating = False
            Call StressTargetEvent(Me.Range("A1").Value, Me.Name, "C3", 0, 1, NewPrice, Me.Name, 1)
            Call StressTargetEvent(Me.Range("A1").Value, Me.Name, "Q3", 2, 1, NewPrice, Me.Name, 2)
            Call StressTargetEvent(Me.Range("A1").Value, Me.Name, "AE3", 2, 0, NewPrice, Me.Name, 3)
            Application.ScreenUpdating = True
            Me.Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
         End If
    End If
End Sub
'Private Sub Worksheet_SelectionChange(ByVal Target As Range)
'    If (Me.Range("A2").Value And (Target.Count = 1) And (Not IsEmpty(Target))) Then
'         If _
'            (Not Intersect(Target, Range(Me.Range("C3"), Me.Range("C3").End(xlDown).End(xlToRight))) Is Nothing) _
'            Or (Not Intersect(Target, Range(Me.Range("Q3"), Me.Range("Q3").End(xlDown).End(xlToRight))) Is Nothing) _
'            Or (Not Intersect(Target, Range(Me.Range("AE3"), Me.Range("AE3").End(xlDown).End(xlToRight))) Is Nothing) _
'         Then
'            Dim RelvantColumn As Long
'            Dim Params(0 To 4) As Variant
'            If Target.Column >= 31 Then
'                RelvantColumn = 30 'AD
'                Params(3) = CLng(2)
'                Params(4) = CLng(0)
'            ElseIf Target.Column >= 17 Then
'                RelvantColumn = 16 'P
'                Params(3) = CLng(2)
'                Params(4) = CLng(1)
'            Else
'                RelvantColumn = 2 'B
'                Params(3) = CLng(0)
'                Params(4) = CLng(1)
'            End If
'            Params(0) = Left(ActiveWorkbook.FullName, InStrRev(ActiveWorkbook.FullName, "\"))
'            Params(1) = CStr(Cells(Target.Row, RelvantColumn))
'            Params(2) = CStr(Cells(2, Target.Column))
'            Call InspectStress(Params)
'         End If
'    End If
'End Sub
