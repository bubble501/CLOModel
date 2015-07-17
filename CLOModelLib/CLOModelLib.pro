TEMPLATE = lib
CONFIG(debug, debug|release) {
    TARGET=CLOModelLibd
    DESTDIR = ../bin/Debug
    MOC_DIR += ./GeneratedFiles/debug
    OBJECTS_DIR += debug
}
CONFIG(release, debug|release) {
    TARGET = CLOModelLib
    DESTDIR = ../bin/Release
    MOC_DIR += ./GeneratedFiles/release
    OBJECTS_DIR += release
}
QT += core sql widgets gui
DEFINES += QT_DLL QT_SQL_LIB QT_WIDGETS_LIB CLOMODELLIB_LIB
INCLUDEPATH += "$$(QBBGLIBPATH)/include" \
    "$$(SIMSTRINGPATH)/include/simstring" \
    "$$(QUAZIPPATH)/include/quazip" \
    "$$(ZLIBPATH)/include" \
    "$$(BOOSTPATH)" \
    "$$(KDCHARTPATH)/include/KDChart" \
    ./GeneratedFiles \
    . \
    "$$MOC_DIR" \
    ./Private
LIBS += -L"$$(QBBGLIBPATH)/lib" \
    -L"$$(KDCHARTPATH)/lib" \
    -L"$$(QUAZIPPATH)/lib" 
CONFIG(release, debug|release) {
LIBS += \
    -lQBbgLib \
    -lQuaZip \
    -lkdchart2
}
CONFIG(debug, debug|release) {
LIBS += \
    -lQBbgLibd \
    -lQuaZipd \
    -lkdchartd2
}
DEPENDPATH += .
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(CLOModelLib.pri)
headers.path=$$PREFIX/include
headers.files= *.h
isEmpty(PREFIX){
	PREFIX=..\CLOModelLib
	warning("Prefix not set")
}
target.path=$$PREFIX/lib
unix:!symbian {
	target.path=$$PREFIX/lib/$${LIB_ARCH}
}
INSTALLS += headers target

