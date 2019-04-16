#
# harbour-taskswitcher spec
# (C) piggz 2018-2019
#

Name:       harbour-taskswitcher

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary:    Keyboard taskswitcher
Version:    0.3
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        https://github.com/piggz/harbour-taskswitcher
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
/usr/lib/systemd/user/harbour-taskswitcher.service
/usr/lib/systemd/user/harbour-taskswitcher-user.service

%post
# Enable and start services
systemctl --user daemon-reload
systemctl --user enable harbour-taskswitcher.service
systemctl --user enable harbour-taskswitcher-user.service
systemctl --user start harbour-taskswitcher.service
systemctl --user start harbour-taskswitcher-user.service

%pre
# In case of update, stop first
if [ "$1" = "2" ]; then
    systemctl --user stop harbour-taskswitcher.service
    systemctl --user stop harbour-taskswitcher-user.service
fi
exit 0

%preun
# in case of complete removal, stop and disable
if [ "$1" = "0" ]; then
    systemctl --user stop harbour-taskswitcher.service
    systemctl --user stop harbour-taskswitcher-user.service
    systemctl --user disable harbour-taskswitcher.service
    systemctl --user disable harbour-taskswitcher-user.service
fi
