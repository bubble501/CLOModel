TEMPLATE = subdirs
SUBDIRS += CLOModelLib/CLOModelLib.pro \
    CLOViewer/CLOViewer.pro \
    StandaloneStress/StandaloneStress.pro \
    ConsoleDebugger/ConsoleDebugger.pro \
	CLOModelInstaller/CLOModelInstaller.pro
win32{
    SUBDIRS += CLOModelV2/CLOModelExcel.pro
}
