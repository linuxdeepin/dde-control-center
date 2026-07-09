# 插件开发

## 概述

控制中心插件是一个独立的 CMake 子项目，包含 QML 界面文件和可选的 C++ 数据模块。本文涵盖从创建项目到打包、翻译、安装的完整流程。

## 目录结构

```
src/plugin-{name}/
├── CMakeLists.txt
├── qml/
│   ├── {Name}.qml          # 模块元数据（根 DccObject）
│   ├── {Name}Main.qml       # 主页面（组合子页面）
│   ├── {Name}Page1.qml      # 子页面
│   ├── {name}_icon.dci      # 图标
│   └── metadata.json        # {"Version": "1.0"}
├── operation/               # C++ 源码（可选）
│   ├── {name}module.h/cpp   # 数据模块（DCC_FACTORY_CLASS）
│   ├── {name}worker.h/cpp   # 后台工作类
│   └── {name}dbusproxy.h/cpp # DBus 代理
└── translations/            # 翻译文件
```

## 命名约束

| 约束 | 说明 |
|------|------|
| 插件名 | 只能字母和数字，如 `myplugin` |
| 根 DccObject name | 必须与 CMake 中 `PLUGIN_NAME` 完全一致 |
| QML 文件名 | 必须首字母大写，如 `Myplugin.qml` |
| `main.qml` | 已废弃，须用 `{Name}Main.qml` |
| {Name}.qml 中不能用 dccData | C++ 插件此时尚未加载 |

## CMakeLists.txt 模板

```cmake
cmake_minimum_required(VERSION 3.23)
set(PLUGIN_NAME "myplugin")

find_package(Qt6 COMPONENTS Core LinguistTools REQUIRED)
find_package(DdeControlCenter REQUIRED)

# 可选额外依赖
# find_package(Qt6 COMPONENTS DBus REQUIRED)
# find_package(Dtk6 COMPONENTS Core Gui REQUIRED)

file(GLOB_RECURSE PLUGIN_SRCS "operation/*.cpp" "operation/*.h")
add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SRCS})

target_link_libraries(${PLUGIN_NAME} PRIVATE
    Dde::Control-Center
    Qt6::Core
)
# 按需添加 Qt6::DBus、Dtk6::Gui 等

dcc_install_plugin(NAME ${PLUGIN_NAME} TARGET ${PLUGIN_NAME})
dcc_handle_plugin_translation(NAME ${PLUGIN_NAME})
```

### dcc_install_plugin

`dcc_install_plugin(NAME <name> TARGET <target>)`
- `NAME` — 插件名，同时作为 QML 模块 URI
- `TARGET` — C++ target 名（无 C++ 代码则省略）
- 自动收集 `qml/` 下所有 `.qml`、`.js`、`.dci`、`.svg`、`.png`、`.jpg`、`.webp` 文件
- 验证 QML 文件名首字母大写
- 验证不允许 `main.qml`
- 设置 `AUTOMOC_MACRO_NAMES` 为 `DCC_FACTORY_CLASS`

### dcc_handle_plugin_translation

`dcc_handle_plugin_translation(NAME <name>)`
- 自动处理 20 种 locale 的 TS 文件
- 在 `translations/` 目录下创建 `${name}_xx.ts` 文件
- TS 文件通过 tx 工具推送到翻译平台进行翻译

## QML-Only 插件

只需 QML 文件，CMakeLists.txt 极简：

```cmake
cmake_minimum_required(VERSION 3.18)
set(Plugin_Name system)
dcc_install_plugin(NAME ${Plugin_Name})
```

无需 `add_library()`，无需 `DCC_FACTORY_CLASS`。通常用作分组容器（如 `system`、`device`）。

## {Name}.qml — 模块元数据

插件的入口 QML 文件，定义在导航中的位置。**此文件中不能使用 dccData**。

