TARGET = harbour-taskswitcher-user

CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5 nemonotifications-qt5
PKGCONFIG += Qt5SystemInfo

QT += dbus network gui-private

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

message($${DEFINES})


systemd_services.path = /usr/lib/systemd/user/
systemd_services.files = harbour-taskswitcher-user.service

INSTALLS += target \
                          systemd_services

SOURCES += \
    src/tohkbd2user.cpp \
    src/userdaemon.cpp \
    src/viewhelper.cpp \
    src/applauncher.cpp \
    src/screenshot.cpp

HEADERS += \
    src/userdaemon.h \
    src/viewhelper.h \
    src/applauncher.h \
    src/screenshot.h

OTHER_FILES += \
    qml/taskswitcher.qml

DISTFILES += \
    harbour-taskswitcher-user.desktop

