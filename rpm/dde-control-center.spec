Name:           dde-control-center
Version:        5.1.0.19
Release:        2
Summary:        New control center for Linux Deepin
License:        GPLv3
URL:            https://github.com/linuxdeepin/%{name}
Source0:        %{url}/archive/%{version}/%{name}-%{version}.tar.gz

#BuildRequires:  dtkcore-devel >= 5.1.1
BuildRequires:  gcc-c++
BuildRequires:  desktop-file-utils
BuildRequires:  dde-dock-devel
BuildRequires:  pkgconfig(dde-network-utils)
#BuildRequires:  pkgconfig(dtkwidget) >= 5.1
BuildRequires:  dtkwidget-devel
BuildRequires:  dtkgui-devel dtkcore-devel
#BuildRequires:  pkgconfig(dtkwidget2)
#BuildRequires:  pkgconfig(dframeworkdbus) >= 2.0
BuildRequires:  dde-qt-dbus-factory-devel
BuildRequires:  pkgconfig(gsettings-qt)
BuildRequires:  pkgconfig(geoip)
BuildRequires:  pkgconfig(libnm)
BuildRequires:  libpwquality-devel
BuildRequires:  qt5-devel
BuildRequires:  pkgconfig(xcb-ewmh)
BuildRequires:  kf5-networkmanager-qt-devel
BuildRequires:  udisks2-qt5-devel
BuildRequires:  qt5-linguist
BuildRequires:  cmake
Requires:       dde-account-faces
Requires:       dde-api
Requires:       dde-daemon
Requires:       dde-qt5integration
Requires:       dde-network-utils
Requires:       startdde
Requires:       dde-server-industry-config
Requires:	deepin-pw-check
Requires:	deepin-installer-timezones
Requires:	deepin-sync-daemon

%description
New control center for Linux Deepin.

%package devel
Summary:        %{summary}
BuildArch:      noarch

%description devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}
sed -i 's|lrelease|lrelease-qt5|' translate_generation.sh
sed -i -E '/add_compile_definitions/d' CMakeLists.txt

%build
%cmake . -DDCC_DISABLE_GRUB=YES \
         -DDISABLE_SYS_UPDATE=YES
%make_build

%install
%make_install INSTALL_ROOT=%{buildroot}
# place holder plugins dir
mkdir -p %{buildroot}%{_libdir}/%{name}/plugins
# https://github.com/linuxdeepin/dde-control-center/issues/115
# And we disabled SYS_UPDATE, so reboot-reminder-dialog is useless.
#rm %{buildroot}%{_bindir}/reboot-reminder-dialog
mkdir -p %{buildroot}/usr/lib64/cmake/DdeControlCenter
mv %{buildroot}/cmake/DdeControlCenter/DdeControlCenterConfig.cmake %{buildroot}/usr/lib64/cmake/DdeControlCenter
mv %{buildroot}/usr/lib/libdccwidgets.so %{buildroot}%{_libdir}/
install -Dm644 com.deepin.controlcenter.addomain.policy %{buildroot}%{_datadir}/polkit-1/actions/

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/%{name}.desktop ||:

%ldconfig_scriptlets

%files
%doc README.md
%license LICENSE
%{_sysconfdir}/
%{_bindir}/abrecovery
%{_bindir}/dde-control-center
%{_datadir}/
%{_libdir}/libdccwidgets.so
/etc/xdg/autostart/deepin-ab-recovery.desktop

%files devel
%{_includedir}/dde-control-center
%{_libdir}/cmake/DdeControlCenter/

%changelog
* Fri Jul  3 2020 uniontech <uoser@uniontech.com> - 5.1.0.19-2
- Add dde.sh to profile.d

* Mon Jun 15 2020 uniontech <uoser@uniontech.com> - 5.1.0.19
- Remove the universal menu.

* Fri May 29 2020 uniontech <uoser@uniontech.com> - 5.0.30
- Project init.

