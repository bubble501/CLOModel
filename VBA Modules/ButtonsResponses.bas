Attribute VB_Name = "ButtonsResponses"
Option Explicit
Public Sub RunModel()
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
