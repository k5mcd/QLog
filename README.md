# QLog - UNDER DEVELOPMENT - TESTERS ARE WELCOME

QLog is an Amateur Radio logging application for Linux, Windows and Mac OS. It
is based on the Qt 5 framework und uses SQLite as database backend.

QLogs aims to be as simple as possible, but to provide everything the operator expects from the log. This log is not currently focused on contests.

![Screenshot](https://foldynl.github.io/QLog/screens/qlog_main.png)

## Features

- ADIF import/export
- Rig and rotator control via Hamlib
- HamQTH callbook integration
- DX cluster integration
- **LotW**, **eQSL** and **Clublog** integration (**eQSL includes QSL pictures download**)
- **Secure Password Storage** for all services with password
- **Online** and **Offline** map
- Bandmap
- WSJT-X integration
- Station Location Profile support
- Various station statistics
- Basic Awards support
- Custom QSO Filters
- Basic Satellite support
- **NO** ads, **NO** user tracking, **NO** hidden telemetry - simply free and open-source

## Installation

### Windows
**For developers:**

Prerequisites

- Visual Studio 2015
- Installed Qt 5.12 for Windows
- Installed qtkeychain-devel library and headers (https://github.com/frankosterfeld/qtkeychain)
- Installed OpenSSL-devel libraries and headers
- Installed HamLib-devel libraries and headers
- All integrated in QT Creator

To be honest, It is not easy to compile it under Windows but it is possible.

**For users:**

Installation .exe package will be available for every release - only 64bit version.

### Linux

**For developers:**

`sudo apt-get -y install qtbase5-dev libsqlite3-dev install libhamlib++-dev libqt5charts5-dev qt5-default qttools5-dev-tools libqt5keychain1 qt5keychain-dev qtwebengine5-dev`

`qmake QLog.pro`

`make`

For users:

Repos for Ubuntu (.deb) will be available soon via PPA. RPM package are planned in a next version. Snap or Flatpak are not planned at this moment.

### MacOS

TBD - Need help

## License

Copyright (C) 2020  Thomas Gatzweiler

Copyright (C) 2021  Ladislav Foldyna

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
