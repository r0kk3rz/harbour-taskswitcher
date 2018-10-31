TARGET = harbour-tohkbd2

QT += dbus network
QT -= gui

CONFIG += link_pkgconfig
PKGCONFIG += Qt5SystemInfo

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

target.path = /usr/bin/

udevrule.path = /etc/udev/rules.d/
udevrule.files = config/95-$${TARGET}.rules

vkblayout.path = /usr/share/maliit/plugins/com/jolla/layouts/
vkblayout.files = config/layouts/$${TARGET}.conf config/layouts/$${TARGET}.qml

INSTALLS += target udevrule vkblayout

message($${DEFINES})

SOURCES += \
    src/tohkbd2-daemon.cpp

HEADERS +=

OTHER_FILES += \


