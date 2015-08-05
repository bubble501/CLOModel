TEMPLATE = subdirs
SUBDIRS = CLOModelLib
CLOModelTest.file= CLOModelTest/CLOModelTest.pro
CLOModelTest.depends = CLOModelLib
SUBDIRS += CLOModelTest
CONFIG(release, debug|release) {
    CLOViewer.file= CLOViewer/CLOViewer.pro
    CLOViewer.depends = CLOModelLib
    SUBDIRS += CLOViewer
    StandaloneStress.file= StandaloneStress/StandaloneStress.pro
    StandaloneStress.depends = CLOModelLib
    SUBDIRS += StandaloneStress
    win32{
	    CLOModelV2.file= CLOModelV2/CLOModelExcel.pro
	    CLOModelV2.depends = CLOModelLib
        SUBDIRS += CLOModelV2
    }
}