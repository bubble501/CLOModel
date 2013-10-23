Attribute VB_Name = "Outputs"
Option Explicit
Public Sub PrintTranche( _
    Data As Variant, _
    DestinatAddress As String, _
    TrancheName As String, _
    BackgrndColor As String, _
    PrintDates As Boolean, _
    PrintOutstanding As Boolean, _
    PrintInterest As Boolean, _
    PrintPrincipal As Boolean, _
    PrintTotalFlow As Boolean, _
    PrintDeferred As Boolean, _
    PrintOCtest As Boolean, _
    PrintICtest As Boolean, _
    TargetICtest As Double, _
    TargetOCtest As Double _
)
    Application.ScreenUpdating = False
    Dim ColOffset As Long, i As Long
    ColOffset = 0
    Dim TargetCell As Range
    Dim DestinationAddress As String
    DestinationAddress = Right(DestinatAddress, Len(DestinatAddress) - InStr(1, DestinatAddress, "!"))
    Set TargetCell = Sheets(Left(DestinatAddress, InStr(1, DestinatAddress, "!") - 1)).Cells( _
            CLng(Left(DestinationAddress, InStr(1, DestinationAddress, ",") - 1)), _
            CLng(Right(DestinationAddress, Len(DestinationAddress) - InStr(1, DestinationAddress, ","))) _
        )
    If (TargetCell.Count > 1) Then Set TargetCell = TargetCell.Cells(1, 1)
    Set TargetCell = TargetCell.Offset(1, 0)
    If (PrintDates) Then
        TargetCell.Offset(0, ColOffset).Value = "Date"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 8
        ColOffset = ColOffset + 1
    End If
    If (PrintOutstanding) Then
        TargetCell.Offset(0, ColOffset).Value = "Outstanding"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintInterest) Then
        TargetCell.Offset(0, ColOffset).Value = "Interest"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintPrincipal) Then
        TargetCell.Offset(0, ColOffset).Value = "Principal"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintTotalFlow) Then
        TargetCell.Offset(0, ColOffset).Value = "Total Cash Flow"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintDeferred) Then
        TargetCell.Offset(0, ColOffset).Value = "Cumulative Deferred Interest"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintOCtest) Then
        TargetCell.Offset(0, ColOffset).Value = "OC Test"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 8
        ColOffset = ColOffset + 1
    End If
    If (PrintICtest) Then
        TargetCell.Offset(0, ColOffset).Value = "IC Test"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 8
        ColOffset = ColOffset + 1
    End If
    Dim TempOff As Integer
    If (PrintDates) Then
        TempOff = 1
    Else
        TempOff = 0
    End If
    With TargetCell.Offset(-1, TempOff)
        .Value = TrancheName
        With .Resize(1, ColOffset - TempOff)
            With .Borders
                .LineStyle = xlContinuous
                .Weight = xlMedium
                .ColorIndex = xlAutomatic
            End With
            .Font.Bold = True
            .HorizontalAlignment = xlCenter
            .Merge
        End With
    End With
    For i = LBound(Data, 1) To UBound(Data, 1)
        ColOffset = 0
        If (PrintDates) Then
            TargetCell.Offset(i, ColOffset).Value = Data(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "mmm-yy"
            ColOffset = ColOffset + 1
        End If
        If (PrintOutstanding) Then
            TargetCell.Offset(i, ColOffset).Value = Data(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintInterest) Then
            TargetCell.Offset(i, ColOffset).Value = Data(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintPrincipal) Then
            TargetCell.Offset(i, ColOffset).Value = Data(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintTotalFlow) Then
            TargetCell.Offset(i, ColOffset).Value = Data(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintDeferred) Then
            TargetCell.Offset(i, ColOffset).Value = Data(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintOCtest) Then
            TargetCell.Offset(i, ColOffset).Value = Data(i, ColOffset)
            If (TargetOCtest > 0) Then
                TargetCell.Offset(i, ColOffset).NumberFormat = "[Red][<" + Format(TargetOCtest, "0.0000") + "]0%;[>=10]""Infinity"";0%"
            Else
                TargetCell.Offset(i, ColOffset).NumberFormat = "[<10]0%;[>=10]""Infinity"""
            End If
            ColOffset = ColOffset + 1
        End If
        If (PrintICtest) Then
            TargetCell.Offset(i, ColOffset).Value = Data(i, ColOffset)
            If (TargetICtest > 0) Then
                TargetCell.Offset(i, ColOffset).NumberFormat = "[Red][<" + Format(TargetICtest, "0.0000") + "]0%;[>=10]""Infinity"";0%"
            Else
                TargetCell.Offset(i, ColOffset).NumberFormat = "[<10]0%;[>=10]""Infinity"""
            End If
            ColOffset = ColOffset + 1
        End If
    Next i
    
    With TargetCell.Resize(UBound(Data, 1) + 1, ColOffset)
        With .Borders
            .LineStyle = xlContinuous
            .Weight = xlMedium
            .ColorIndex = xlAutomatic
        End With
        .Borders(xlInsideVertical).Weight = xlThin
        .Borders(xlInsideHorizontal).Weight = xlThin
    End With
    With TargetCell.Resize(1, ColOffset)
        With .Borders
            .LineStyle = xlContinuous
            .Weight = xlMedium
            .ColorIndex = xlAutomatic
        End With
        .Borders(xlInsideVertical).Weight = xlThin
        .Font.Bold = True
        .HorizontalAlignment = xlCenter
    End With
    With TargetCell.Offset(-1, TempOff).Resize(UBound(Data, 1) + 2, ColOffset - TempOff)
        .Interior.Color = RGB( _
                CLng(Split(BackgrndColor, ",")(0)) _
                , CLng(Split(BackgrndColor, ",")(1)) _
                , CLng(Split(BackgrndColor, ",")(2)) _
            )
    End With
End Sub
Public Sub PrintMortgagesRepLines( _
    RepLines As Variant, _
    DestinatAddress As String, _
    Optional PrintCounter As Boolean = False, _
    Optional PrintDates As Boolean = True, _
    Optional PrintOutstanding As Boolean = True, _
    Optional PrintInterest As Boolean = True, _
    Optional PrintScheduled As Boolean = True, _
    Optional PrintPrepay As Boolean = True, _
    Optional PrintAccruedInterest As Boolean = True, _
    Optional PrintTotalPrincipal As Boolean = False, _
    Optional PrintLoss As Boolean = True, _
    Optional PrintLossOnInterest As Boolean = True _
)
    Application.ScreenUpdating = False
    Dim TargetCell As Range
    Dim DestinationAddress As String
    DestinationAddress = Right(DestinatAddress, Len(DestinatAddress) - InStr(1, DestinatAddress, "!"))
    Set TargetCell = Sheets(Left(DestinatAddress, InStr(1, DestinatAddress, "!") - 1)).Cells( _
            CLng(Left(DestinationAddress, InStr(1, DestinationAddress, ",") - 1)), _
            CLng(Right(DestinationAddress, Len(DestinationAddress) - InStr(1, DestinationAddress, ","))) _
        )
    If (TargetCell.Count > 1) Then Set TargetCell = TargetCell.Cells(1, 1)
    Dim ColOffset As Long, i As Long
    ColOffset = 0
    If (PrintCounter) Then
        TargetCell.Offset(0, ColOffset).Value = "Num"
        TargetCell.Offset(0, ColOffset).EntireColumn.Width = 8
        ColOffset = ColOffset + 1
    End If
    If (PrintDates) Then
        TargetCell.Offset(0, ColOffset).Value = "Date"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 8
        ColOffset = ColOffset + 1
    End If
    If (PrintOutstanding) Then
        TargetCell.Offset(0, ColOffset).Value = "Outstanding"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintInterest) Then
        TargetCell.Offset(0, ColOffset).Value = "Interest"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintScheduled) Then
        If (PrintTotalPrincipal) Then
            TargetCell.Offset(0, ColOffset).Value = "Scheduled Principal"
        Else
            TargetCell.Offset(0, ColOffset).Value = "Principal"
        End If
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintPrepay) Then
        TargetCell.Offset(0, ColOffset).Value = "Prepayments"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintAccruedInterest) Then
        TargetCell.Offset(0, ColOffset).Value = "Accrued Interest"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintTotalPrincipal) Then
        TargetCell.Offset(0, ColOffset).Value = "Total Principal"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintLoss) Then
        TargetCell.Offset(0, ColOffset).Value = "Principal Loss"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    If (PrintLossOnInterest) Then
        TargetCell.Offset(0, ColOffset).Value = "Interest Loss"
        TargetCell.Offset(0, ColOffset).EntireColumn.ColumnWidth = 12
        ColOffset = ColOffset + 1
    End If
    
    
    For i = 1 To UBound(RepLines, 1)
        ColOffset = 0
        If (PrintCounter) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "0"
            ColOffset = ColOffset + 1
        End If
        If (PrintDates) Then
            TargetCell.Offset(i, ColOffset).Value = DateValue(RepLines(i, ColOffset))
            TargetCell.Offset(i, ColOffset).NumberFormat = "mmm-yy"
            ColOffset = ColOffset + 1
        End If
        If (PrintOutstanding) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintInterest) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintScheduled) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintPrepay) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintAccruedInterest) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintTotalPrincipal) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintLoss) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
        If (PrintLossOnInterest) Then
            TargetCell.Offset(i, ColOffset).Value = RepLines(i, ColOffset)
            TargetCell.Offset(i, ColOffset).NumberFormat = "_-* #,##0_-;-* #,##0_-;_-* "" - ""??_-;_-@_-"
            ColOffset = ColOffset + 1
        End If
    Next i
    With TargetCell.Resize(UBound(RepLines, 1) + 1, ColOffset)
        With .Borders
            .LineStyle = xlContinuous
            .Weight = xlMedium
            .ColorIndex = xlAutomatic
        End With
        .Borders(xlInsideVertical).Weight = xlThin
        .Borders(xlInsideHorizontal).Weight = xlThin
    End With
    With TargetCell.Resize(1, ColOffset)
        With .Borders
            .LineStyle = xlContinuous
            .Weight = xlMedium
            .ColorIndex = xlAutomatic
        End With
        .Borders(xlInsideVertical).Weight = xlThin
        .Interior.Color = RGB(118, 147, 60)
        .Font.Bold = True
        .HorizontalAlignment = xlCenter
    End With
