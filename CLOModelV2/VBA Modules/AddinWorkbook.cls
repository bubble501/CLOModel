VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "ThisWorkbook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Option Explicit
Private Declare Function SetEnvironmentVariable Lib "kernel32" Alias "SetEnvironmentVariableA" (ByVal lpName As String, ByVal lpValue As String) As Long
Private Declare Function GetEnvironmentVariable Lib "kernel32" Alias "GetEnvironmentVariableA" (ByVal lpName As String, ByVal lpBuffer As String, ByVal nSize As Long) As Long
Private Sub Workbook_Open()
    SetEnvironmentVariable "QT_PLUGIN_PATH", "Z:\24AM\Analytics\Development\CLOModel2\Environment;" + GetEnvironmentVar("QT_PLUGIN_PATH")
    'SetEnvironmentVariable "QT_QPA_PLATFORM_PLUGIN_PATH", "Z:\24AM\Analytics\Development\CLOModel2\Environment;" + GetEnvironmentVar("QT_QPA_PLATFORM_PLUGIN_PATH")
    SetEnvironmentVariable "PATH", "Z:\24AM\Analytics\Development\CLOModel2\Environment;" + GetEnvironmentVar("PATH")
End Sub
Private Function GetEnvironmentVar(Name As String) As String
GetEnvironmentVar = String(32767, 0)
GetEnvironmentVariable Name, GetEnvironmentVar, Len(GetEnvironmentVar)
GetEnvironmentVar = TrimNull(GetEnvironmentVar)
End Function

Private Function TrimNull(item As String)
Dim iPos As Long
iPos = InStr(item, vbNullChar)
TrimNull = IIf(iPos > 0, Mid(item, 1, iPos - 1), item)
End Function

