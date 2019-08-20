# 连接编辑页面

经过重构之后，网络编辑相关功能将不再依赖dde-session-daemon，而是通过libkf5networkmanagerqt库和NetworkManager直接通信。

## 基本结构

提供编辑功能相关的主要文件有类似connectioneditpage，settings目录和sections目录中的文件。
类似connectioneditpage的文件是编辑界面的入口，其上层就是一些设备界面，如WiredPage，WirelessPage等，
设备界面负责将编辑页面实例化并显示出来，剩下的事情就全部都是编辑界面的工作了。
之所以说类似connectioneditpage的文件是因为某些编辑页面需要一切更多的设备相关的数据才能初始化，
比如connectionwirelesseditpage， 在编辑一个从没有连接过的Ap时，就需要知道这个Ap的信息。

编辑页面在实现上主要被分为了两个部分：

- 顶部和底部的按钮们
- 中部是一个编辑widget

中部widget是由settings目录下的文件提供的，按照不同的网络连接实现了不同的settings类。 
从编辑页面的角度来看中部的编辑widget是一个整体，但实际的实现是由一个个section组合起来的，
所有的section都在sections目录下，按照功能抽象和实现了诸如通用，8021x，ipv4，以太，无线等等不同的section，
可以按需使用，也可以继承进行扩展。

## 主要类

### 控制中心中的类

| 类名 | 简介 |
| :- | :- |
| ConnectionEditPage | 编辑界面入口类 |
| AbstractSettings | Settings抽象类，供ConnectionEditPage统一管理 |
| AbstractSections | Sections抽象类，供Settings统一管理 |

其他的类基本都是继承上述三个类，然后实现检查错误和保存设置的方法，并做一些自己的界面处理工作。
在集成一些非抽象类时，需要留意类中的init方法，可以参考已有的继承例子: `SecretWiredSection继承Secret8021xSection`

### 库中的类

命名空间：NetworkManage

在控制中心的代码中使用了库中的代码的地方都指明使用了其命名空间

| 类名 | 简介 |
| :- | :- |
| Connection | 代表了一个NetworkManager的连接(可对应到具体配置文件) |
| ConnectionSettings | 一个连接的设置，包含一些通用设置也可以获取到具体的Setting，其定义了各种Connection类型的enum |
| Setting | 所有具体Setting的父类，其定义了各种Setting类型的enum |
| Ipv4Setting | 继承自Setting，抽象了ipv4的设置，可以将其对应到控制中心的ipv4设置小节 |
| ... | 还有大量其他类似的子类Setting |

### 库相关的其他知识

libkf5networkmanagerqt这个库的文档并不是十分详细，有时还没有源码的头文件的注释详细，不过文档也是一份参考，
而且对于迅速浏览这个库的结构，内容有很大帮助，文档包含在`libkf5networkmanagerqt-doc`这个包里，
安装这个包之后可以在qt-creator里的帮助里找到这份文档。

除了上述类相关的，库中还有几个需要了解的**很重要**的函数库(直接在NetworkManager命名空间下的函数)：

| 文件名 | 简介 |
| :- | :- |
| manager.h | 这个头文件包含了很多管理性质的函数，如激活断开一个连接 |
| settings.h | 新增查找一个连接 |
| utils.h | 一些有用的工具函数 |

## 主要流程

以有线网络的编辑举例：
WiredPage将Connection的uuid作为参数去实例化一个ConnectionEditPage，ConnectionEditPage通过uuid从获取到一个Connection对象
和ConnectionSettings对象，进而用ConnectionSettings对象实例化控制中心的Settings类，Connection对象用来在最后保存设置。
控制中心的Settings类通过ConnectionSettings对象可以拿到Connection中包含的各种Setting，
根据自己的类型，使用对应类型的Setting对象去实例化不同的Section， 将各个Section组合在一起，并统一管理。

总之，就是将一个Connection分级，按照界面的分级情况将对应的数据下发给界面所对应的类，
并且最终都会分配到一个Section类上， Section类进行详细工作。

