TEMPLATE = lib
CONFIG += qt warn_on
QT -= gui
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += *.h

SOURCES += *.c *.cpp

unix {
    headers.path=$$PREFIX/include/quazip
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib
    INSTALLS += headers target

	OBJECTS_DIR=.obj
	MOC_DIR=.moc
	
	LIBS += -lz
}

win32 {
    headers.path=$$PREFIX/include/quazip
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib
    INSTALLS += headers target

    LIBS += -lzlib
    *-msvc*: QMAKE_LFLAGS += /IMPLIB:$$DESTDIR\\quazip.lib
}

DEFINES += QUAZIP_BUILD
