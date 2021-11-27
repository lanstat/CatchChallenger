contains(DEFINES, Q_OS_ANDROID) {
VIEWPORT_BASE = $$PWD/mobile
}
!contains(DEFINES, Q_OS_ANDROID) {
VIEWPORT_BASE = $$PWD/desktop 
}

SOURCES += $$VIEWPORT_BASE/Battle.cpp
SOURCES += $$VIEWPORT_BASE/Menu.cpp