End Sub

  Public Sub PlotMortgagesFlows( _
    TargetPlotSheet As String, _
    TargetPlotIndex As Long, _
    Dtes As Variant, _
    Outstanding As Variant, _
    Interest As Variant, _
    Scheduled As Variant, _
    Prepay As Variant, _
    TotalPrinc As Variant, _
    LossOnPrinc As Variant _
)
    Dim Dates() As Date
    Dim TargetPlot As Chart
    Dim i As Long
    Set TargetPlot = Sheets(TargetPlotSheet).ChartObjects(TargetPlotIndex).Chart
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
    ReDim Dates(LBound(Dtes) To UBound(Dtes))
    For i = LBound(Dtes) To UBound(Dtes)
        Dates(i) = DateValue(Dtes(i))
    Next i
    With TargetPlot
        .ChartType = xlColumnStacked
        .HasLegend = True
        .Legend.Position = xlLegendPositionTop
        .HasTitle = True
        .Axes(xlValue).HasTitle = True
        .Axes(xlCategory).HasTitle = False
        .Axes(xlCategory).TickLabels.NumberFormat = "mmm-yy"
        .ChartTitle.Text = "Loans Flows"
        .Axes(xlValue).AxisTitle.Caption = "Flows"
        .Axes(xlValue).DisplayUnit = xlMillions
    End With
    If (CountEmpty(LossOnPrinc) < UBound(LossOnPrinc) - LBound(LossOnPrinc) + 1) Then
        With TargetPlot.SeriesCollection.NewSeries
            .Name = "Loss"
            .Values = LossOnPrinc
            .XValues = Dates
        End With
    End If
    If (CountEmpty(TotalPrinc) < UBound(TotalPrinc) - LBound(TotalPrinc) + 1) Then
        With TargetPlot.SeriesCollection.NewSeries
            If (CountEmpty(Scheduled) < UBound(Scheduled) - LBound(Scheduled) + 1) Then
                .Name = "Total Principal"
                .ChartType = xlColumnClustered
            Else
                .Name = "Principal"
            End If
            .Values = TotalPrinc
            .XValues = Dates
        End With
    End If
    If (CountEmpty(Prepay) < UBound(Prepay) - LBound(Prepay) + 1) Then
        With TargetPlot.SeriesCollection.NewSeries
            .Name = "Prepayments"
            .Values = Prepay
            .XValues = Dates
        End With
    End If
    If (CountEmpty(Scheduled) < UBound(Scheduled) - LBound(Scheduled) + 1) Then
        With TargetPlot.SeriesCollection.NewSeries
            If (CountEmpty(TotalPrinc) < UBound(TotalPrinc) - LBound(TotalPrinc) + 1) Then
                .Name = "Scheduled Principal"
            Else
                .Name = "Principal"
            End If
            .Values = Scheduled
            .XValues = Dates
        End With
    End If
    If (CountEmpty(Interest) < UBound(Interest) - LBound(Interest) + 1) Then
        With TargetPlot.SeriesCollection.NewSeries
            .Name = "Interest"
            .Values = Interest
            .XValues = Dates
        End With
    End If
    If (CountEmpty(Outstanding) < UBound(Outstanding) - LBound(Outstanding) + 1) Then
        With TargetPlot.SeriesCollection.NewSeries
            .Name = "Outstanding"
            .Values = Outstanding
            .XValues = Dates
            .ChartType = xlArea
            .AxisGroup = xlSecondary
            With .Format.Fill
                .Visible = msoTrue
                .PresetGradient msoGradientHorizontal, 1, msoGradientParchment
            End With
        End With
    End If
    With TargetPlot
        If (CountEmpty(Outstanding) < UBound(Outstanding) - LBound(Outstanding) + 1) Then
            .Axes(xlValue, xlSecondary).HasTitle = True
            .Axes(xlValue, xlSecondary).AxisTitle.Caption = "Outstanding"
            .Axes(xlValue, xlSecondary).DisplayUnit = xlMillions
        End If
    End With
