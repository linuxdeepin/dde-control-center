## DDE Control Center

DDE Control Center is the control panel of Deepin Desktop Environment.

## Dependencies

Check `debian/control` for build-time and runtime dependencies, or use `cmake` to check the missing required dependencies.

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

A `debian` folder is provided to build the package under the *deepin* linux desktop distribution. To build the package, use the following command:

```shell
$ sudo apt build-dep . # install build dependencies
$ dpkg-buildpackage -uc -us -nc -b # build binary package(s)
```

## Usage

Execute `dde-control-center -h` to get more details.

Note: `--spec` can be used to debug plugins. The passed in value is the path where the so of plugin is in.

## Getting help

You can press `F1` to start [deepin-manual](https://github.com/linuxdeepin/deepin-manual) when you focus on DDE Control Center window.

You may also find these channels useful if you encounter any other issues:

* [Telegram group](https://deepin.org/to/tg)
* [Matrix](https://matrix.to/#/#deepin-community:matrix.org)
* [IRC (libera.chat)](https://web.libera.chat/#deepin-community)
* [Forum](https://bbs.deepin.org)
* [WiKi](https://wiki.deepin.org/)

## Develop Plugins with cmake

```cmake

# just show the target link way

find_package(DdeControlCenter REQUIRED)
find_package(Dtk COMPONENTS Core Widget REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_library(dcc_exampleplugin SHARED
    plugin.h
    plugin.cpp
)

target_link_libraries(dcc_exampleplugin PRIVATE
    Dde::DCCWidget
    Dde::DCCInterface
    Dtk::Core
    Dtk::Widget
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

```

## Getting involved

We encourage you to report issues and contribute changes

* [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en). (English)
* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

## License

DDE Control Center is licensed under [GPL-3.0-or-later](LICENSE).
