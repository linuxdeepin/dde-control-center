# QML API 参考

## 概述

控制中心在 `org.deepin.dcc` 模块中提供了 QML 组件。`import org.deepin.dcc 1.0` 后可直接使用。

## QML 加载机制

### 插件 QML 是如何被加载上来的？

每个插件目录下有三个关键文件（v1.1 格式）：

```
plugins_v1.1/{plugin-name}/
├── qmldir                      # QML 模块描述文件（由 cmake 自动生成）
├── {Name}.qml                  # 入口元数据（菜单项）
├── {Name}Main.qml              # 完整 UI 实现（按需加载）
├── {plugin-name}.so            # C++ 后端（可选，通过 QPluginLoader 加载）
```

加载链路如下：

```
DccPluginLoader 状态机
│
├── MetaData 阶段 → updateType() 探测插件格式
│   └── v1.1: 检查 qmldir 是否存在 → 设置 T_HasModule | T_HasMain
│   └── v1.0: 检查 {name}.qml / {name}Main.qml 是否存在
│
├── Module 阶段 → loadModule() 加载 {name}.qml
│   ├── v1.1: component.loadFromModule(name(), typeName)
│   │         └── Qt QML 模块系统，通过 qmldir 定位 QML 文件
│   └── v1.0: component.loadUrl(path + "/" + name() + ".qml")
│              └── 直接加载 QML 文件路径
│   └── 创建 DccObject → 作为 module() 挂到 rootModule 下
│       └── 含 name/parentName/displayName/icon 等菜单元数据
│
├── Data 阶段 → loadData() 加载 {name}.so（详见下文）
│
└── MainObj 阶段 → loadMain() 加载 {name}Main.qml
    ├── 用户导航到该模块时才触发
    └── 原理同 Module 阶段，创建完整 UI 页面的 DccObject
```

### CMakeLists.txt 中为什么不需要包含 QML 文件？

这是因为 `dcc_install_plugin()` 宏内部自动调用了 `dcc_build_plugin()`，而该宏使用 `qt_add_qml_module()` 自动处理 QML 文件：

```cmake
# 用户写的 CMakeLists.txt —— 极其简洁
dcc_install_plugin(NAME ${PLUGIN_NAME} TARGET ${PLUGIN_NAME})
```

**`dcc_build_plugin()` 宏内部自动做了以下工作：**

1. **自动扫描 QML 文件**：`file(GLOB_RECURSE QML_FILES qml/*.qml qml/*.js)` — 无需手动列出
2. **自动扫描资源文件**：`file(GLOB_RECURSE RESOURCE_FILES qml/*.dci qml/*.svg qml/*.png ...)`
3. **生成 QML 模块**：调用 `qt_add_qml_module()` 创建 QML 模块
4. **生成 qmldir**：Qt 自动生成模块描述文件，供 `loadFromModule()` 定位
5. **设置文件别名**：QML 文件名首字母自动大写作为资源别名
6. **处理 `main.qml` → `{Name}Main.qml` 重命名检测**：如果检测到 `main.qml` 会报错提示重命名
7. **安装到插件目录**：`install(TARGETS ${NAME}_qml DESTINATION plugins_v1.1/${NAME})`
8. **自动 MOC**：对使用了 `DCC_FACTORY_CLASS` 宏的源文件自动运行 MOC

**⚠️ 文件名规则**：插件 v6.0.77+ 要求 QML 文件名**首字母必须大写**，且不再允许 `main.qml`，必须用 `{Name}Main.qml`。宏中对此有强制校验。

### .so 文件是怎么被加载起来的？

.so 的加载链路如下：