End Sub
Public Sub PrintStressTest( _
    StressValues As Variant, _
    DestinatAddress As String, _
    SetupConditionalFormatting As Boolean _
)
    Application.ScreenUpdating = False
    Dim TargetCell As Range
    Dim DestinationAddress As String
    DestinationAddress = Right(DestinatAddress, Len(DestinatAddress) - InStr(1, DestinatAddress, "!"))
    Set TargetCell = Sheets(Left(DestinatAddress, InStr(1, DestinatAddress, "!") - 1)).Cells( _
            CLng(Left(DestinationAddress, InStr(1, DestinationAddress, ",") - 1)), _
            CLng(Right(DestinationAddress, Len(DestinationAddress) - InStr(1, DestinationAddress, ","))) _
        )
    If (TargetCell.Count > 1) Then Set TargetCell = TargetCell.Cells(1, 1)
    Dim i As Long, j As Long
    For i = LBound(StressValues, 1) To UBound(StressValues, 1)
        For j = LBound(StressValues, 2) To UBound(StressValues, 2)
            With TargetCell.Offset(i - LBound(StressValues, 1), j - LBound(StressValues, 2))
                .Value = StressValues(i, j)
                .NumberFormat = "0.00%"
            End With
        Next j
    Next i
    Set TargetCell = TargetCell.Resize(UBound(StressValues, 1) - LBound(StressValues, 1) + 1, UBound(StressValues, 2) - LBound(StressValues, 2) + 1)
    With TargetCell
        With .Borders
            .LineStyle = xlContinuous
            .Weight = xlMedium
            .ColorIndex = xlAutomatic
        End With
        .Borders(xlInsideVertical).Weight = xlThin
        .Borders(xlInsideHorizontal).Weight = xlThin
        If (SetupConditionalFormatting) Then
            .FormatConditions.Delete
            .Interior.Color = RGB(146, 208, 80)
            .FormatConditions.Add(xlCellValue, xlGreaterEqual, "1").Interior.Color = RGB(255, 0, 0)
            .FormatConditions.Add(xlCellValue, xlGreaterEqual, "0.0001").Interior.Color = RGB(255, 192, 0)
        End If
    End With
