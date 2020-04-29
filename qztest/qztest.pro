TEMPLATE = app
QT -= gui
QT += network
CONFIG += qtestlib
CONFIG += console
CONFIG -= app_bundle
DEPENDPATH += .
INCLUDEPATH += .
!win32: LIBS += -lz
win32 {
    # workaround for qdatetime.h macro bug
    DEFINES += NOMINMAX
}

greaterThan(QT_MAJOR_VERSION, 4) {
    # disable all the Qt APIs deprecated before Qt 6.0.0
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
    LIBQUAZIP_VERSION_PREFIX = Qt5
} else {
    LIBQUAZIP_VERSION_PREFIX = Qt
}

CONFIG(staticlib): DEFINES += QUAZIP_STATIC

# Input
HEADERS += qztest.h \
testjlcompress.h \
testquachecksum32.h \
testquagzipfile.h \
testquaziodevice.h \
testquazipdir.h \
testquazipfile.h \
testquazip.h \
    testquazipnewinfo.h \
    testquazipfileinfo.h

SOURCES += qztest.cpp \
testjlcompress.cpp \
testquachecksum32.cpp \
testquagzipfile.cpp \
testquaziodevice.cpp \
testquazip.cpp \
testquazipdir.cpp \
testquazipfile.cpp \
    testquazipnewinfo.cpp \
    testquazipfileinfo.cpp

OBJECTS_DIR = .obj
MOC_DIR = .moc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../quazip/release/ -l$${LIBQUAZIP_VERSION_PREFIX}Quazip1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../quazip/debug/ -l$${LIBQUAZIP_VERSION_PREFIX}Quazipd1
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../quazip/ -l$${LIBQUAZIP_VERSION_PREFIX}Quazip_debug
else:unix: LIBS += -L$$OUT_PWD/../quazip/ -l$${LIBQUAZIP_VERSION_PREFIX}Quazip

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/../quazip

RESOURCES += \
    qztest.qrc
