TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    basedb.cpp \
    dmdb.cpp \
    inifile.cpp \
    main.cpp \
    MyDate.cpp \
    LoadForNoTem.cpp \
    LoadDay.cpp

HEADERS += \
    basedb.h \
    dmdb.h \
    inifile.h \
    MyDate.h \
    LoadForNoTem.h \
    LoadDay.h


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../opt/dmdbms/bin/release/ -ldmapi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../opt/dmdbms/bin/debug/ -ldmapi
else:unix: LIBS += -L$$PWD/../../../../../opt/dmdbms/bin/ -ldmapi

INCLUDEPATH += $$PWD/../../../../../opt/dmdbms/bin
DEPENDPATH += $$PWD/../../../../../opt/dmdbms/bin
INCLUDEPATH += /opt/dmdbms/bin/include