End Sub
Public Sub PrintMergedCell(msg As String, DestinatAddress As String, RowSpann As Long, ColSpann As Long, FillColor As String)
    Application.ScreenUpdating = False
    Dim TargetCell As Range
    Dim DestinationAddress As String
    DestinationAddress = Right(DestinatAddress, Len(DestinatAddress) - InStr(1, DestinatAddress, "!"))
    Set TargetCell = Sheets(Left(DestinatAddress, InStr(1, DestinatAddress, "!") - 1)).Cells( _
            CLng(Left(DestinationAddress, InStr(1, DestinationAddress, ",") - 1)), _
            CLng(Right(DestinationAddress, Len(DestinationAddress) - InStr(1, DestinationAddress, ","))) _
        )
    If (TargetCell.Count > 1) Then Set TargetCell = TargetCell.Cells(1, 1)
    TargetCell.Value = msg
    With TargetCell.Resize(RowSpann, ColSpann)
        .Merge
        With .Borders
            .LineStyle = xlContinuous
            .Weight = xlMedium
            .ColorIndex = xlAutomatic
        End With
        .Font.Bold = True
        .Interior.Color = RGB( _
                CLng(Split(FillColor, ",")(0)) _
                , CLng(Split(FillColor, ",")(1)) _
                , CLng(Split(FillColor, ",")(2)) _
            )
    End With
End Sub
Public Sub PrintColumn(Title As String, Values As Variant, DestinatAddress As String, NuFormat As String, FillColor As String)
    Application.ScreenUpdating = False
    Dim i As Long
    Dim TargetCell As Range
    Dim DestinationAddress As String
    DestinationAddress = Right(DestinatAddress, Len(DestinatAddress) - InStr(1, DestinatAddress, "!"))
    Set TargetCell = Sheets(Left(DestinatAddress, InStr(1, DestinatAddress, "!") - 1)).Cells( _
            CLng(Left(DestinationAddress, InStr(1, DestinationAddress, ",") - 1)), _
            CLng(Right(DestinationAddress, Len(DestinationAddress) - InStr(1, DestinationAddress, ","))) _
        )
    If (TargetCell.Count > 1) Then Set TargetCell = TargetCell.Cells(1, 1)
    TargetCell.Value = Title
    For i = LBound(Values) To UBound(Values)
        TargetCell.Offset(1 + i - LBound(Values), 0).Value = Values(i)
        TargetCell.Offset(1 + i - LBound(Values), 0).NumberFormat = NuFormat
    Next i
    With TargetCell.Resize(UBound(Values) - LBound(Values) + 2, 1)
        With .Borders
            .LineStyle = xlContinuous
            .Weight = xlMedium
            .ColorIndex = xlAutomatic
        End With
        .Borders(xlInsideVertical).Weight = xlThin
        .Borders(xlInsideHorizontal).Weight = xlThin
        .Interior.Color = RGB( _
                CLng(Split(FillColor, ",")(0)) _
                , CLng(Split(FillColor, ",")(1)) _
                , CLng(Split(FillColor, ",")(2)) _
            )
    End With
     With TargetCell
        With .Borders
            .LineStyle = xlContinuous
            .Weight = xlMedium
            .ColorIndex = xlAutomatic
        End With
        .Font.Bold = True
        .HorizontalAlignment = xlCenter
    End With
