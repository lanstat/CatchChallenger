include(../../../../server/catchchallenger-server.pri)

QT       += sql core
DEFINES += CATCHCHALLENGER_CLASS_QT

RESOURCES += $$PWD/../../../../server/all-server-resources.qrc \
    $$PWD/../../../../server/databases/resources-db-sqlite.qrc

#now in generale, only linux is C only, mac, windows, other is in Qt for compatibility
#win32:RC_FILE += $$PWD/resources-windows.rc

QT       += gui network

SOURCES += $$PWD/../../../../server/base/QtServer.cpp \
    $$PWD/../../../../server/base/QtTimerEvents.cpp \
    $$PWD/../../../../server/base/QtDatabase.cpp \
    $$PWD/QFakeSocket.cpp \
    $$PWD/QFakeServer.cpp \
    $$PWD/InternalServer.cpp \
    $$PWD/../../../../server/NormalServer.cpp \
    $$PWD/../../../../server/NormalServerGlobal.cpp \
    $$PWD/../../../../server/QSslServer.cpp
HEADERS += $$PWD/../../../../server/base/QtServer.hpp \
    $$PWD/../../../../server/base/QtTimerEvents.hpp \
    $$PWD/../../../../server/base/QtDatabase.hpp \
    $$PWD/QFakeServer.hpp \
    $$PWD/QFakeSocket.hpp \
    $$PWD/InternalServer.hpp \
    $$PWD/../../../../server/NormalServer.hpp \
    $$PWD/../../../../server/NormalServerGlobal.hpp \
    $$PWD/../../../../server/QSslServer.hpp

