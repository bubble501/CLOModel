TEMPLATE = subdirs
SUBDIRS = CLOModelLib

RatingsTest.file= Tests/RatingsTest/RatingsTest.pro
RatingsTest.depends = CLOModelLib
BloombergVectorTest.file= Tests/BloombergVectorTest/BloombergVectorTest.pro
BloombergVectorTest.depends = CLOModelLib
SUBDIRS += BloombergVectorTest RatingsTest

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