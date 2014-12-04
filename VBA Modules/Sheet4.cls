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
Private Sub Worksheet_BeforeDoubleClick(ByVal Target As Range, Cancel As Boolean)
    Dim allFields As New Collection
    PopulateDafaultLabels allFields
    On Error GoTo StepNotFoundError
    Application.EnableEvents = False
    Dim FirstStep As Range
    Dim LastStep As Range
    Set FirstStep = Me.Cells.Find(What:=allFields("StepHead"), LookAt:=xlWhole, LookIn:=xlFormulas)
    Set LastStep = Me.Cells.Find(What:=allFields("StepHead"), SearchDirection:=xlPrevious, LookAt:=xlWhole, LookIn:=xlFormulas)
    Dim singleCell As Range
    Me.Unprotect
    For Each singleCell In Target
    If (singleCell.Row > FirstStep.Row And singleCell.Column = FirstStep.Column) Then
        Call EditWaterfallStep("Liabilities + input", singleCell, allFields, singleCell.Row < LastStep.Row)
    End If
    Next singleCell
StepNotFoundError:
Cancel = True
Application.EnableEvents = True
On Error GoTo 0
End Sub

