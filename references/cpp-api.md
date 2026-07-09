# C++ API 参考

## 概述

控制中心开发包提供以下 C++ 接口。所有公共类位于 `dccV25` 命名空间。

**头文件：** 安装后位于 `include/` 目录，CMake 通过 `find_package(DdeControlCenter)` + `Dde::Control-Center` 链接。

## 索引

| 类 | 职责 | 使用场景 |
|------|------|----------|
| `DccObject` | 树形数据节点 | 定义界面中的菜单项或功能项 |
| `DccApp` | 全局单例 | 页面导航、模块树管理 |
| `DccFactory` | 插件工厂接口 | 插件注册入口 |
| `DccModel` | QAbstractItemModel | 列表视图数据模型 |
| `DccRepeater` | 动态 DccObject 生成 | 批量创建界面项 |

## DccObject

**头文件：** `dccobject.h`
**QML 类型：** `DccObject` (QML_ELEMENT)

表示界面中一个菜单项或功能项的树形节点。所有 DccObject 组成树形结构，通过 `parentName` (URL 路径如 `"system/display"`) 建立父子关系。

### 属性

| 属性 | 类型 | 默认值 | 说明 |
|------|------|-------|------|
| `name` | `QString` | `""` | 唯一标识符，不能含 `/` |
| `parentName` | `QString` | `""` | 父项 URL 路径 |
| `weight` | `quint32` | `0` | 排序权重，越大越靠后（建议 10/20/30） |
| `displayName` | `QString` | `""` | 显示名称 |
| `description` | `QString` | `""` | 描述文本 |
| `icon` | `QString` | `""` | 图标名（对应 .dci 文件，不含后缀） |
| `badge` | `qint8` | `0` | 角标（0=无，非0=红点） |
| `visible` | `bool` | `true` | 运行时是否可见 |
| `enabled` | `bool` | `true` | 运行时是否启用 |
| `visibleToApp` | `bool` | 只读 | 结合 hideModule 后的可见状态 |
| `enabledToApp` | `bool` | 只读 | 结合 disableModule 后的启用状态 |
| `canSearch` | `bool` | `true` | 是否参与搜索 |
| `backgroundType` | `BackgroundTypes` | `AutoBg` | 背景行为标志 |
| `currentObject` | `DccObject*` | `nullptr` | 当前选中的子对象 |
| `children` | `QVector<DccObject*>` | 只读 | 子对象列表 |
| `pageType` | `quint8` | `Menu(0x40)` | 页面类型 |
| `page` | `QQmlComponent*` | `nullptr` | 界面对应的 QML 组件 |
| `parentItem` | `QQuickItem*` | `nullptr` | 对应的 QQuickItem 父项 |

### PageType 枚举

| 常量 | 值 | 场景 |
|------|------|------|
| `Menu` | `0x40` (64) | 菜单项，点击进入子页面；page 默认 DccRightView |
| `MenuEditor` | `0x41` (65) | 菜单带编辑控件，子项是菜单 |
| `Editor` | `0x21` (33) | 左标签 + 右控件（如 Switch） |
| `Item` | `0x22` (34) | 全宽控件，page 占满整行 |
| `EditorPage` | `1` | 编辑控件页面基值 |
| `ItemPage` | `2` | 控件页面基值 |
| `UserType` | `0x80` (128) | 用户自定义 |

### BackgroundType 枚举（位标志）

| 常量 | 值 | 效果 |
|------|------|------|
| `AutoBg` | `0` | 自动：Menu→ClickStyle，Group→Normal，其他无背景 |
| `Normal` | `0x01` | 正常背景 |
| `Hover` | `0x02` | 鼠标悬浮背景 |
| `Clickable` | `0x04` | 可点击，触发 `active` 信号 |
| `Highlight` | `0x08` | 高亮背景（选中状态） |
| `Warning` | `0x10` | 红色警告背景 |
| `ClickStyle` | `Normal\|Hover\|Clickable` | 可点击样式 |

### 信号

**激活：** `active(const QString &cmd)` — 点击或 ShowPage 触发；`deactive()` — 页面退出

**子对象变化：** `childAboutToBeAdded`、`childAdded`、`childAboutToBeRemoved`、`childRemoved`、`childrenChanged`

**属性变化：** 每个属性有对应的 `xxxChanged` 信号

### 使用要点

```qml
// Editor: 左标签 + 右控件
DccObject { pageType: DccObject.Editor; page: Switch {} }

// Item: 全宽控件
DccObject { pageType: DccObject.Item; page: DccGroupView {} }

// Menu: 菜单项，子页面
DccObject { pageType: DccObject.Menu; page: DccSettingsView {} }

// 点击效果
DccObject { backgroundType: DccObject.ClickStyle; onActive: cmd => { ... } }
```

## DccApp

**头文件：** `dccapp.h`
**QML 类型：** `DccApp` (qmlRegisterSingletonInstance)

全局单例，管理模块树和页面导航。

### 属性

| 属性 | 类型 | 说明 |
|------|------|------|
| `root` | `DccObject*` | 根节点（只读） |
| `activeObject` | `DccObject*` | 当前活动对象（只读） |
| `sidebarWidth` | `int` | 侧边栏宽度（默认 180） |
| `width`/`height` | `int` | 窗口尺寸（只读） |

### 方法

| 方法 | 说明 |
|------|------|
| `showPage(url)` | 跳转到指定页面 URL |
| `addObject(obj)` | 添加 DccObject 到模块树 |
| `removeObject(obj)` | 移除（动态创建需手动 destroy） |
| `object(name)` | 按名称查找 DccObject |
| `mainWindow()` | 主窗口 |
| `navModel()` | 导航模型 |
| `searchModel()` | 搜索模型 |

```qml
DccApp.showPage("system/display/brightness")
DccApp.showPage("")  // 返回上一级
let obj = DccApp.object("system/display")
```

## DccFactory

**头文件：** `dccfactory.h`
**接口 IID：** `"org.deepin.dde.dcc-factory/v1.0"`

插件工厂基类，插件通过 `DCC_FACTORY_CLASS` 宏注册。

| 虚方法 | 说明 |
|--------|------|
| `QObject *create(QObject *parent)` | 返回数据对象，导出为 QML 中的 `dccData` |
| `DccObject *dccObject(QObject *parent)` | 未提供 QML 时自行返回 DccObject |

### DCC_FACTORY_CLASS 宏

在 `.cpp` 中调用，生成工厂插件类。`create()` 返回 `new classname(parent)`。调用后必须 `#include "{classname}.moc"`。

```cpp
#include "dccfactory.h"
DCC_FACTORY_CLASS(MyModule)
#include "mymodule.moc"
```

## DccRepeater

**头文件：** `dccrepeater.h`
**QML 类型：** `DccRepeater` (QML_ELEMENT)

继承 DccObject，用 model 动态创建子 DccObject。

| 属性 | 类型 | 说明 |
|------|------|------|
| `model` | `QVariant` | 数据源 |
| `delegate` | `QQmlComponent*` | 模板组件 |
| `count` | `int` | 生成数量（只读） |

| 方法 | 说明 |
|------|------|
| `resetModel()` | 重建 |
| `objectAt(int)` | 获取指定索引的 DccObject |

| 信号 | 说明 |
|------|------|
| `objAdded(int, QObject*)` | 新对象添加 |
| `objRemoved(int, QObject*)` | 对象移除 |

```qml
DccRepeater {
    model: 5
    delegate: DccObject {
        name: "item" + index
        parentName: "myplugin/group"
        weight: 10 + index
        pageType: DccObject.Editor
        page: Switch {}
    }
}
```
