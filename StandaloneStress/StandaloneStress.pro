TEMPLATE = app
TARGET = StandaloneStress
DESTDIR = ../Win32/Release
QT += core widgets gui
CONFIG += release
DEFINES += WIN64 QT_DLL QT_WIDGETS_LIB
INCLUDEPATH += "../CLOModelLib" \
    ./GeneratedFiles \
    . \
    ./GeneratedFiles/Release
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(StandaloneStress.pri)
win32:RC_FILE = StandaloneStress.rc
