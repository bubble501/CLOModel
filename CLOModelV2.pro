TEMPLATE = subdirs
SUBDIRS += CLOModelLib/CLOModelLib.pro \
    CLOViewer/CLOViewer.pro \
    StandaloneStress/StandaloneStress.pro \
	CLOModelInstaller/CLOModelInstaller.pro
win32{
    SUBDIRS += CLOModelV2/CLOModelExcel.pro
}
