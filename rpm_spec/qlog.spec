%define REPO_VERSION %(echo $REPO_VERSION)

Summary: Qt Logging program for hamradio operators
Name: QLog
Version: %{REPO_VERSION}
Release: 1%{?dist}
License: GPLv2
Group: Productivity/Hamradio/Logging
Source: https://github.com/foldynl/QLog/archive/refs/tags/v%{version}.tar.gz#/qlog-%{version}.tar.gz
Source1: https://github.com/foldynl/QLog-Flags/archive/refs/tags/v%{version}.tar.gz#/qlog-flags-%{version}.tar.gz
URL: https://github.com/foldynl/QLog/wiki
Packager: Ladislav Foldyna <ok1mlg@gmail.com>

%description
QLog is an Amateur Radio logging application for Linux, Windows and Mac OS. It
is based on the Qt 5 framework and uses SQLite as database backend.

%prep
%global debug_package %{nil}
%setup
%setup -T -D -b 1 
cp -r ../QLog-Flags-%{version}/* res/flags/


%build
/usr/bin/qmake-qt5 PREFIX='/usr'
%make_build

%install
INSTALL_ROOT=%{buildroot} make -f Makefile install

%post

%postun

%files
%{_bindir}/*
%license LICENSE
%doc README.md Changelog
%{_datadir}/applications/qlog.desktop
%{_datadir}//icons/hicolor/256x256/apps/qlog.png


%changelog
* Sun Dec 19 2021 Ladislav Foldyna - 0.3.0-1
- Rework Station Profile - stored in DB, new fields
- Added VUCC fields support
- Added BandMap marks (CTRL+M)
- Clublog is uploaded the same way as EQSL and LOTW (modified QSO are resent)
- Clublog real-time upload is temporary disabled
- Added QRZ suppor - upload QSO and Callsign query
- Callbook cooperation - Primary&Secondary - Secondary used when Primary did not find

* Sat Nov 27 2021 Ladislav Foldyna - 0.2.0-1
- Initial version of the package based on v0.2.0
