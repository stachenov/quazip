TEMPLATE = app
QT -= gui
CONFIG += qtestlib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ..
LIBS += -lquazip

# Input
HEADERS += testquazipfile.h \
           testquachecksum32.h \
           testjlcompress.h \
           qztest.h
SOURCES += testquazipfile.cpp \
           testquachecksum32.cpp \
           testjlcompress.cpp \
           qztest.cpp
