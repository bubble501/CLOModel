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
        CurrRowDim = 0
        CurrColDim = 1
    ElseIf (StressGrid = 2) Then
        CurrRowDim = 2
        CurrColDim = 1
    Else
        CurrRowDim = 2
        CurrColDim = 0
    End If
    Call GetInputFromStructure( _
        "Loans Pool" _
        , "Liabilities + input" _
        , Left(ActiveWorkbook.FullName, InStrRev(ActiveWorkbook.FullName, "\")) _
        , FieldsLabels _
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
    Call StructureFromBloomberg(FieldsLabels, "Liabilities + input")
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
    DestinationBook.Sheets("Liabilities + input").Range("B1").formula = SourceBook.Sheets("Liabilities + input").Range("B1").formula
    DestinationBook.Sheets("Liabilities + input").Range("D1").formula = SourceBook.Sheets("Liabilities + input").Range("D1").formula
    DestinationBook.Sheets("Liabilities + input").Range("F1").formula = SourceBook.Sheets("Liabilities + input").Range("F1").formula
    DestinationBook.Sheets("Liabilities + input").Range("H1").formula = SourceBook.Sheets("Liabilities + input").Range("H1").formula
    For i = 1 To 16
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 0).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 0).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 1).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 1).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 2).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 2).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 3).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 3).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 5).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 5).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 8).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 8).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 10).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 10).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 11).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 11).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 12).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 12).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 13).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 13).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 22).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 22).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 23).formula = SourceBook.Sheets("Liabilities + input").Range("A3").Offset(i, 23).formula
        DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 24).formula = "=BDP(" + DestinationBook.Sheets("Liabilities + input").Range("A3").Offset(i, 24).Address + "& "" Mtge"",""INT_ACC"")"
    Next i
    For i = 1 To 47
        If Not ( _
        i = 3 Or i = 4 Or i = 10 Or i = 11 Or i = 16 Or i = 17 Or i = 18 Or i = 28 Or i = 29 Or i = 38 Or i = 41 Or i = 42 Or i = 43 Or i = 44 _
        ) Then
            DestinationBook.Sheets("Liabilities + input").Range("B23").Offset(i, 0).formula = SourceBook.Sheets("Liabilities + input").Range("B23").Offset(i, 0).formula
        End If
    Next i
    DestinationBook.Sheets("Liabilities + input").Range("C62").formula = SourceBook.Sheets("Liabilities + input").Range("C62").formula
    DestinationBook.Sheets("Liabilities + input").Range("C63").formula = SourceBook.Sheets("Liabilities + input").Range("C63").formula
    i = 1
    Do While (True)
        If (IsEmpty(SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 0))) Then Exit Do
        DestinationBook.Sheets("Liabilities + input").Range("E24").Offset(i, 0).formula = SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 0).formula
        If Not IsEmpty(SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 1)) Then
            DestinationBook.Sheets("Liabilities + input").Range("E24").Offset(i, 1).formula = SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 1).formula
        End If
        If Not IsEmpty(SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 2)) Then
            DestinationBook.Sheets("Liabilities + input").Range("E24").Offset(i, 2).formula = SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 2).formula
        End If
        If Not IsEmpty(SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 3)) Then
            DestinationBook.Sheets("Liabilities + input").Range("E24").Offset(i, 3).formula = SourceBook.Sheets("Liabilities + input").Range("E24").Offset(i, 3).formula
        End If
        i = i + 1
    Loop
    Dim j As Long
    i = 1
    Do While (True)
        If (IsEmpty(SourceBook.Sheets("Loans Pool").Range("D1").Offset(i, 0))) Then Exit Do
        For j = 1 To 15
            DestinationBook.Sheets("Loans Pool").Range("A1").Offset(i, j).formula = SourceBook.Sheets("Loans Pool").Range("A1").Offset(i, j).formula
        Next j
        i = i + 1
    Loop
    Application.Calculation = xlCalculationAutomatic
End Sub
