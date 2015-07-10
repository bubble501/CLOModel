VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} DayCountForm 
   Caption         =   "Set Day Count Convention"
   ClientHeight    =   5640
   ClientLeft      =   45
   ClientTop       =   375
   ClientWidth     =   5280
   OleObjectBlob   =   "DayCountForm.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "DayCountForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Public result As String
Public TargetAddress As String

Private Sub AddStepButton_Click()
    If (Me.DayCountCombo.Value = "") Or (Me.InterestTypeCombo.Value = "") Then
        Exit Sub
    Else
        If VectorBox.Text = "" Then
            VectorBox.Text = CStr(Me.DayCountCombo.List(Me.DayCountCombo.ListIndex, 1) + Me.InterestTypeCombo.List(Me.InterestTypeCombo.ListIndex, 1))
        Else
            VectorBox.Text = VectorBox.Text & " " & StepBox.Text & "S " & _
                CStr(Me.DayCountCombo.List(Me.DayCountCombo.ListIndex, 1) + Me.InterestTypeCombo.List(Me.InterestTypeCombo.ListIndex, 1))
        End If
        StepSpinn.Enabled = True
        StepBox.Enabled = True
        StepLabel.Enabled = True
        StepBox.Text = "1"
        DayCountCombo.Value = Null
        InterestTypeCombo.Value = Null
        AnchorDatePick.Enabled = True
        AnchorLabel.Enabled = True
    End If
End Sub

Private Sub AnchorDatePick_Change()
    If (Left(VectorBox.Text, 1) = "A") Then
        VectorBox.Text = Right(VectorBox.Text, Len(VectorBox.Text) - 13)
    End If
    If Not IsNull(AnchorDatePick.Value) Then
        VectorBox.Text = "A " & Format(AnchorDatePick.Month, "00") & "/" & Format(AnchorDatePick.Day, "00") & "/" & Format(AnchorDatePick.Year, "0000") & " " & VectorBox.Text
    End If
End Sub

Private Sub BbgButton_Click()
    result = "=bdp(" + TargetAddress + " & "" Mtge"",""DAY_CNT"")"
    Me.Hide
End Sub
Private Sub CancelButton_Click()
    result = ""
    Me.Hide
End Sub



Private Sub ClearButton_Click()
        StepSpinn.Enabled = False
        StepBox.Enabled = False
        StepLabel.Enabled = False
        StepBox.Text = ""
        DayCountCombo.Value = Null
        InterestTypeCombo.Value = Null
        AnchorDatePick.Enabled = False
        AnchorLabel.Enabled = False
        VectorBox.Text = ""
End Sub

Private Sub kButton_Click()

    If (Me.DayCountCombo.Value = "") Or (Me.InterestTypeCombo.Value = "") Then
         result = ""
    ElseIf (VectorBox.Text <> "") And (Not ((Left(VectorBox.Text, 1) = "A") And (Len(VectorBox.Text) = 13))) Then
        result = VectorBox.Text
    Else
        result = CStr(Me.DayCountCombo.List(Me.DayCountCombo.ListIndex, 1) + Me.InterestTypeCombo.List(Me.InterestTypeCombo.ListIndex, 1))
    End If
    Me.Hide
End Sub


Private Sub StepBox_Change()
    If (Val(StepBox.Text) = 0) Then StepBox.Text = ""
End Sub

Private Sub StepSpinn_SpinDown()
    If (Val(StepBox.Text) > 1) Then
        StepBox.Text = Val(StepBox.Text) - 1
    End If
End Sub

Private Sub StepSpinn_SpinUp()
    StepBox.Text = Val(StepBox.Text) + 1
End Sub

Private Sub StepBox_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
    Select Case KeyAscii
      Case vbKey0 To vbKey9, 8
      Case Else
        KeyAscii = 0
    End Select
End Sub


Private Sub UserForm_Initialize()
    Dim DayCountsArray(0 To 11, 0 To 1)
    DayCountsArray(0, 0) = "ACT/ACT"
    DayCountsArray(0, 1) = 101
    DayCountsArray(1, 0) = "ACT/360"
    DayCountsArray(1, 1) = 102
    DayCountsArray(2, 0) = "ACT/365"
    DayCountsArray(2, 1) = 103
    DayCountsArray(3, 0) = "30/360 Nominal"
    DayCountsArray(3, 1) = 104
    DayCountsArray(4, 0) = "ACT/ACT Nominal"
    DayCountsArray(4, 1) = 105
    DayCountsArray(5, 0) = "ACT/360 Nominal"
    DayCountsArray(5, 1) = 106
    DayCountsArray(6, 0) = "ACT/365 Nominal"
    DayCountsArray(6, 1) = 107
    DayCountsArray(7, 0) = "ISMA-30/360"
    DayCountsArray(7, 1) = 131
    DayCountsArray(8, 0) = "ISDA ACT/ACT"
    DayCountsArray(8, 1) = 201
    DayCountsArray(9, 0) = "AFB ACT/ACT"
    DayCountsArray(9, 1) = 202
    DayCountsArray(10, 0) = "ISDA ACT/ACT Nominal"
    DayCountsArray(10, 1) = 203
    DayCountsArray(11, 0) = "AFB ACT/ACT Nominal"
    DayCountsArray(11, 1) = 204
    Me.DayCountCombo.List = DayCountsArray
    
    Dim InterestTypeArray(0 To 2, 0 To 1)
    InterestTypeArray(0, 0) = "Simple"
    InterestTypeArray(0, 1) = 0
    InterestTypeArray(1, 0) = "Compounded"
    InterestTypeArray(1, 1) = 1024
    InterestTypeArray(2, 0) = "Continuously Compounded"
    InterestTypeArray(2, 1) = 2048
    Me.InterestTypeCombo.List = InterestTypeArray
    
End Sub
Public Sub SetDayCount(CurrentDayCnt As Integer)
Dim DayCountsArray
DayCountsArray = Me.DayCountCombo.List
If (CurrentDayCnt >= 2048) Then
        Me.InterestTypeCombo.ListIndex = 2
        CurrentDayCnt = CurrentDayCnt - 2048
    ElseIf (CurrentDayCnt >= 1024) Then
        Me.InterestTypeCombo.ListIndex = 1
        CurrentDayCnt = CurrentDayCnt - 1024
    Else
        Me.InterestTypeCombo.ListIndex = 0
    End If
    If (DayCountCombo > 0) Then
        Dim i As Integer
        For i = 0 To 11
            If (DayCountsArray(i, 1) = CurrentDayCnt) Then
                Me.DayCountCombo.ListIndex = i
                Exit For
            End If
        Next i
    End If
End Sub
Public Function ShowForm()
    Me.Show vbModal
    ShowForm = result
    Unload Me
End Function
