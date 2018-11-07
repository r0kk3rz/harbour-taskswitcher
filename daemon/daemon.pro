TARGET = harbour-taskswitcher

QT += dbus network
QT -= gui

CONFIG += link_pkgconfig
#PKGCONFIG += Qt5SystemInfo

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

target.path = /usr/bin/

systemd_services.path = /usr/lib/systemd/user/
systemd_services.files = harbour-taskswitcher.service

INSTALLS += target \
                          systemd_services

message($${DEFINES})

SOURCES += \
    src/taskswitcher-daemon.cpp \
    src/eventhandler.cpp \
    src/worker.cpp

HEADERS += \
    src/eventhandler.h \
    src/worker.h

OTHER_FILES += \


