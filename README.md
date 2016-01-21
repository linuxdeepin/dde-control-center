# DDE Control Center

DDE Control Center is Deepin Desktop Environment control panel.

## Dependencies

### Build dependencies

* qmake (>= 5.3)
* [libdui](https://github.com/linuxdeepin/libdui) (developer package)
* [dde-dock](https://github.com/linuxdeepin/dde-dock) (developer package)

### Runtime dependencies

* [libdui](https://github.com/linuxdeepin/libdui)
* [dde-dock](https://github.com/linuxdeepin/dde-dock)
* [dde-account-faces]((https://github.com/linuxdeepin/dde-account-faces)
* [dde-api]((https://github.com/linuxdeepin/dde-api)
* [dde-daemon]((https://github.com/linuxdeepin/dde-daemon)
* [deepin-artwork-themes]((https://github.com/linuxdeepin/deepin-artwork-themes)
* gtk+-2.0
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

When install complete, the executable binary file is placed into `/usr/bin/dde-control-center`, and plugins file will be placed into `/usr/lib/dde-control-center/modules/`.

## Usage

Execute `dde-control-center -h` to get more detail.

## Getting help

You can press `F1` to start [deepin-manual](https://github.com/linuxdeepin/deepin-manual) when you focus on DDE Control Center window.
Other usage issues can ask via these channels:
* [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
* [IRC Channel](https://webchat.freenode.net/?channels=deepin)
* [Official Forum](https://bbs.deepin.org/)
* [Wiki](http://wiki.deepin.org/)

## License

DDE Control Center is licensed under [GPLv3](https://github.com/linuxdeepin/developer-center/wiki/LICENSE).
