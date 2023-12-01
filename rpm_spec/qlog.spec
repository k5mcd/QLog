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
* Fri Dec 1 2023 Ladislav Foldyna - 0.30.0-1
- [NEW] - QSL Images are stored in the database
- [NEW] - Added AppStream Metainfo File (PR #262 thanks AsciiWolf)
- [NEW] - Added (WPX) prefix (issue #263)
- [NEW] - Added WPX Award statistics
- [NEW] - Added support for external translation files(issue #275)
- [CHANGED] - Removed QSOID from Export dialog column setting (issue #258)
- Fixed Date editor does not support NULL value in Logbook Direct Editor (issue #256)
- Fixed duplicate entry in Windows Add or Remove - only Window platform (issue #260)
- Fixed RST fields revert to 59 after changing them (issue #261)
- Fixed Cannot change TQSL Path in Settings - flatpak (issue #271)

* Mon Nov 13 2023 Ladislav Foldyna - 0.29.2-1
- Fixed QLog is already running error popup on MacOS (issue #257 thanks rjesson)

* Fri Nov 10 2023 Ladislav Foldyna - 0.29.1-1
- Fixed QSL cards tooltip are not displayed under qt6.5 (issue #248)
- Fixed Distance unit is not displayed in QSO Info under Windows (issue #250)
- Fixed Editing STATION_CALLSIGN can cause unwanted change in QSO Detail (issue #251)
- Fixed QSO Detail Operator Name containes an incorrect value (issue #252)
- Fixed Calls with VE, VA are coding as Amsterdam & St Paul Islands instead of Canada (issue #253)
- Fixed LoTW QSL import reports unmatched QSOs sometime (issue #254)

* Fri Oct 20 2023 Ladislav Foldyna - 0.29.0-1
- [NEW] - Added user-defined layout for New QSO Detail widget
- [NEW] - Main window State and Geometry can be saved to layout profile
- [NEW] - Awards - Added WAS
- [NEW] - Awards - WAZ/ITU/WAC show all possible values
- [NEW] - Distance unit (km/miles) is controlled by OS Locale
- [CHANGED] - Removed SAT Tab - field can be added via Layout Editor
- Improved Import QSO performance
- Fixed QLog crashes if POTA, SOTA or WWFF contain incorrect values (issue #245)
- Fixed QSOs are not uploaded to QRZ and HRDLog if fields contain HTML delimiter strings (issue #247)

* Fri Sep 22 2023 Ladislav Foldyna - 0.28.0-1
- [NEW] - Added ON4KST Chat Support
- [NEW] - Added Az BeamWidth and Az Offset to Antenna Profile
- [NEW] - Double-Clicking the IBP callsign in the online map tunes the frequency
- Fixed Browse button should open an expecting folder (issue #241)
- Fixed Reword QSL buttons and Settings in QSO Details and Settings (issue #242)

* Mon Aug 21 2023 Ladislav Foldyna - 0.27.0-1
- [NEW] - Added HRDLog Support
- Fixed Text field alignment (issue #233)
- Fixed Rig/Rot Connection port type selection (issue #235)
- Fixed Incorrect Distance Value in WSJTX Widget (issue #236)
- Fixed Incorrect WSJTX locator target on the map (issue #237)

* Sun Jul 30 2023 Ladislav Foldyna - 0.26.0-1
- [NEW] - Added user-defined layout for New QSO widget
- [NEW] - Pressing Spacebar in Callsign field skips RST fields
- [NEW] - Added user-defined URL for web lookup (issue #230)
- Fixed WSJTX QSOs should have an Operator Name from Callbook (issue #223)
- Fixed US call area suffixes not handled correctly (issue #226 thanks Florian)
- Fixed QSO Filter Detail allows to save an empty Filter Name (issue #228)

* Mon Jul 17 2023 Ladislav Foldyna 0.25.1-1
- Fixed Unexpected mode change when Setting Dialog is saved (issue #222)
- Fixed QSL_SENT field has an incorrect ADIF name (issue #225)

* Tue Jul 4 2023 Ladislav Foldyna - 0.25.0-1
- [NEW] - Export - Added CSV Format
- [NEW] - Export - Added Type of Export Generic/QSLs (issue #209)
- [NEW] - Export - Added Exported Columns Setting
- [NEW] - Export - All export formats use the ADIF field name convention
- [CHANGED] - Export - JSON format contains a header - JSON format change
- [CHANGED] - Default Statistics Interval is curr_date-1 and curr_day
- Fixed Errors from Secure Storage are not shown (issue #216)
- Fixed RX/TX Bands are uneditable when RX/TX freqs are missing (issue #220)

* Fri Jun 16 2023 Ladislav Foldyna - 0.24.0-1
- Fixed Incorrect FT4 mode-submode (issue #212)
- Fixed CONTESTIA mode should be CONTESTI (issue #213)
- Fixed Context menu deselects NewContactEditLine (issue #215)
- Fixed incorrect WSJTX Filter initialization (issue #218)

* Fri Jun 9 2023 Ladislav Foldyna - 0.23.0-1
- [NEW] - Added CWDaemon Keyer Support
- [NEW] - Added FLDigi Keyer Support
- [NEW] - Online Map - based on locale, the map language is selected (Only EN, FR, GE supported - issue #180)
- Fixed After entering longer QTH, the field content is not left-aligned (issue #157)
- Fixed wrong QSO Time in case of JTDX (issue #204)
- Fixed QSL Sent Date fields are not filled if QSL Sent Status fields are Y (issue #207)

* Sun May 7 2023 Ladislav Foldyna - 0.22.0-1
- [NEW] - ADIF Import - My Profile is used to define default values
- [NEW] - ADIF Import - Checking a minimal set of input fields (start_time, call, band, mode, station_callsign)
- [NEW] - ADIF Import - Added Import Result Summary + Import Detail Info
- [NEW] - Main Menu - Added Help -> Mailing List.
- [NEW] - Export - Filter for the exported QSOs
- [CHANGE] - Renamed Locator to Gridsquare
- Fixed Some anomalies in the input and processing of QSLr Date (issue #192)
- Fixed User unfriedly CW Keyer Error (issue #194)
- Fixed ADIF import (issue #196)
- Fixed Operator field is incorrectly used  (issue #197)
- Fixed Crash if an unknown POTA & SOTA/WWFF Setting is entered (issue #198)
- Fixed FLDIGI cannot connect QLog (issue #199)
- Fixed if ADIF record is missing band info, add this from freq field (thx DJ5CW)

* Sun Apr 16 2023 Ladislav Foldyna - 0.21.0-1
- [NEW] - Rotator - Added Used-Defined Buttons
- [NEW] - Rotator - Added Destination Azimuth Needle
- [NEW] - Online Map - Added Antenna Beam Path
- [NEW] - Rig - Combos are disbled when disconnected
- [NEW] - Club Member Lists (issue #60)
- [NEW] - Alert Table shows rule names
- [CHANGED] - Alerts, DXC and WSJTX Network Notifications
- Fixed Antenna Azimuth Negative Value (issue #191)
- Fixed CTY file is not loaded when duplicate record (issue #193)

* Tue Mar 14 2023 Ladislav Foldyna - 0.20.0-1
- [NEW] - Added MUF Layer to online map
- [NEW] - Added International Beacon Project (IBP) Beacons to online map
- [NEW] - Centering the map on the current profile at start (issue #185)
- Fixed incorrect ADIF interpretation of _SENT fields (issue #176)
- Fixed Awards Dialog, Table double click for ITU/CQZ/WAZ/IOTA shows incorrect QSOs (issue #177)
- Fixed ADIF double-type fields when 0.0 is currently mapped to NULL (issue #178)
- Fixed QSO Detail to save NULL instead of empty string (issue #179)
- Fixed ADIF Import default _INTL values are now stored correctly (issue #183)
- Fixed Maps show an incorrect path if from/to grids are the same (issue #186)
- Fixed Online Maps incorrect Bounds if Bandmap callsign double-click (issue #188)
- Updated German translation (thx DL2KI)

* Fri Feb 17 2023 Ladislav Foldyna - 0.19.0-1
- [NEW] - Added Aurora Layer to online map
- [NEW] - Logbook - filter options are saved and restored
- [NEW] - Map Setting is saved and restored (issue #140)
- [NEW] - QSO Duration (issue #158)
- [NEW] - DX Cluster uses monospace font (issue #164)
- [NEW] - Awards - if click on the Entity/band the logbook filter is set (issue #168)
- [NEW] - WSJTX - Added Multicast support (issue #172)
- Fixed WWFF LOV Download (issue #169)

* Sun Jan 15 2023 Ladislav Foldyna - 0.18.0-1
- [NEW] - ADIF 3.1.4 updates
-   Added new modes FREEDV and M17
-   Added new band (submm)
-   Adopted Altitude (for SOTA only)
-   Adopted POTA (includes POTA List)
-   Adopted Gridsquare_ext (only import/export)
-   Adopted Hamlogeu_* (only import/export)
-   Adopted HamQTH_* (only import/export)
- [NEW] - Added new DXCC Status and color for it - Confirmed
- [NEW] - New Contact - Tab selection is saved
- [NEW] - Grid can contain 8-characters
- [NEW] - User filter can contain NULL value
- [NEW] - Compilation - added variables for external sources
- [NEW] - My DXCC/CQZ/ITUZ/Country is filled
- [NEW] - Alerts - Added Aging (issue #153)
- [NEW] - Alerts - Added DXCC Status Color (issue #153)
- [NEW] - DXC - Added Log Status to filter (issue #154)
- [NEW] - DXC - Added Spot deduplication to filter (issue #154)
- [NEW] - WSJTX - Added CQ-Spot Filter (issue #155)
- [NEW] - QSO Detail contains DXCC Tab (issue #156)
- [CHANGED] - New QSO DXCC Tab reworked (issue #144)
- [CHANGED] - All DXCC Stats are computed based on My DXCC instead of My Callsign
- [CHANGED] - Station Profile Setting layout

* Sun Dec 18 2022 Ladislav Foldyna - 0.17.0-1
- [NEW] - NetPort and Polling interval can be defined for NET Rigs
- [NEW] - NetPort can be defined for NET Rots
- [NEW] - Added Saving Bandmap Zoom per band (issue #137)
- [NEW] - CW speed synchronisation (issue #139)
- Fixed Missing callbook data when callsign has prefix (issue #133)
- Fixed Winkey2 echo chars are incorrectly displayed in CW Console (issue #141)
- [CHANGED] - Online Map - Gray-Line is enabled by default
- Update Timezone database

* Sun Nov 20 2022 Ladislav Foldyna - 0.16.0-1
- [NEW] - SOTA/IOTA lists updated regularly
- [NEW] - Added WWFF list, updated regularly
- [NEW] - QTH/Grid are filled based on SOTA/WWFF
- [NEW] - DXC/WSJTX columns are movable, added column visibility setting
- [NEW] - DXC/WSJTX columns layout is saved
- [NEW] - Added Wiki&Report Issue links to Help section
- [NEW] - About dialog contains run-time information
- [NEW] - Solar Info as a ToolTip
- [NEW] - QSO Manual Entry Mode
- Fixed Bandmap unlogical animation when band is changed (issue #128)
- Fixed Bandmap marks are not displayed correctly when RIT/XI (issue #131)
- Fixed Setting Dialog size
- Update Timezone database

* Sun Oct 16 2022 Ladislav Foldyna - 0.15.0-1
- Fixed Keeping the Bandmap RX mark always visible when centre RX is disabled (issue #115)
- Fixed Equipment Menu: Swapped Connect Keyer and Rig (issue #122)
- Fixed Callsign is deleted when clicking bandmap (issue #126)
- Fixed typo in the Map layer menu (issue #127)
- Fixed compilation issues & warning under QT6 - preparation for QT6 migration

* Sun Oct 2 2022 Ladislav Foldyna - 0.14.1-1
- Fixed CW Console - HALT Button is not enabled under Ubuntu flavours (issue #124)

* Thu Sep 29 2022 Ladislav Foldyna - 0.14.0-1
- [NEW] CW Console (Winkey2, Morse over CAT support)
- [NEW] DX Cluster pre-defined commands (send last spot, get stats)
- [NEW] Added DX Cluster Views (Spots, WCY, WWV, ToALL)
- [NEW] Implemented DX Cluster Reconnection
- [NEW] Remember last used DX Cluster
- [CHANGED] - UI unifications - Rot/Rig/DXC
- Fixed COM port validation for Windows platform
- Fixed Reconnecting (DXC/Callbook) (issue #110)
- Fixed DX Cluster crashes when DXC server is not connected and a command is sent (issue #111)
- Fixed Bandmap callsign selection not fully works (issue #116)

* Sat Aug 6 2022 Ladislav Foldyna - 0.13.0-1
- [NEW] QSY Contact Wiping (issue #100)
- [NEW] Timeoff is highlighted when QSO timer is running (issue #100)
- [NEW] Callsign whisperer
- [NEW] Bandmap - Spot's color is recalculated when QSO is saved
- [NEW] BandMap - CTRL + Wheel zooming
- [NEW] BandMap - Zooming via buttons keeps a focus on centre freq
- [NEW] BandMap - DX Spot's Comment as a tooltip
- [CHANGED] BandMap - UI Layout
- Fixed MacOS builds (PR #102) (thx gerbert)
- Fixed templates under MacOS (PR #101) (thx gerbert)
- Fixed WindowsOS Installer - Unable to upgrade version

* Fri Jul 15 2022 Ladislav Foldyna - 0.12.0-1
- [NEW] Statistics - Show ODX on the map
- [EXPERIMENTAL] Support for QT Styles (issue #88)
- [CHANGED] - Removed F2 as a shortcut for QSO field editing
- Next fixing of a high CPU load when DXC is processed (issue #52)
- Fixed QSO fields from prev QSOs when Prefix - Callsign - Suffix (issue #90)
- Fixed Chaotic QSO start time (issue #93)
- Offline maps - Lighter colors, night sky removed, Sun position removed (issue #97)
- Fixed incorrect A-Index colort (issue #98)
- Fixed Stats Widget - percents - does not reflect date range (issue #99)
- Fixed potential LogParam Cache issue
- Import/Export polishing

* Sun Jun 26 2022 Ladislav Foldyna - 0.11.0-1
- [NEW] QSO Detail/Edit Dialog
- [NEW] Added mW power Support
- [NEW] Implemented ADIF 3.1.3
- [NEW] Rigwidget saves last used freq for bands
- Fixed Rig Combo size when Rig Profile name is long (issue #31)
- Fixed CQZ, ITUZ do not validate whether their entered value is a number (issue #75)
- Fixed vucc, myvucc must be uppercase - Edit mode (issue #76)
- Fixed Greyline-Map is very dark (issue #78)
- Fixed DX Country is not saved properly when name is between S-Z (issue #79)
- Fixed Bandmap call selection - only left mouse button (issue #82)
- Fixed My Notes Copy & Paste - Rich Text (issue #83)
- Fixed Font appearance in the context menu (issue #84)

* Sun Jun 5 2022 Ladislav Foldyna - 0.10.0-1
- [NEW] Bandmap shows XIT/RIT Freq
- [NEW] Bandmap RX Mark Center (issue #69)
- [NEW] Getting PTT State from RIG - only for CAT-controlled rigs
- [NEW] PTT Shortchut - only for CAT-controlled rigs
- Fixed Lost internet conneciton is not detected properly (issue #56)
- Fixed Cannot manually edit QSO Date&Time (issue #66)
- Fixed Field contents in capital letters (issue #67)
- Fixed Band RX is not updated when RX Freq is edited (issue #72)
- Fixed Stat Windget does not handle a date range correctly (issue #73)
- Fixed eQSL card is incorreclty handled when a callsign contains special characters (issue #74)

* Fri May 20 2022 Ladislav Foldyna - 0.9.0-1
- [NEW] User-defined Spot Alerts
- [NEW] User filter contains a new operator "Starts with"
- [NEW] a real local time is shown for the DX callsign (issue #45)
- [NEW] Lotw/eQSL registration info is showed from callbooks
- [NEW] Added shortcuts for menu and tabs
- [NEW] Bandmap - Switching a band view via Bandmap context menu (issue #57)
- [CHANGED] - Network Notification format
- Fixed issue with My Notes multiple lines edit/show mode (issue #39)
- Fixed issue when GUI froze when Rig disconnect was called (issue #50)
- Partially fixed a high CPU load when DXC is processed (issue #52)
- Fixed crashes under Debian "bullseye" - 32bit (issue #55)
- Fixed Bandmap Callsign selection margin (issue #61)
- Fixed issue when it was not possible to enter RX/TX freq manually

* Fri Apr 22 2022 Ladislav Foldyna - 0.8.0-1
- RIT/XIT offset enable/disable detection (issue #26)
- Fixed Rig Setting, Data Bits (issue #28)
- Added default PWR for Rig profile (issue #30)
- Fixed issue when GUI freezes during Rig connection (issue #32 & #33)
- Fixed issue with an incorrect value of A-Index (issue #34)
- Fixed ADI Import - incorrect _INTL fields import (issue #35)
- Fixed isuue with an editing of bands in Setting dialog (#issue 36)
- Fixed issue with hamlib when get_pwr crashes for a network rig (issue #37)
- Improved new QSO fields are filled from prev QSO (issue #40)
- Added mode for a network Rig (issue #41)
- Fixed warning - processing a new request but the previous one hasn't been completed (issue #42)
- Fixed Info widget when Country name is long (issue #43)
- Reordered column visibility Tabs (issue #46)
- Improved Rig tunning when XIT/RIT is enabled (issue #47)

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
