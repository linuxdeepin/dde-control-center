## DDE 控制中心

DDE 控制中心控制整个DDE桌面环境

## 依赖

查看 `debian/control` 文件来了解此项目的构建与运行时依赖，或者使用 `cmake` 检查缺失的必要组件。

## 安装

### 源码安装

1. Make sure you have installed all dependencies.

2. Build:
```bash
cd dde-control-center
cmake -B build
cmake --build build 
```

3. Install:
```bash
sudo make install
```
注意，不推荐，应该先打包然后安装。

运行文件在 `/usr/bin/dde-control-center` , 插件在 `${CMAKE_INSTALL_FULL_LIBDIR}/dde-control-center/modules/`下，`CMAKE_INSTALL_FULL_LIBDIR`请查阅GNUInstallDir,根据发行版有所不同 , 如arch下这个目录在 `/usr/lib/dde-control-center/modules/`.

为在 *deepin* 桌面发行版进行此软件包的构建，我们还提供了一个 `debian` 目录。若要构建软件包，可参照下面的命令进行构建：

```shell
$ sudo apt build-dep . # 安装构建依赖
$ dpkg-buildpackage -uc -us -nc -b # 构建二进制软件包
```

## 使用方式

Execute `dde-control-center -h` to get more details.

## 帮助

任何使用问题都可以通过以下方式寻求帮助:

* [Telegram 群组](https://t.me/deepin_community)
* [Matrix](https://matrix.to/#/#deepin-community:matrix.org)
* [IRC (libera.chat)](https://web.libera.chat/#deepin-community)
* [Forum](https://bbs.deepin.org)
* [WiKi](https://wiki.deepin.org/)

## 加入我们

请到developer-center提交issue或者参与讨论。当然也可以直接参与代码维护提交代码和修复。我们会很开心获得您的帮助和贡献

* [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en). (English)
* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

## License

DDE Control Center 遵循协议 [GPL-3.0-or-later](LICENSE).