```
DccPluginLoader::loadData()
│
├── 1. 拼接路径: m_path + "/" + name() + ".so"
│
├── 2. QPluginLoader loader(soPath)
│   ├── loader.load() —— 通过 Qt 插件系统加载动态库
│   ├── 验证 IID == "org.deepin.dde.dcc-factory/v1.0"
│   └── 创建工厂实例: QObject *instance = loader.instance()
│
├── 3. 将 instance 转型为 DccFactory*
│   └── m_factory = qobject_cast<DccFactory*>(instance)
│
├── 4. createData() → m_factory->create(parent)
│   ├── 在后台线程执行（通过 LoadDataTask + QThreadPool）
│   └── 返回的 QObject* 作为数据对象，导出为 dccData 供 QML 使用
│
└── 5. createDccObject() → 将数据注册到全局 DccObject 树
    └── moveThread() → 将数据对象移动到主线程
```

**`DCC_FACTORY_CLASS` 宏的工作机制**（定义在 `include/dccfactory.h`）：

```cpp
#define DCC_FACTORY_CLASS(classname)
    class Q_DECL_EXPORT classname##DccFactory : public dccV25::DccFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID DccFactory_iid)    // 声明插件 IID
        Q_INTERFACES(dccV25::DccFactory)          // 声明实现的接口
    public:
        QObject *create(QObject *parent) override {
            return new classname(parent);         // 创建插件主对象
        }
    };

// 用户只需写一行：
DCC_FACTORY_CLASS(MyPlugin)
#include "myplugin.moc"
```

**关键点：**
- `Q_PLUGIN_METADATA` + `Q_INTERFACES` 让 Qt 的 `QPluginLoader` 能够发现并验证该插件
- `DCC_FACTORY_CLASS` 宏自动生成一个 `Q_DECL_EXPORT` 的工厂子类
- `.so` 中不需要 `main()` 函数，Qt 插件系统通过 IID 匹配加载入口
- `DccPluginLoader::loadData()` 在 `QThreadPool` 后台线程执行，不阻塞 UI
- 加载完成后通过 `moveThread()` 将数据对象移动到主线程

## 索引

| 组件 | 用途 | 场景 |
|------|------|------|
| `DccObject` | 树节点 | 所有界面项的基础类型 |
| `DccApp` | 全局单例 | 页面跳转、模块管理 |
| `DccGroupView` | 组容器 | 分隔线和圆角组 |
| `DccSettingsView` | 设置页容器 | body+footer 设置页 |
| `DccSettingsObject` | 设置页模板 | 快捷创建设置页 |
| `DccRowView` | 行容器 | 水平排列按钮/控件 |
| `DccRightView` | 右侧视图 | Menu 默认 page |
| `DccRepeater` | 动态生成 | 批量创建 DccObject |
| `DccLoader` | 页面加载器 | 加载 DccObject.page |
| `DccItemBackground` | 背景渲染器 | 多状态背景 |
| `DccLabel` | 标签 | 自动省略+悬浮提示 |
| `DccTitleObject` | 分组标题 | 标题+描述 |
| `DccCheckIcon` | 勾选图标 | 切换选中状态 |
| `DccTimeRange` | 时间选择器 | 时:分格式 |
| `DccDBusInterface` | DBus 交互 | 属性绑定、信号、方法调用 |

## 布局容器

### DccGroupView

组样式容器，根据子项 `pageType` 渲染，所有子项放在一个组中。

| 属性 | 类型 | 默认值 | 说明 |
|------|------|-------|------|
| `isGroup` | `bool` | `true` | true=无间距显示分隔线，false=有间距无分隔线 |
| `spacing` | `real` | — | 子项间距 |
| `model` | `DccModel` | `DccModel{root:dccObj}` | 数据模型 |

```qml
DccObject {
    pageType: DccObject.Item
    page: DccGroupView { isGroup: true }
    // 子 DccObject 自动在组内渲染
}
```

### DccSettingsView

设置页面滚动视图，支持 body 内容区 + footer 按钮区。

| 属性 | 类型 | 默认值 | 说明 |
|------|------|-------|------|
| `isGroup` | `bool` | `false` | 组样式 |
| `spacing` | `real` | `0` | 间距 |
| `scrollBarVisible` | `bool` | `false` | 滚动条 |

