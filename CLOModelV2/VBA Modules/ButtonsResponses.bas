Attribute VB_Name = "ButtonsResponses"
Option Explicit
Public Sub RunBaseModel()
Call RunCLOModel(True)
End Sub
Public Sub RunTestModel()
Call RunCLOModel(False)
End Sub
Private Sub RunStress(StressGrid As Long)
    If (StressGrid < 1 Or StressGrid > 3) Then Exit Sub
    Dim CurrRowDim As Long, CurrColDim As Long
    Application.ScreenUpdating = False
    Sheets("Stress Test").Unprotect
    Sheets("Tranches Results").Unprotect
    Sheets("Mortgages Results").Unprotect
    Sheets("Graphical Output").Unprotect
    Sheets("Liabilities + input").Unprotect
    Dim FieldsLabels As New Collection
    Call PopulateDafaultLabels(FieldsLabels)
    FieldsLabels.Add "Field" + Format(((StressGrid - 1) * 3) + 1, "0"), "StressCPRField"
    FieldsLabels.Add "Field" + Format(((StressGrid - 1) * 3) + 2, "0"), "StressLSHead"
    FieldsLabels.Add "Field" + Format(StressGrid * 3, "0"), "StressCDRHead"
    If (StressGrid = 1) Then
        CurrRowDim = 1
        CurrColDim = 2
    ElseIf (StressGrid = 2) Then
        CurrRowDim = 4
        CurrColDim = 2
    Else
        CurrRowDim = 4
        CurrColDim = 1
    End If
    Call GetInputFromStructure( _
        "Loans Pool" _
        , "Liabilities + input" _
        , Left(ActiveWorkbook.FullName, InStrRev(ActiveWorkbook.FullName, "\")) _
        , FieldsLabels _
        , False _
        , True _
        , StressTestSheet:="Stress Test" _
        , StressRowDimension:=CurrRowDim _
        , StressColDimension:=CurrColDim _
    )
    With Sheets("Liabilities + input")
      .EnableCalculation = False
      .EnableCalculation = True
      .Calculate
    End With
    Sheets("Stress Test").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Sheets("Stress Test").Range("A1").Value = Sheets("Stress Test").Range("A1").Value
    Sheets("Tranches Results").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Sheets("Mortgages Results").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Sheets("Graphical Output").Protect
    Sheets("Graphical Output").EnableSelection = xlUnlockedCells
    Sheets("Liabilities + input").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True
End Sub
Public Sub SeparateWaterfalls()
    Dim FieldNames As New Collection
    Call PopulateDafaultLabels(FieldNames)
    Call SeparateWaterfall("Liabilities + input", FieldNames)
End Sub

Public Sub RunStress1()
    Call RunStress(1)
End Sub
Public Sub RunStress2()
    Call RunStress(2)
End Sub
Public Sub RunStress3()
    Call RunStress(3)
End Sub
Public Sub RunAllStress()
    Call RunStress(1)
    Call RunStress(2)
    Call RunStress(3)
End Sub

Public Sub getStructureFromBloomberg()
    Sheets("Liabilities + input").Unprotect
    Dim FieldsLabels As New Collection
    Call PopulateDafaultLabels(FieldsLabels)
    'Call StructureFromBloomberg(FieldsLabels, "Liabilities + input")
    Sheets("Liabilities + input").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True
End Sub

Private Sub ResetToDefault()
    Dim i As Long
    Application.ScreenUpdating = False
    Sheets("Tranches Results").Unprotect
    Sheets("Mortgages Results").Unprotect
    Sheets("Graphical Output").Unprotect
    Sheets("Liabilities + input").Unprotect
    Sheets("Stress Test").Unprotect
    
    Sheets("Mortgages Results").Cells.Clear
    Sheets("Tranches Results").Cells.Clear
    Sheets("Raw Portfolio").Cells.Clear
    Sheets("Loans Pool").Range("B2:P351").ClearContents
    
    With Sheets("Graphical Output")
        For i = 1 To .ChartObjects.Count
            While (.ChartObjects(i).Chart.SeriesCollection.Count > 0)
                .ChartObjects(i).Chart.SeriesCollection(1).Delete
            Wend
        Next i
    End With
    With Sheets("Stress Test")
        For i = 1 To .ChartObjects.Count
            While (.ChartObjects(i).Chart.SeriesCollection.Count > 0)
                .ChartObjects(i).Chart.SeriesCollection(1).Delete
            Wend
        Next i
    End With
    
    Sheets("Stress Test").Range("A1").Value = ""
    If Not IsEmpty(Sheets("Stress Test").Range("C3")) Then Range(Sheets("Stress Test").Range("C3"), Sheets("Stress Test").Range("C3").End(xlDown).End(xlToRight)).Clear
    If Not IsEmpty(Sheets("Stress Test").Range("q3")) Then Range(Sheets("Stress Test").Range("Q3"), Sheets("Stress Test").Range("Q3").End(xlDown).End(xlToRight)).Clear
    If Not IsEmpty(Sheets("Stress Test").Range("ae3")) Then Range(Sheets("Stress Test").Range("AE3"), Sheets("Stress Test").Range("AE3").End(xlDown).End(xlToRight)).Clear
        
    
    With Sheets("Liabilities + input")
        .Range("A4:D19").ClearContents
        .Range("f4:f19").ClearContents
        .Range("i4:n19").ClearContents
        .Range("r4:r19").ClearContents
        .Range("v4:x19").ClearContents
        .Range("b24:b38").ClearContents
        .Range("b42:b60").ClearContents
        .Range("b62:c63").ClearContents
        .Range("a83:b100").ClearContents
        .Range("b1").ClearContents
        .Range("d1").ClearContents
        .Range("f1").ClearContents
        .Range("h1").ClearContents
        .Range("e25:h150").ClearContents
    End With
    Sheets("Stress Test").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Sheets("Tranches Results").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Sheets("Mortgages Results").Protect UserInterfaceOnly:=True, AllowFormattingCells:=True, AllowFormattingColumns:=True, AllowFormattingRows:=True
    Sheets("Graphical Output").Protect
    Sheets("Graphical Output").EnableSelection = xlUnlockedCells
    Application.ScreenUpdating = True
End Sub
Public Sub ImportFromOldVersion()
    Application.ScreenUpdating = False
    Application.Calculation = xlCalculationManual
    Dim fd As FileDialog
    Dim i As Long
    Set fd = Application.FileDialog(msoFileDialogFilePicker)
    Dim DestinationBook As Workbook
    Set DestinationBook = Application.ActiveWorkbook
    Dim SourceBook As Workbook
    Dim vrtSelectedItem As Variant
    With fd
        If .Show = -1 Then
            For Each vrtSelectedItem In .SelectedItems
                Set SourceBook = Workbooks.Open(vrtSelectedItem, True, True)
                Exit For
            Next vrtSelectedItem
        End If
    End With
    Set fd = Nothing
    Dim TempSheet As Worksheet
    
    
    On Error Resume Next
    Set TempSheet = DestinationBook.Sheets("Raw Portfolio")
    If (TempSheet Is Nothing) Then
        DestinationBook.Sheets.Add.Name = "Raw Portfolio"
    Else
        TempSheet.Cells.Clear
    End If
    Set TempSheet = Nothing
    Set TempSheet = DestinationBook.Sheets("Monitoring")
    If (TempSheet Is Nothing) Then
        DestinationBook.Sheets.Add.Name = "Monitoring"
    Else
        TempSheet.Cells.Clear
    End If
    On Error GoTo 0
    SourceBook.Sheets("Raw Portfolio").Cells.Copy Destination:=DestinationBook.Sheets("Raw Portfolio").Cells
    SourceBook.Sheets("Monitoring").Cells.Copy Destination:=DestinationBook.Sheets("Monitoring").Cells
    DestinationBook.Sheets("Liabilities + input").Range("B1").Formula = SourceBook.Sheets("Liabilities + input").Range("B1").Formula
    DestinationBook.Sheets("Liabilities + input").Range("D1").Formula = SourceBook.Sheets("Liabilities + input").Range("D1").Formula
    DestinationBook.Sheets("Liabilities + input").Range("F1").Formula = SourceBook.Sheets("Liabilities + input").Range("F1").Formula
    DestinationBook.Sheets("Liabilities + input").Range("H1").Formula = SourceBook.Sheets("Liabilities + input").Range("H1").Formula
    For i = 1 To 16
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 0).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 0).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 1).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 1).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 2).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 2).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 3).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 3).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 5).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 5).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 8).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 8).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 10).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 10).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 11).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 11).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 12).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 12).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 13).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 13).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 22).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 22).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 23).Formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 23).Formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 24).Formula = "=BDP(" + DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 24).Address + "& "" Mtge"",""INT_ACC"")"
    Next i
    For i = 1 To 47
        If Not ( _
        i = 3 Or i = 4 Or i = 10 Or i = 11 Or i = 16 Or i = 17 Or i = 18 Or i = 28 Or i = 29 Or i = 38 Or i = 41 Or i = 42 Or i = 43 Or i = 44 _
        ) Then
            DestinationBook.Sheets("Liabilities + input").Range("B23").Offset(i, 0).Formula = SourceBook.Sheets("Liabilities + input").Range("B23").Offset(i, 0).Formula
        End If
    Next i
    DestinationBook.Sheets("Liabilities + input").Range("C62").Formula = SourceBook.Sheets("Liabilities + input").Range("C62").Formula
    DestinationBook.Sheets("Liabilities + input").Range("C63").Formula = SourceBook.Sheets("Liabilities + input").Range("C63").Formula
    i = 1
    Do While (True)
        If (IsEmpty(SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 0))) Then Exit Do
        DestinationBook.Sheets("Liabilities + input").Range("E24").Offset(i, 0).Formula = SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 0).Formula
        If Not IsEmpty(SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 1)) Then
            DestinationBook.Sheets("Liabilities + input").Range("E24").Offset(i, 1).Formula = SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 1).Formula
        End If
        If Not IsEmpty(SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 2)) Then
            DestinationBook.Sheets("Liabilities + input").Range("E24").Offset(i, 2).Formula = SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 2).Formula
        End If
        If Not IsEmpty(SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 3)) Then
            DestinationBook.Sheets("Liabilities + input").Range("E24").Offset(i, 3).Formula = SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 3).Formula
        End If
        i = i + 1
    Loop
    Dim j As Long
    i = 1
    Do While (True)
        If (IsEmpty(SourceBook.Sheets("Loans Pool").Range("D1").Offset(i, 0))) Then Exit Do
        For j = 1 To 15
            DestinationBook.Sheets("Loans Pool").Range("A1").Offset(i, j).Formula = SourceBook.Sheets("Loans Pool").Range("A1").Offset(i, j).Formula
        Next j
        i = i + 1
    Loop
    Application.Calculation = xlCalculationAutomatic
End Sub
Sub Forward_Curves_Check()
    Application.ScreenUpdating = False
    Dim i As Long
    i = 1
    While Sheets("Liabilities + input").CheckBoxes(i).Text <> "Forward Curves"
        If (i = Sheets("Liabilities + input").CheckBoxes.Count) Then Exit Sub
        i = i + 1
    Wend
    If Sheets("Liabilities + input").CheckBoxes(i).Value = xlOn Then
        Sheets("Forward Curves").Visible = xlSheetVisible
    Else
        Sheets("Forward Curves").Visible = xlSheetVeryHidden
    End If
    Application.ScreenUpdating = True
End Sub
Public Sub GuessScenario()
    Call EditLoanScenarios
End Sub

