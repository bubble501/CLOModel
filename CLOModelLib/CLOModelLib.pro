TEMPLATE = lib
TARGET = CLOModelLib
DESTDIR = ../Win32/Release
QT += core sql widgets gui
CONFIG += release
DEFINES += WIN64 QT_DLL QT_SQL_LIB QT_WIDGETS_LIB CLOMODELLIB_LIB
INCLUDEPATH += "$$(QBBGLIBPATH)/include" \
    "$$(SIMSTRINGPATH)/include/simstring" \
    "$$(QUAZIPPATH)/include" \
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
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(CLOModelLib.pri)
headers.path=$$PREFIX/include
headers.files= *.h
unix:!symbian {
    isEmpty(PREFIX){
		PREFIX=/usr/local
	}
	target.path=$$PREFIX/lib/$${LIB_ARCH}
}
win32 {
    isEmpty(PREFIX){
		PREFIX=C:\CLOModelLib
	}
	target.path=$$PREFIX/lib/$${LIB_ARCH}
}
INSTALLS += headers target