End Sub
Public Sub PrintDataColumn(Values As Variant, DestinatAddress As String, NuFormat As String)
    Application.ScreenUpdating = False
    Dim i As Long
    Dim TargetCell As Range
    Dim DestinationAddress As String
    DestinationAddress = Right(DestinatAddress, Len(DestinatAddress) - InStr(1, DestinatAddress, "!"))
    Set TargetCell = Sheets(Left(DestinatAddress, InStr(1, DestinatAddress, "!") - 1)).Cells( _
            CLng(Left(DestinationAddress, InStr(1, DestinationAddress, ",") - 1)), _
            CLng(Right(DestinationAddress, Len(DestinationAddress) - InStr(1, DestinationAddress, ","))) _
        )
    If (TargetCell.Count > 1) Then Set TargetCell = TargetCell.Cells(1, 1)
    For i = LBound(Values) To UBound(Values)
        TargetCell.Offset(i - LBound(Values), 0).Value = Values(i)
        TargetCell.Offset(i - LBound(Values), 0).NumberFormat = NuFormat
    Next i
End Sub
Public Sub PlotTranchesDynamic( _
    TargetPlotSheet As String, _
    TargetPlotIndex As Long, _
    Dtes As Variant, _
    Outstanding As Variant, _
    Titles As Variant, _
    CallDateString As String _
)
    Dim Dates() As Date
    Dim CallDate As Date
    Dim TempValues() As Double
    Dim TargetPlot As Chart
    Dim i As Long, j As Long
    If (Len(CallDateString) > 0) Then CallDate = DateValue(CallDateString)
    Set TargetPlot = Sheets(TargetPlotSheet).ChartObjects(TargetPlotIndex).Chart
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
     With TargetPlot
        .HasLegend = True
        .Legend.Position = xlLegendPositionRight
        .HasTitle = True
        .Axes(xlValue).HasTitle = False
        .Axes(xlCategory).HasTitle = False
        .Axes(xlCategory).TickLabels.NumberFormat = "mmm-yy"
        .ChartType = xlAreaStacked
        .ChartTitle.Text = "Notes Outstanding"
        With .Axes(xlValue)
            .DisplayUnit = xlMillions
            .TickLabels.NumberFormat = "0"
            .HasDisplayUnitLabel = True
            .MinimumScaleIsAuto = True
            .MaximumScaleIsAuto = True
        End With
    End With
    ReDim Dates(LBound(Dtes) To UBound(Dtes))
    ReDim TempValues(LBound(Outstanding, 1) To UBound(Outstanding, 1))
    For i = LBound(Dtes) To UBound(Dtes)
        Dates(i) = DateValue(Dtes(i))
    Next i
    For i = UBound(Outstanding, 2) To LBound(Outstanding, 2) Step -1
        For j = LBound(Outstanding, 1) To UBound(Outstanding, 1)
            TempValues(j) = Outstanding(j, i)
        Next j
        With TargetPlot.SeriesCollection.NewSeries
            .Name = Titles(i)
            .Values = TempValues
            .XValues = Dates
        End With
    Next i
    If (Len(CallDateString) > 0) Then
        Dim CallDateFound As Boolean
        CallDateFound = False
        For j = LBound(Dates) To UBound(Dates)
            If (Dates(j) >= CallDate And (Not CallDateFound)) Then
                TempValues(j) = 1
                CallDateFound = True
            Else
                TempValues(j) = 0
            End If
        Next j
        With TargetPlot.SeriesCollection.NewSeries
            .ChartType = xlColumnClustered
            .Name = "Call Date"
            .Values = TempValues
            .XValues = Dates
            .AxisGroup = xlSecondary
            .Format.Fill.Patterned msoPatternDarkHorizontal
            .Format.Fill.ForeColor.RGB = RGB(90, 90, 90)
        End With
        TargetPlot.ChartGroups(TargetPlot.ChartGroups.Count).GapWidth = 500
        With TargetPlot.Axes(xlValue, xlSecondary)
            .MaximumScale = 1
            .MinimumScale = 0
            .HasTitle = False
            .DisplayUnit = xlNone
            .HasDisplayUnitLabel = False
            .Border.Weight = xlHairline
            .Border.LineStyle = xlNone
            .MajorTickMark = xlNone
            .MinorTickMark = xlNone
            .TickLabelPosition = xlNone
        End With
    End If
