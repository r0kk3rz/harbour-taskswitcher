#
# harbour-ambience-tohkbd2 spec
# (C) kimmoli 2014-2015
#

Name:       harbour-taskswitcher

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary:    Bluetooth keyboard taskswitcher
Version:    0.0.devel
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        https://github.com/kimmoli/tohkbd2
Source0:    %{name}-%{version}.tar.bz2

BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(sailfishapp) >= 0.0.10
BuildRequires:  pkgconfig(nemonotifications-qt5)
BuildRequires:  pkgconfig(Qt5SystemInfo)
BuildRequires:  mce-headers
BuildRequires:  desktop-file-utils
BuildRequires:  qt5-qttools-linguist

Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   ambienced

%description
%{summary}

%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5 SPECVERSION=%{version}

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

desktop-file-install --delete-original \
    --dir %{buildroot}%{_datadir}/applications \
    %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/harbour-taskswitcher
%attr(755,root,root) %{_bindir}/harbour-taskswitcher-user
%{_datadir}/harbour-taskswitcher-user/
%{_datadir}/applications

#%post
#DBUS_SESSION_BUS_ADDRESS="unix:path=/run/user/100000/dbus/user_bus_socket" \
#    dbus-send --type=method_call --dest=org.freedesktop.DBus / org.freedesktop.DBus.ReloadConfig
##restart maliit
#systemctl-user restart maliit-server
##reload udev rules
#udevadm control --reload
# if tohkbd2 is connected, start daemon now
# vendor id 6537 = 0x1989 = dirkvl
# product id 3 = tohkbd2
#if [ -e /sys/devices/platform/toh-core.0/vendor ]; then
#    if grep -q 6537 /sys/devices/platform/toh-core.0/vendor ; then
#        if grep -q 3 /sys/devices/platform/toh-core.0/product ; then
#            systemctl start harbour-tohkbd2.service
#        fi
#    fi
#fi

#%pre
# In case of update, stop and disable first
#if [ "$1" = "2" ]; then
#    systemctl stop harbour-tohkbd2.service
#    systemctl disable harbour-tohkbd2.service
#    udevadm control --reload
#fi

#%preun
## in case of complete removal, stop and disable
#if [ "$1" = "0" ]; then
#    systemctl stop harbour-tohkbd2.service
#    systemctl disable harbour-tohkbd2.service
#    udevadm control --reload
#fi

#%postun
#DBUS_SESSION_BUS_ADDRESS="unix:path=/run/user/100000/dbus/user_bus_socket" \
#    dbus-send --type=method_call --dest=org.freedesktop.DBus / org.freedesktop.DBus.ReloadConfig
#restart maliit
#systemctl-user restart maliit-server
