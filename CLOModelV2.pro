TEMPLATE = subdirs
SUBDIRS = CLOModelLib \
    CLOViewer \
	StandaloneStress
CLOViewer.depends = CLOModelLib
StandaloneStress.depends = CLOModelLib
win32{
    SUBDIRS += CLOModelV2
	CLOModelV2.file= CLOModelV2\CLOModelExcel.pro
	CLOModelV2.depends = CLOModelLib
}