End Sub
Public Sub PlotOCTest( _
    TargetPlotSheet As String, _
    TargetPlotIndex As Long, _
    Dtes As Variant, _
    OCValues As Variant, _
    Titles As Variant, _
    CallDateString As String _
)
    Dim Dates() As Date
    Dim CallDate As Date
    Dim TempValues() As Double
    Dim TargetPlot As Chart
    Dim i As Long, j As Long
    If (Len(CallDateString) > 0) Then CallDate = DateValue(CallDateString)
    Set TargetPlot = Sheets(TargetPlotSheet).ChartObjects(TargetPlotIndex).Chart
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
    With TargetPlot
        .HasLegend = True
        .Legend.Position = xlLegendPositionRight
        .HasTitle = True
        .Axes (xlValue)
        .Axes(xlCategory).HasTitle = False
        .Axes(xlCategory).TickLabels.NumberFormat = "mmm-yy"
        .ChartType = xlLine
        .ChartTitle.Text = "OC Test"
        With .Axes(xlValue)
            .HasTitle = False
            .HasDisplayUnitLabel = False
            .DisplayUnit = xlNone
            .MinimumScaleIsAuto = True
            .MaximumScaleIsAuto = True
            .TickLabels.NumberFormat = "0%"
            .MaximumScale = 2
            .MinimumScale = 0
        End With
    End With
    ReDim Dates(LBound(Dtes) To UBound(Dtes))
    ReDim TempValues(LBound(OCValues, 1) To UBound(OCValues, 1))
    For i = LBound(Dtes) To UBound(Dtes)
        Dates(i) = DateValue(Dtes(i))
    Next i
    For i = UBound(OCValues, 2) To LBound(OCValues, 2) Step -1
        For j = LBound(OCValues, 1) To UBound(OCValues, 1)
            TempValues(j) = OCValues(j, i)
        Next j
        With TargetPlot.SeriesCollection.NewSeries
            .Name = Titles(i)
            .Values = TempValues
            .XValues = Dates
        End With
    Next i
    If (Len(CallDateString) > 0) Then
        Dim CallDateFound As Boolean
        CallDateFound = False
        For j = LBound(Dates) To UBound(Dates)
            If (Dates(j) >= CallDate And (Not CallDateFound)) Then
                TempValues(j) = 1
                CallDateFound = True
            Else
                TempValues(j) = 0
            End If
        Next j
        With TargetPlot.SeriesCollection.NewSeries
            .ChartType = xlColumnClustered
            .Name = "Call Date"
            .Values = TempValues
            .XValues = Dates
            .AxisGroup = xlSecondary
            .Format.Fill.Patterned msoPatternDarkHorizontal
            .Format.Fill.ForeColor.RGB = RGB(90, 90, 90)
        End With
        TargetPlot.ChartGroups(TargetPlot.ChartGroups.Count).GapWidth = 500
        With TargetPlot.Axes(xlValue, xlSecondary)
            .MaximumScale = 1
            .MinimumScale = 0
            .HasTitle = False
            .DisplayUnit = xlNone
            .HasDisplayUnitLabel = False
            .Border.Weight = xlHairline
            .Border.LineStyle = xlNone
            .MajorTickMark = xlNone
            .MinorTickMark = xlNone
            .TickLabelPosition = xlNone
        End With
    End If
