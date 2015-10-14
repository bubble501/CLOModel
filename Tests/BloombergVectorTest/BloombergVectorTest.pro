TEMPLATE = app
TARGET = BloombergVectorTest
CONFIG(debug, debug|release) {
    DESTDIRPARENT = ../../bin/Debug
    MOC_DIR += ./GeneratedFiles/debug
    OBJECTS_DIR += debug
}
CONFIG(release, debug|release) {
    DESTDIRPARENT = ../../bin/Release
    MOC_DIR += ./GeneratedFiles/release
    OBJECTS_DIR += release
}
DESTDIR = $$DESTDIRPARENT/Tests
QT += core testlib
CONFIG += debug console testcase
DEFINES += QT_DLL QT_TESTLIB_LIB
INCLUDEPATH += ../../CLOModelLib \
    . \
    $$MOC_DIR
LIBS += -L"$$DESTDIRPARENT"
CONFIG(release, debug|release) {
    LIBS += -lCLOModelLib
}
CONFIG(debug, debug|release) {
    LIBS += -lCLOModelLibd
}
DEPENDPATH += .
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(BloombergVectorTest.pri)
