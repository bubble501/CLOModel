TEMPLATE = lib
TARGET = CLOModelExcel
DESTDIR = ../Win32/Release
QT += core widgets gui
CONFIG += release
DEFINES += WIN64 QT_DLL QT_WIDGETS_LIB CLOMODELV2_LIB NOMINMAX
INCLUDEPATH += $(SolutionDir)/CLOModelLib \
    ./GeneratedFiles \
    . \
    ./GeneratedFiles/Release
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(CLOModelExcel.pri)