End Sub
Public Sub PlotICTest( _
    TargetPlotSheet As String, _
    TargetPlotIndex As Long, _
    Dtes As Variant, _
    OCValues As Variant, _
    Titles As Variant, _
    CallDateString As String _
)
    Dim Dates() As Date
    Dim CallDate As Date
    Dim TempValues() As Double
    Dim TargetPlot As Chart
    Dim i As Long, j As Long
    If (Len(CallDateString) > 0) Then CallDate = DateValue(CallDateString)
    Set TargetPlot = Sheets(TargetPlotSheet).ChartObjects(TargetPlotIndex).Chart
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
    With TargetPlot
        .HasLegend = True
        .Legend.Position = xlLegendPositionRight
        .HasTitle = True
        .Axes (xlValue)
        .Axes(xlCategory).HasTitle = False
        .Axes(xlCategory).TickLabels.NumberFormat = "mmm-yy"
        .ChartType = xlLine
        .ChartTitle.Text = "IC Test"
        With .Axes(xlValue)
            .HasTitle = False
            .HasDisplayUnitLabel = False
            .DisplayUnit = xlNone
            .MinimumScaleIsAuto = True
            .MaximumScaleIsAuto = True
            .TickLabels.NumberFormat = "0%"
            .MaximumScale = 10
            .MinimumScale = 0
        End With
    End With
    ReDim Dates(LBound(Dtes) To UBound(Dtes))
    ReDim TempValues(LBound(OCValues, 1) To UBound(OCValues, 1))
    For i = LBound(Dtes) To UBound(Dtes)
        Dates(i) = DateValue(Dtes(i))
    Next i
    For i = UBound(OCValues, 2) To LBound(OCValues, 2) Step -1
        For j = LBound(OCValues, 1) To UBound(OCValues, 1)
            TempValues(j) = OCValues(j, i)
        Next j
        With TargetPlot.SeriesCollection.NewSeries
            .Name = Titles(i)
            .Values = TempValues
            .XValues = Dates
        End With
    Next i
    If (Len(CallDateString) > 0) Then
        Dim CallDateFound As Boolean
        CallDateFound = False
        For j = LBound(Dates) To UBound(Dates)
            If (Dates(j) >= CallDate And (Not CallDateFound)) Then
                TempValues(j) = 1
                CallDateFound = True
            Else
                TempValues(j) = 0
            End If
        Next j
        With TargetPlot.SeriesCollection.NewSeries
            .ChartType = xlColumnClustered
            .Name = "Call Date"
            .Values = TempValues
            .XValues = Dates
            .AxisGroup = xlSecondary
            .Format.Fill.Patterned msoPatternDarkHorizontal
            .Format.Fill.ForeColor.RGB = RGB(90, 90, 90)
        End With
        TargetPlot.ChartGroups(TargetPlot.ChartGroups.Count).GapWidth = 500
        With TargetPlot.Axes(xlValue, xlSecondary)
            .MaximumScale = 1
            .MinimumScale = 0
            .HasTitle = False
            .DisplayUnit = xlNone
            .HasDisplayUnitLabel = False
            .Border.Weight = xlHairline
            .Border.LineStyle = xlNone
            .MajorTickMark = xlNone
            .MinorTickMark = xlNone
            .TickLabelPosition = xlNone
        End With
    End If
End Sub
Public Sub PlotAnnualExcess( _
    TargetPlotSheet As String, _
    TargetPlotIndex As Long, _
    Dtes As Variant, _
    Values As Variant, _
    CallDateString As String _
)
    Dim Dates() As Date
    Dim CallDate As Date
    Dim TempValues() As Double
    Dim TargetPlot As Chart
    Dim i As Long
    If (Len(CallDateString) > 0) Then CallDate = DateValue(CallDateString)
    Set TargetPlot = Sheets(TargetPlotSheet).ChartObjects(TargetPlotIndex).Chart
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
     ReDim Dates(LBound(Dtes) To UBound(Dtes))
    ReDim TempValues(LBound(Values) To UBound(Values))
    For i = LBound(Dtes) To UBound(Dtes)
        Dates(i) = DateValue(Dtes(i))
    Next i
    With TargetPlot
        .HasLegend = False
        .HasTitle = True
        .Axes(xlValue).HasTitle = False
        .Axes(xlCategory).HasTitle = False
        .Axes(xlCategory).TickLabels.NumberFormat = "mmm-yy"
        .ChartType = xlLine
        .ChartTitle.Text = "Excess Spread"
        With .Axes(xlValue)
            .HasDisplayUnitLabel = False
            .DisplayUnit = xlNone
            .MinimumScaleIsAuto = True
            .MaximumScaleIsAuto = True
            .TickLabels.NumberFormat = "0.00%"
        End With
    End With
    
    With TargetPlot.SeriesCollection.NewSeries
        .Name = "Annualized Excess Spread"
        .Values = Values
        .XValues = Dates
    End With
    If (Len(CallDateString) > 0) Then
        Dim CallDateFound As Boolean
        CallDateFound = False
        For i = LBound(Dates) To UBound(Dates)
            If (Dates(i) >= CallDate And (Not CallDateFound)) Then
                TempValues(i) = 1
                CallDateFound = True
            Else
                TempValues(i) = 0
            End If
        Next i
        With TargetPlot.SeriesCollection.NewSeries
            .ChartType = xlColumnClustered
            .Name = "Call Date"
            .Values = TempValues
            .XValues = Dates
            .AxisGroup = xlSecondary
            .Format.Fill.Patterned msoPatternDarkHorizontal
            .Format.Fill.ForeColor.RGB = RGB(90, 90, 90)
        End With
        TargetPlot.ChartGroups(TargetPlot.ChartGroups.Count).GapWidth = 500
        With TargetPlot.Axes(xlValue, xlSecondary)
            .MaximumScale = 1
            .MinimumScale = 0
            .HasTitle = False
            .DisplayUnit = xlNone
            .HasDisplayUnitLabel = False
            .Border.Weight = xlHairline
            .Border.LineStyle = xlNone
            .MajorTickMark = xlNone
            .MinorTickMark = xlNone
            .TickLabelPosition = xlNone
        End With
    End If
