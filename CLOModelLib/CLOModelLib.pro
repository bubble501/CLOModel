TEMPLATE = lib
CONFIG(debug, debug|release) {
    win32: TARGETNAME=CLOModelLibd
	mac: TARGETNAME=CLOModelLib_debug
    DESTDIRNAME = ../bin/Debug
    MOC_DIR += ./GeneratedFiles/debug
    OBJECTS_DIR += debug
}
CONFIG(release, debug|release) {
    TARGETNAME = CLOModelLib
    DESTDIRNAME = ../bin/Release
    MOC_DIR += ./GeneratedFiles/release
    OBJECTS_DIR += release
}
TARGET = $$TARGETNAME
DESTDIR = $$DESTDIRNAME
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
    ./GeneratedFiles/Release \
    ./Private
LIBS += -L"$$(QBBGLIBPATH)/lib" \
    -L"$$(KDCHARTPATH)/lib" \
    -L"$$(QUAZIPPATH)/lib" \
    -lQBbgLib \
    -lQuaZip \
    -lkdchart2
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

