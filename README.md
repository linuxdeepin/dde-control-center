## DDE Control Center

DDE Control Center is the control panel of Deepin Desktop Environment.
## Dependencies

### Build dependencies
* pkg-config
* cmake
* Qt5(>= 5.6) with modules:
  - qt5-default
  - qt5declarative5-dev
  - qt5svg5-dev
  - qttools5-dev-tools
  - qtdeclarative5-dev
* Deepin-tool-kit(>=2.0) with modules:
  - dtkcore-dev
  - dtkwidget-dev
  - dtkcore-bin
* geoip-dev
* dde-qt-dbus-factory
* gsettings-qt-dev
* kf5networkmanagerqt-dev
* xrandr-dev
* mtdev-dev
* fontconfig1-dev
* freetype6-dev
* egl1-mesa-dev
* xcb-ewmh-dev

### Runtime dependencies

* [deepin-tool-kit](https://github.com/linuxdeepin/deepin-tool-kit)
* [dde-api](https://github.com/linuxdeepin/dde-api)
* [dde-daemon](https://github.com/linuxdeepin/dde-daemon)
* [startdde](https://github.com/linuxdeepin/startdde)
* [dde-qt-dbus-factory](https://github.com/linuxdeepin/dde-qt-dbus-factory)
* [qt5integration](https://github.com/linuxdeepin/qt5integration)
* geoip-database
* Qt5(>= 5.6)
  * Qt5-DBus
  * Qt5-Multimedia
  * Qt5-MultimediaWidgets
  * Qt5-Svg

## Installation

### Build from source code

1. Make sure you have installed all dependencies.

2. Build:
```
$ cd dde-control-center
$ mkdir Build
$ cd Build
$ cmake ..
$ make
```

3. Install:
```
$ sudo make install
```

The executable binary file could be found at `/usr/bin/dde-control-center` after the installation is finished, and plugins will be placed into `${CMAKE_INSTALL_FULL_LIBDIR}/dde-control-center/modules/`, usually is `/usr/lib/dde-control-center/modules/`.

## Usage

Execute `dde-control-center -h` to get more details.

## Getting help

You can press `F1` to start [deepin-manual](https://github.com/linuxdeepin/deepin-manual) when you focus on DDE Control Center window.

You may also find these channels useful if you encounter any other issues:

* [Telegram group](https://t.me/deepin)
* [Matrix](https://matrix.to/#/#deepin-community:matrix.org)
* [IRC (libera.chat)](https://web.libera.chat/#deepin-community)
* [Forum](https://bbs.deepin.org)
* [WiKi](https://wiki.deepin.org/)


## Getting involved

We encourage you to report issues and contribute changes

* [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en). (English)
* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

## License

DDE Control Center is licensed under [GPL-3.0-or-later](LICENSE).
