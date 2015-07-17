TEMPLATE = app
TARGET = StandaloneStress
CONFIG(debug, debug|release) {
    DESTDIR = ../bin/Debug
    MOC_DIR += ./GeneratedFiles/debug
    OBJECTS_DIR += debug
}
CONFIG(release, debug|release) {
    DESTDIR = ../bin/Release
    MOC_DIR += ./GeneratedFiles/release
    OBJECTS_DIR += release
}
QT += core widgets gui
DEFINES += QT_DLL QT_WIDGETS_LIB
INCLUDEPATH += "../CLOModelLib" \
    ./GeneratedFiles \
    . \
    "$$MOC_DIR"
LIBS += -L"$$DESTDIR"
CONFIG(release, debug|release) {
    LIBS += -lCLOModelLib
}
CONFIG(debug, debug|release) {
    LIBS += -lCLOModelLibd
}
DEPENDPATH += .
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(StandaloneStress.pri)
win32:RC_FILE = StandaloneStress.rc