```qml
import org.deepin.dcc 1.0

DccObject {
    name: "myplugin"           // 与 CMake PLUGIN_NAME 相同
    parentName: "root"         // 父模块 URL
    displayName: qsTr("My Plugin")
    icon: "myplugin_icon"
    weight: 100

    // 通过 DBus 条件控制模块显隐
    visible: false
    DccDBusInterface {
        property var available
        service: "com.example.Service"
        path: "/com/example"
        inter: "com.example.Service"
        connection: DccDBusInterface.SessionBus
        onAvailableChanged: root.visible = available > 0
    }
}
```

## {Name}Main.qml — 主页面

组合子页面。**此文件中可以使用 dccData**。

```qml
import org.deepin.dcc 1.0

DccObject {
    MypluginPage1 {
        name: "page1"
        parentName: "myplugin"
        displayName: qsTr("Normal Page")
        icon: "myplugin_icon"
        weight: 10
    }
    MypluginPage2 {
        name: "page2"
        parentName: "myplugin"
        displayName: qsTr("Settings Page")
        icon: "myplugin_icon"
        weight: 20
    }
}
```

## C++ 数据模块

```cpp
// mypluginmodule.h
#include <QObject>

class MyPluginModule : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
public:
    explicit MyPluginModule(QObject *parent = nullptr);
    QString name() const;
    void setName(const QString &name);
    Q_INVOKABLE int calc(int a, int b);
Q_SIGNALS:
    void nameChanged(const QString &name);
private:
    QString m_name;
};
```

```cpp
// mypluginmodule.cpp
#include "mypluginmodule.h"
#include "dccfactory.h"

MyPluginModule::MyPluginModule(QObject *parent) : QObject(parent) {}

QString MyPluginModule::name() const { return m_name; }
void MyPluginModule::setName(const QString &name) {
    if (m_name != name) { m_name = name; Q_EMIT nameChanged(m_name); }
}
int MyPluginModule::calc(int a, int b) { return a + b; }

DCC_FACTORY_CLASS(MyPluginModule)
#include "mypluginmodule.moc"
```

### DCC_FACTORY_CLASS 宏

定义在 `dccfactory.h`，在匿名 namespace 中生成工厂类 `{classname}DccFactory`，继承 `dccV25::DccFactory`，实现 `create()` 返回 `new classname(parent)`。

要点：
- 必须在 `.cpp` 中调用（不能在头文件中）
- 调用后必须 `#include "{classname}.moc"`（不是 `.h`）
- 每个插件只有一个入口类调用此宏
- `create()` 返回的 QObject 在 QML 中作为 `dccData` 访问

## C++ 架构模式

### 简单模式（示例风格）

只有一个类处理所有逻辑，直接 `DCC_FACTORY_CLASS`。

### Interaction-Model-Worker 三层（Bluetooth 风格）

```
DCC_FACTORY_CLASS(Interaction)
  → Interaction (QObject)
      ├── Model (QObject, 数据持有)
      └── Worker (QObject, 后台操作)
            └── DBusProxy (Dtk::Core::DDBusInterface)
```

Worker 在构造函数中调用 `activate()` 开始数据同步。QML 通过 `dccData.model()` / `dccData.work()` 访问。

### 单层 Model（Mouse 风格）

Model 本身作为工厂类，30+ 个 Q_PROPERTY 直接暴露所有设置项，Worker 作为 friend class 直接操作私有成员。

## 线程安全

`DccFactory::create()` 在线程池中执行，完成后 data 对象移动到主线程。构造函数中创建的所有 QObject 必须纳入 QObject 树（以 data 对象为 parent 或其子孙），否则不会随主线程移动。

```cpp
// 错误：m_worker 无 parent，停留在线程池线程
MyModule::MyModule(QObject *parent) : QObject(parent) {
    m_worker = new Worker();  // 无 parent！
}

// 正确：所有子对象都有 parent
MyModule::MyModule(QObject *parent) : QObject(parent) {
    m_worker = new Worker(this);
}
```

