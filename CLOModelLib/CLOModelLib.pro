TEMPLATE = lib
TARGET = CLOModelLib
DESTDIR = ../Win32/Release
QT += core sql widgets gui
CONFIG += release
DEFINES += WIN64 QT_DLL QT_SQL_LIB QT_WIDGETS_LIB CLOMODELLIB_LIB
INCLUDEPATH += $(QBBGLIBPATH)/QBbgLib \
    $(SIMSTRINGPATH)/include/simstring \
    $(QUAZIPPATH)/include \
    $(ZLIBPATH)/include \
    $(BOOSTPATH) \
    $(KDCHARTPATH)/include/KDChart \
    ./GeneratedFiles \
    . \
    ./GeneratedFiles/Release \
    ./Private
LIBS += -L"$(QBBGLIBPATH)/Win32/Release" \
    -L"$(KDCHARTPATH)/lib" \
    -L"$(QUAZIPPATH)/lib" \
    -lQBbgLib \
    -lQuaZip \
    -lkdchart2
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(CLOModelLib.pri)
