## DDE控制中心
DDE控制中心是UOS桌面环境的控制面板。
## 依赖
请查看“debian/control”文件中提供的“Depends”。

### 编译依赖
请查看“debian/control”文件中提供的“Build-Depends”。

## 安装

### 构建过程

1. 确保已经安装了所有的编译依赖

2. 构建
```
$ cd dde-control-center
$ mkdir Build
$ cd Build
$ cmake ..
$ make
```

3. 安装
```
$ sudo make install
```

安装完成后可执行二进制文件在/usr/bin/dde-control-center，插件会放在/usr/lib/dde-control-center/modules/ 目录下。

## 用法

执行 `dde-control-center -h` 以获取更多详细信息。

## 获得帮助

当你正在使用 DDE 控制中心时，您可以按 `F1` 启动 [deepin-manual](https://github.com/linuxdeepin/deepin-manual)。

如果您遇到任何其他问题，您可能还会发现这些渠道很有用：

* [Matrix](https://matrix.to/#/#deepin-community:matrix.org)
* [官方论坛](https://bbs.deepin.org/)
* [Wiki](https://wiki.deepin.org/)
* [项目地址](https://github.com/linuxdeepin/dde-control-center)
* [开发者中心](https://github.com/linuxdeepin/developer-center/issues) 

## 贡献指南

我们鼓励您报告问题并做出更改

* [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en). (English)
* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

## License

DDE控制中心在 [LGPL-3.0-or-later](LICENSE)下发布。