## Debian 打包

插件打包只需两个控制中心相关依赖：

- 构建依赖：**`dde-control-center-dev`** — 提供头文件（`dccfactory.h`）、CMake 配置（`find_package(DdeControlCenter)`）、插件 CMake 宏
- 运行时依赖：**`dde-control-center`** — 提供插件加载框架和运行环境

`dde-control-center-dev` 自身依赖 `dde-control-center`、`qt6-declarative-dev`、`qt6-tools-dev`，会间接拉入所需的基础 Qt 包。

插件文件安装到 `/usr/lib/{multiarch}/dde-control-center/plugins_v1.1/{name}/`，翻译安装到 `/usr/share/dde-control-center/translations/v1.1/`。

## 翻译

QML 中使用 `qsTr()`，C++ 中使用 `tr()`（需类继承 `QObject` 并使用 `Q_OBJECT` 宏）。

`dcc_handle_plugin_translation` 自动处理 20 种 locale。TS 文件通过 tx 工具推送到翻译平台进行翻译，不需要手动编辑。

## 编译调试

```bash
mkdir build && cd build && cmake .. && make -j$(nproc)

# 单插件调试，无需安装
dde-control-center --spec ./lib/plugins_v1.1/
```

不能混用 ASAN：控制中心无 ASAN 时，不能加载使用 ASAN 编译的插件。

### 常见错误速查

| 问题 | 检查 |
|------|------|
| 插件不显示 | hideModule、{Name}.qml 中 visible、DBus 条件 |
| dccData 为 undefined | 是否在 {Name}.qml 中访问、DCC_FACTORY_CLASS 是否正确、include 是否是 .moc |
| QML 修改无效 | 文件名首字母大写、非 main.qml、清除 QML 缓存 |
| 加载崩溃 | 线程安全（子对象 parent）、空指针 |

详见 [debugging.md](debugging.md)。

## 示例插件

控制中心源码提供完整示例 `examples/plugin-example/`：

```
plugin-example/
├── CMakeLists.txt
├── src/
│   ├── pluginexample.h       # PluginExample 类 (Q_PROPERTY, Q_INVOKABLE)
│   └── pluginexample.cpp     # DCC_FACTORY_CLASS(PluginExample)
├── qml/
│   ├── Example.qml           # name:"example", parentName:"root"
│   ├── ExampleMain.qml       # 组合 ExamplePage1/2/3
│   ├── ExamplePage1.qml      # Editor + DccGroupView + DccRepeater
│   ├── ExamplePage2.qml      # Menu + DccSettingsView (body/footer)
│   ├── ExamplePage3.qml      # DccSettingsObject 快捷方式
│   └── dcc_example.dci
└── translations/
```

### ExamplePage1 关键模式

- **Editor** — 左标签 + 右控件：`pageType: DccObject.Editor` + `page: Button { ... }`
- **DccGroupView** — 组容器，自动渲染分隔线和圆角
- **DccRepeater** — 用 model 动态创建多个 DccObject：`model: 3; delegate: DccObject { ... }`
- **onActive** — `backgroundType: DccObject.ClickStyle` 时点击触发
- **Connections** — 在 QML 中监听 C++ 信号：`target: dccData; function onXxxChanged(v) { ... }`

### ExamplePage2 关键模式

- **Menu + DccSettingsView** — `pageType: DccObject.Menu` + `page: DccSettingsView {}`
- **body/footer** — 必须有恰好两个子 DccObject（body + footer）
- **footer** — 使用 `DccRowView` 水平排列按钮，可加空 `Item { Layout.fillWidth: true }` 实现右对齐

### QML 上下文变量

| 变量 | 来源 | 说明 |
|------|------|------|
| `dccData` | `DccFactory::create()` | C++ 数据对象 |
| `dccObj` | 当前 DccObject | 当前正在渲染的 DccObject |
| `DccApp` | 全局单例 | 应用导航和管理 |
