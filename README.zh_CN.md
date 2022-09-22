## DDE网络核心框架
DDE网络核心框架是UOS桌面环境的网络连接框架。

## 依赖
请查看“debian/control”文件中提供的“Depends”。

### 编译依赖
请查看“debian/control”文件中提供的“Build-Depends”。

## 安装

### 构建过程

1. 确保已经安装了所有的编译依赖

2. 构建
```
$ cd dde-network-core
$ mkdir Build
$ cd Build
$ cmake ..
$ make
```

3. 安装
```
$ sudo make install
```

安装完成后可执行二进制文件在/usr/bin/dde-network-dialog，插件会放在相关模块的插件目录下。

## 用法

执行 `dde-network-dialog -h` 以获取更多详细信息。

## 获得帮助

当你使用DDE控制中心，进入网络模块时，您可以按 `F1` 启动 [deepin-manual](https://github.com/linuxdeepin/deepin-manual)。

如果您遇到任何其他问题，您可能还会发现这些渠道很有用：

* [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
* [IRC Channel](https://webchat.freenode.net/?channels=deepin)
* [官方论坛](https://bbs.deepin.org/)
* [Wiki](https://wiki.deepin.org/)
* [项目地址](https://github.com/linuxdeepin/dde-network-core) 

## 贡献指南

我们鼓励您报告问题并做出更改

* [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en). (English)
* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

## License

DDE网络核心框架在 [GPL-3.0-or-later](LICENSE)下发布。
