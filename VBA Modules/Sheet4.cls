VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Sheet4"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Option Explicit
Private Sub Worksheet_Change(ByVal Target As Range)
    Dim allFields As New Collection
    PopulateDafaultLabels allFields
    On Error GoTo StepNotFoundError
    Application.EnableEvents = False
    Dim StepFound As Range
    Set StepFound = Me.Cells.Find(what:=allFields("StepHead"), LookAt:=xlWhole, LookIn:=xlFormulas)
    Dim singleCell As Range
    For Each singleCell In Target
    If (singleCell.Row > StepFound.Row And singleCell.Column = StepFound.Column) Then
        Me.Unprotect
        Call NewWaterfallStepChanged(singleCell, allFields)
        Me.Protect
    End If
    Next singleCell
StepNotFoundError:
Application.EnableEvents = True
On Error GoTo 0
End Sub

