# QML API 参考

## 概述

控制中心在 `org.deepin.dcc` 模块中提供了 QML 组件。`import org.deepin.dcc 1.0` 后可直接使用。

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
