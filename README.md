# DDE Control Center

DDE Control Center is the control panel of Deepin Desktop Environment.

## Dependencies

### Build dependencies

* qmake (>= 5.3)
* [libdui](https://github.com/linuxdeepin/libdui) (developer package)
* [dde-dock](https://github.com/linuxdeepin/dde-dock) (developer package)

### Runtime dependencies

* [libdui](https://github.com/linuxdeepin/libdui)
* [dde-dock](https://github.com/linuxdeepin/dde-dock)
* [dde-account-faces](https://github.com/linuxdeepin/dde-account-faces)
* [dde-api](https://github.com/linuxdeepin/dde-api)
* [dde-daemon](https://github.com/linuxdeepin/dde-daemon)
* [deepin-artwork-themes](https://github.com/linuxdeepin/deepin-artwork-themes)
* Qt5 (>= 5.3)
  * Qt5-DBus
  * Qt5-Multimedia
  * Qt5-MultimediaWidgets
  * Qt5-Svg
  * Qt5-X11extras

## Installation

### Build from source code

1. Make sure you have installed all dependencies.

2. Build:
```
$ cd dde-control-center
$ mkdir Build
$ cd Build
$ qmake ..
$ make
```

3. Install:
```
$ sudo make install
```

The executable binary file could be found at `/usr/bin/dde-control-center` after the installation is finished, and plugins will be placed into `/usr/lib/dde-control-center/modules/`.

## Usage

Execute `dde-control-center -h` to get more details.

## Getting help

You can press `F1` to start [deepin-manual](https://github.com/linuxdeepin/deepin-manual) when you focus on DDE Control Center window.

You may also find these channels useful if you encounter any other issues:

* [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
* [IRC Channel](https://webchat.freenode.net/?channels=deepin)
* [Official Forum](https://bbs.deepin.org/)
* [Wiki](http://wiki.deepin.org/)

## License

DDE Control Center is licensed under [GPLv3](LICENSE).
