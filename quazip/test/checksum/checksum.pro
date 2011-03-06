
include (../../includes.pri)

TEMPLATE = app
CONFIG += qt warn_on console

QT -= gui
DEPENDPATH += .
INCLUDEPATH += . ../../

LIBS += -L../../bin -lquazip

# Input
SOURCES += main.cpp