使用要求：DccObject 必须有恰好两个子对象（body + footer）。

```qml
DccObject {
    pageType: DccObject.Menu
    page: DccSettingsView {}
    DccObject { name: "body"; parentName: root.name; ... }   // 内容区
    DccObject { name: "footer"; parentName: root.name; ... }  // 按钮区
}
```

### DccSettingsObject

封装 DccSettingsView 结构的模板，预创建 body/footer。

| 属性 | 类型 | 说明 |
|------|------|------|
| `bodyUrl` | `string`（只读） | body 的 URL |
| `footerUrl` | `string`（只读） | footer 的 URL |

```qml
DccSettingsObject {
    name: "settings"
    parentName: "myplugin"
    DccObject { name: "opt"; parentName: root.bodyUrl; pageType: DccObject.Editor; page: Switch {} }
    DccObject { name: "save"; parentName: root.footerUrl; pageType: DccObject.Item; page: Button {} }
}
```

### DccRowView

横向排列子项的容器（`RowLayout`，spacing=10）。常用于 footer 按钮区域。

### DccRightView

右侧可滚动视图，包裹单个 DccGroupView。Menu 类型 DccObject 未指定 page 时默认使用。

## 内容控件

### DccTitleObject

分组标题，自动设置 `pageType: DccObject.Item`，渲染标题（DTK t5 字体）+ 描述（DTK t8 字体，半透明）。

```qml
DccTitleObject {
    name: "sectionTitle"
    parentName: "myplugin"
    displayName: qsTr("Display Settings")
    description: qsTr("Configure your display")
    weight: 5
}
```

### DccLabel

带自动省略（ElideRight）和悬浮 ToolTip 的 Label。`hovered` 属性（bool）表示是否悬浮。

### DccCheckIcon

勾选图标（`D.ActionButton`），`size` 属性（默认 16）。选中显示 `item_checked`，未选中 `item_unchecked`。

### DccTimeRange

时间范围选择器（`D.SpinBox`），格式 "时:分"。

| 属性 | 类型 | 说明 |
|------|------|------|
| `hour` | `int` | 小时（0-23） |
| `minute` | `int` | 分钟（0-59） |
| `timeString` | `string`（只读） | "HH:MM" |

信号：`timeChanged()` — 停止编辑 500ms 后触发。

## 辅助组件

### DccLoader

加载 DccObject.page 的 Loader。属性：`dccObj`（var）、`dccObjItem`（Item）。自动设置 `dccObj.parentItem`。

### DccRepeater

动态创建 DccObject 子项。见 [cpp-api.md](cpp-api.md) 中的 C++ API 部分。

### DccDBusInterface

DBus 交互组件，支持属性绑定、信号监听和方法调用。

| 属性 | 类型 | 说明 |
|------|------|------|
| `service` | `string` | DBus 服务名 |
| `path` | `string` | 对象路径 |
| `inter` | `string` | 接口名 |
| `connection` | `BusType` | `SessionBus`(0) 或 `SystemBus`(1) |
| `suffix` | `string` | 属性前缀（避免 QML 保留字冲突） |

**属性绑定：** 声明 `property var xxx` 自动绑定 DBus 属性，变化时触发 `onXxxChanged`。

**信号监听：** 声明 `function onXxx(args)` 监听 DBus 信号。

**方法调用：** `callWithCallback(method, args, callback, errorCallback)`

```qml
DccDBusInterface {
    property var windowRadius
    service: "org.deepin.dde.Appearance1"
    path: "/org/deepin/dde/Appearance1"
    inter: "org.deepin.dde.Appearance1"
    connection: DccDBusInterface.SessionBus

    onWindowRadiusChanged: root.visible = windowRadius > 0

    function onChanged(type, value) {
        callWithCallback("List", ["gtk"], listSlot, listErrorSlot)
    }
    function listSlot(result) { console.log(result) }
    function listErrorSlot(error) { console.log(error) }
}
```
