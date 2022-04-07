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
* Fri Apr 8 2022 Ladislav Foldyna - 0.7.0-1
- [NEW] Ant/Rig/Rot Profiles
- [NEW] Rig widget shows additional information
- [NEW] Rig widget Band/Mode/Profile Changer
- [NEW] Rot profile Changer
- [NEW] AZ/EL are stored when Rot is connected
- Fixed an issue with Statistic widget (issue #25)
- Fixed Rot AZ current value (issue #22)

* Thu Mar 10 2022 Ladislav Foldyna - 0.6.5-1
- Fixed missing modes in Setting Dialog (issue #11)
- Fixed Station Profile text color in dark mode (issue #10)
- Fixed DXCluster Server Combo (issue #12)
- Fixed TAB focus on QSO Fields (issue #14)

* Sun Mar 6 2022 Ladislav Foldyna - 0.6.0-1
- [NEW] QSL - added import a file with QSL - QSLr column
- Fixed QLog start when Band is 3cm (too long start time due to the Bandmap drawing) (issue #6)
- Fixed Rotator Widget Warning - map transformation issue (issue #8)
- Changed Bandmap window narrow size (issue #3)
- Changed User Filter Widget size
- Removed Units from Logbook widget
- Removed UTC string
- Renamed RSTs, RSTr etc. (issue #4)
- Renamed Main Menu Services->Service and Station->Equipment
- Internal - reworked Service networking signal handling

* Sat Feb 19 2022 Ladislav Foldyna - 0.5.0-1
- DB: Started to use *_INTL fields
- DB: Added all ADIF-supported modes/submodes
- GUI: Dark Mode
- GUI: TIme format controlled by Locale
- Import/Export: ADI do not export UTF-8 characters and *_INTL fields
- Import/Export: ADX exports UTF-8 characters and *_INTL fields
- Import/Export: Added Import of ADX file format
- Logbook: Shows QSO summary as a Callsign's tooltip
- Logbook: QSO time is shown with seconds; added timezone
- New QSO: Added My notes - free text for your personal notes
- Backup: Change backup format form ADI to ADX (ADX supports UTF-8)
- Settings: WSJTX Port is changable

* Sun Jan 9 2022 Ladislav Foldyna - 0.4.0-1
- Stats: Added Show on Map - QSOs and Worked&Confirmed Grids
- Stats: Stats are refreshed after every QSO
- WSJTX: Remove TRX/Monitoring Status
- Added Split mode - RX/TX RIG Offset
- Added export of selected QSOs
- Fixed FLdigi interface
- CPPChecks & Clazy cleanup

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