End Sub

Public Sub PlotCostFunding( _
    TargetPlotSheet As String, _
    TargetPlotIndex As Long, _
    Dtes As Variant, _
    Values As Variant, _
    CallDateString As String _
)
    Dim Dates() As Date
    Dim CallDate As Date
    Dim TempValues() As Double
    Dim TargetPlot As Chart
    Dim i As Long
    If (Len(CallDateString) > 0) Then CallDate = DateValue(CallDateString)
    Set TargetPlot = Sheets(TargetPlotSheet).ChartObjects(TargetPlotIndex).Chart
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
     ReDim Dates(LBound(Dtes) To UBound(Dtes))
    ReDim TempValues(LBound(Values) To UBound(Values))
    For i = LBound(Dtes) To UBound(Dtes)
        Dates(i) = DateValue(Dtes(i))
    Next i
    With TargetPlot
        .HasLegend = False
        .HasTitle = True
        .Axes(xlValue).HasTitle = False
        .Axes(xlCategory).HasTitle = False
        .Axes(xlCategory).TickLabels.NumberFormat = "mmm-yy"
        .ChartType = xlLine
        .ChartTitle.Text = "WA Cost Of Funding"
        With .Axes(xlValue)
            .HasDisplayUnitLabel = False
            .DisplayUnit = xlNone
            .MinimumScaleIsAuto = True
            .MaximumScaleIsAuto = True
            .TickLabels.NumberFormat = "0.00%"
        End With
    End With
    
    With TargetPlot.SeriesCollection.NewSeries
        .Name = "WA Cost Of Funding"
        .Values = Values
        .XValues = Dates
    End With
    If (Len(CallDateString) > 0) Then
        Dim CallDateFound As Boolean
        CallDateFound = False
        For i = LBound(Dates) To UBound(Dates)
            If (Dates(i) >= CallDate And (Not CallDateFound)) Then
                TempValues(i) = 1
                CallDateFound = True
            Else
                TempValues(i) = 0
            End If
        Next i
        With TargetPlot.SeriesCollection.NewSeries
            .ChartType = xlColumnClustered
            .Name = "Call Date"
            .Values = TempValues
            .XValues = Dates
            .AxisGroup = xlSecondary
            .Format.Fill.Patterned msoPatternDarkHorizontal
            .Format.Fill.ForeColor.RGB = RGB(90, 90, 90)
        End With
        TargetPlot.ChartGroups(TargetPlot.ChartGroups.Count).GapWidth = 500
        With TargetPlot.Axes(xlValue, xlSecondary)
            .MaximumScale = 1
            .MinimumScale = 0
            .HasTitle = False
            .DisplayUnit = xlNone
            .HasDisplayUnitLabel = False
            .Border.Weight = xlHairline
            .Border.LineStyle = xlNone
            .MajorTickMark = xlNone
            .MinorTickMark = xlNone
            .TickLabelPosition = xlNone
        End With
    End If
End Sub

Public Sub PlotStressMargin( _
    TargetPlotSheet As String, _
    TargetPlotIndex As Long, _
    Values As Variant, _
    Xvar As Long, _
    Yvar As Long, _
    XHeader As Variant, _
    YHeader As Variant _
)
    Dim TargetPlot As Chart
    Dim i As Long, j As Long
    Set TargetPlot = Sheets(TargetPlotSheet).ChartObjects(TargetPlotIndex).Chart
    While (TargetPlot.SeriesCollection.Count > 0)
        TargetPlot.SeriesCollection(1).Delete
    Wend
    Dim TempValue() As Double
    ReDim TempValue(1 To UBound(Values, 1))
    ReDim TempXVal(1 To UBound(Values, 1))
    For j = 1 To UBound(Values, 1)
        TempXVal(j) = XHeader(j)
    Next j
    For i = 1 To UBound(Values, 2)
        For j = 1 To UBound(Values, 1)
            TempValue(j) = Values(j, i)
        Next j
        With TargetPlot.SeriesCollection.NewSeries
            .Name = YHeader(i)
            .Values = TempValue
            .XValues = TempXVal
        End With
    Next i
    With TargetPlot
        .ChartType = xlSurface
        .HasLegend = False
        .HasTitle = True
        .Axes(xlValue).HasTitle = True
        .Axes(xlCategory).HasTitle = True
        .Axes(xlTimeScale).HasTitle = True
        .ChartTitle.Text = "Discount Margin Sensitivity"
        .Axes(xlValue).AxisTitle.Caption = "Discount Margin"
        .Axes(xlCategory).ReversePlotOrder = False
        .Axes(xlTimeScale).ReversePlotOrder = True
    End With
End Sub
