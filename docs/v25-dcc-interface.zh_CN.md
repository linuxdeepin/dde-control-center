\mainpage dde-control-center
@brief dde-control-center

# dde-control-center
## 接口变更记录
| 时间 | 版本 | 说明 | 控制中心版本号 |
|---|---|---|---|
| 2024.11.8 | 1.0 | 创建 | 6.0.71 |
| 2024.12.2 | 1.0 | 修改main.qml为{name}Main.qml(防止翻译冲突，兼容以前命名) | 6.0.77 |
| 2026.1.14 | 1.1 | 修改{name}.qml为{Name}.qml(qml命名规范) | 6.1.71 |

## V25控制中心新特性
1.  V25控制中心只负责框架设计，具体功能全部由插件实现
2.  V25控制中心支持多级插件系统，支持插件插入到任意位置中
3.  更方便、更精确的搜索功能,更好的搜索交互
4.  高度可定制，可定制任意插件是否显示，若插件支持，可定制任意插件内容是否显示
5.  界面采用qml实现，更灵活，更易维护
6.  插件数据采用C++实现，更高效，与界面完全解偶
7.  插件支持多语言，支持多语言切换
8.  插件显示禁用支持统一配置。配置修改立即生效
## V25控制中心插件安装路径必要说明
1.  V25控制中心插件安装路径为`${CMAKE_INSTALL_LIBDIR}/dde-control-center/plugins_v1.0`
2.  该路径下插件以单个文件夹形式存在，文件夹名为插件名，文件夹内为插件文件,假设插件名为example，则插件文件夹内容为：
```bash
${CMAKE_INSTALL_LIBDIR}/dde-control-center/plugins_v1.0/example/
├── qmldir
├── libexample_qml.so
└── example.so
```
1.  example.so、qmldir为qml插件c++导出的动态库
2.  libexample_qml.so为qml资源文件编译成的动态库。包含资源有：
    1) Example.qml为插件元数据文件，包含一个DccObject对象。通常该对象只是插件入口菜单。为了让主界面快速显示出来
    2) ExampleMain.qml为插件入口文件，插件启动时，会自动加载该文件，该文件中根对象为一个DccObject对象，该对象可以包含任意qml对象，并且该文件中可以用到example.so导出的函数，使用方式为：dccData.xxx(),dccData为example.so导出的对象
    3) Xxx.qml为插件其他文件，在ExampleMain.qml中使用
## V25控制中心插件开发说明
1.  V25控制中心插件开发需要安装dde-control-center-dev包，该包中包含V25控制中心插件开发所需头文件和库文件
2.  V25控制中心使用的是qt6,qt6与qt5混用会导致程序崩溃。因此插件需要使用qt6进行开发
## V25控制中心插件加载顺序说明
1.  插件加载时，会先根据配置判断该插件是否显示，若不显示，则加载结束。查看配置命令：`dde-dconfig get org.deepin.dde.control-center -r org.deepin.dde.control-center hideModule`
2.  以Qt的qml插件形式加载example模块
3.  加载Example.qml，若Example.qml中根对象DccObject对象visible属性为false，则加载结束
4.  在线程中加载example.so，最后会将example.so导出的对象移动到主线程
5.  将example.so导出的对象设置为dccData,加载ExampleMain.qml。此时，ExampleMain.qml中可以使用dccData.xxx()调用example.so导出的函数
6.  加载完成，将DccObject对象插入到模块树中
## V25控制中心插件开发必要说明
1.  控制中心有一个option,可以用来加载一个文件夹下的插件，比如一般插件会放置到`build`文件夹下，这时候可以`dde-control-center --spec ./lib/plugins_v1.0`来加载单独一个插件进行调试。另外提醒，调试时候不要使用asan，因为没有使用asan的控制中心无法加载使用了asan编译的插件
2.  控制中心插件加载是在线程中，但最终会将插件对象移到主线程。所以example.so构造函数中创建的对象需要在example.so导出类的树结构中(即子对象的父对象或祖先对象是example.so导出类)，否则不会被移动到主线程，导致其中信号槽线程等不到，无法正常使用。
3.  example.so导出类是唯一的，插件中不建议使用单例，可在example.so导出类中创建一个单例对象
## V25控制中心开发接口说明
控制中心导出的qml类有：
### 关键类
#### DccObject
| 属性名称 | 说明 | 备注 |
|---|---|---|
| name | 名称 | 作为唯一id使用，结合父项的name组成url,用于定位跳转、配置隐藏禁用等，由字符、数字组成，不建议有符号空格，不可有‘/’(url分隔符，会影响解析) |
| parentName | 父项名称 | 父项的url,表示该项是哪个项的子项。此处可以是一个url,如：“aa/bb/cc” |
| weight | 权重 | 权重越高，该项所插入的位置越靠后。取值范围：0-65535，建议取值用10、20、30的方式，方便有需求要从中间插入控件 |
| displayName | 显示名称 | 用于搜索、显示，需支持翻译 |
| description | 描述 | 用于显示 |
| icon | 图标 | 图标名 |
| badge | 标识 | 用于显示红色圆点等，如：更新的红点提示，取值范围：0-255 |
| visible | 可见 | 与控件显示关联，默认true |
| enabled | 启用 | 与控件状态关联，默认true |
| visibleToApp | 可见 | 只读，包含配置与visible的结果，与控件显示关联 |
| enabledToApp | 启用 | 只读，包含配置与enabled的结果，与控件状态关联 |
| canSearch | 可搜索 | 默认true |
| backgroundType | 背景样式 | 默认AutoBg |
| pageType | 界面类型 | Menu、Editor、Item等，影响page显示方式，取值范围：0-255 |
| page | 界面控件 |  |
| parentItem | 控件父项 |  |

| 信号 | 说明 | 备注 |
|---|---|---|
| active | 激活 | backgroundType为Clickable时，点击控件出发，参数为空。DBus的ShowPage方法出发，如:ShowPage("aa/bb?param=1")，则aa/bb项会收到active("param=1")信号 |
| deactive | 停用 | 页面退出时触发 |
#### DccApp 全局单例
| 函数 | 说明 | 备注 |
|---|---|---|
| root | 根结点 | 属性 |
| activeObject | 当前菜单项 | 属性 |
| addObject | 添加Object | 将DccObject加到模块树上,方法 |
| removeObject | 移除Object | 将DccObject从模块树上移除，动态创建的Object需要手动destroy,方法 |
| showPage | 跳转页面 | 若url参数为空，会按Object的父项查找，直到找到Menu类型的Object,将其设置为当前页面。url参数不为空，则找到对应项触发active信号,方法 |
| mainWindow | 主窗口 | 方法 |
| activeItemChanged | 搜索或showPage对应的控件，常用于强提醒显示 | 信号 |
### 辅助类
#### DccModel
以根结点的子项组成一个ListModel
| 属性名称 | 说明 | 备注 |
|---|---|---|
| root | 根结点 |  |
#### DccRepeater
使用提供的model实例化多个基于DccObject的对象，并添加到父项中，与Repeater类似
#### DccDBusInterface
与DBus交互的类，支持属性、信号、方法
### 界面类
#### DccGroupView
一个组样式的控件
#### DccRightView
控制中心右侧样式控件，Menu类型的DccObject未指定page时，会自动使用该控件
#### DccSettingsView
与DccRightView类似，但可以显示一个下方悬浮区域，需要其对应的DccObject有两个子项
#### DccItemBackground
处理控件背景的控件，在DccRightView中用到
## 代码示例：
### 代码文件夹结构
假设插件名为example，代码文件夹内容为：
```plain
plugin-example
├── CMakeLists.txt          # CMake构建脚本，用于编译和构建插件
├── qml                     # QML文件目录
│   ├── dcc_example.dci     # 图标文件
│   ├── ExamplePage1.qml    # 第一个示例页面的QML文件，在exampleMain.qml中加载
│   ├── ExamplePage2.qml    # 第二个示例页面的QML文件，在exampleMain.qml中加载
│   ├── Example.qml         # 主QML文件，包含简单的插件信息
│   └── ExampleMain.qml     # 主QML文件，包含插件所有页面
└── src                     # 源文件目录，存放C++源文件和相关头文件
    ├── pluginexample.cpp   # 插件的C++实现文件，包含功能实现和QML与C++的交互
    ├── pluginexample.h     # 插件的头文件，定义类、函数和QML中可能用到的接口
    └── resources.qrc       # qrc资源文件（可选）
```

（文件说明按模块加载顺序说明）

### CMakeLists.txt
```bash
cmake_minimum_required(VERSION 3.23) # qt_add_qml_module函数支持最小版本3.18,建议版本3.23+
# 该name会设置为插件名，只支持字母加数字，需要与{Name}.qml中DccObject的name相同，用于插件禁用操作
set(PLUGIN_NAME "example")

find_package(Qt6 COMPONENTS Core LinguistTools REQUIRED) # dcc_handle_plugin_translation中用到LinguistTools的函数
find_package(DdeControlCenter REQUIRED) # 查找dde-control-center库

file(GLOB_RECURSE PLUGIN_SRCS
        "src/*.cpp"
        "src/*.h"
        # "src/qrc/example.qrc"
)
add_library(${PLUGIN_NAME} MODULE
        ${PLUGIN_SRCS}
)

# target_include_directories(${PLUGIN_NAME} PUBLIC
#     Dde::Control-Center
# )

target_link_libraries(${PLUGIN_NAME} PRIVATE
    Dde::Control-Center # 添加dde-control-center库
    Qt6::Core
)
# 处理插件安装
dcc_install_plugin(NAME ${PLUGIN_NAME} TARGET ${PLUGIN_NAME})
# 处理翻译和安装，如果自己处理翻译，可以不调用该函数
dcc_handle_plugin_translation(NAME ${PLUGIN_NAME} )
```
### Example.qml
```javascript
import org.deepin.dcc 1.0

// 该文件中不能使用dccData,根对象为DccObject
DccObject {
    id: root
    name: "example" // 与插件名相同
    parentName: "root"
    displayName: qsTr("Example")
    icon: "dcc_example"
    weight: 1000

    visible: false // 控制模块显示，如果模块不显示，则不会加载example.so和example.qml
    DccDBusInterface { // 控制中心导致的qml类，可使用DBus。如果是用DConfig,dtk有导出D.Config类，可以直接使用
        property var windowRadius // 关注的dbus属性
        service: "org.deepin.dde.Appearance1"
        path: "/org/deepin/dde/Appearance1"
        inter: "org.deepin.dde.Appearance1"
        connection: DccDBusInterface.SessionBus
        onWindowRadiusChanged: { // dbus属性变化信号
            root.visible = windowRadius > 0
        }
        // on为关键字，关联DBus信号
        function onChanged(type, value) { // dbus信号
            console.log("Changed signal received, type: ", type, ", value: ", value)
            // 调用DBus的List方法，["gtk"]为参数，多个参数示例:["str",1,"str"]，listSlot为处理DBus调用返回，参数个数为DBus返回的参数个数。listErrorSlot为处理DBus错误返回，1个参数，为错误字符串
            callWithCallback("List", ["gtk"], listSlot, listErrorSlot)
        }
        function listSlot(ty) {
            console.log("List slot received, type: " + ty)
        }
        function listErrorSlot(error) {
            console.log("error", error)
        }
    }
}
```
### pluginexample.h
```cpp
class PluginExample : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
public:
    explicit PluginExample(QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);
    // 属性、Q_INVOKABLE、信号、槽等可在qml中直接使用
    Q_INVOKABLE int calc(int a, int b);

public Q_SLOTS:
    void setCalcType(int type);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void calcTypeChanged(int calcType);
};
```
### pluginexample.cpp
pluginexample.cpp为PluginExample类实现，与控制中心插件相关内容为：

```cpp
#include "dccfactory.h"
DCC_FACTORY_CLASS(PluginExample) // DCC_FACTORY_CLASS在dccfactory.h中定义，用于注册插件,该宏会自动生成PluginExampleFactory类，并实现create函数。PluginExampleFactory类为Qt类，所以需要包含pluginexample.moc
#include "pluginexample.moc"
```
### ExampleMain.qml
```javascript
import org.deepin.dcc 1.0

// 该文件中可以使用dccData,根对象为DccObject
DccObject {
    ExamplePage1 {
        name: "example_1"
        parentName: "example"
        displayName: qsTr("Normal Page")
        icon: "dcc_example"
        weight: 10
    }
    ExamplePage2 {
        name: "example_2"
        parentName: "example"
        displayName: qsTr("Settings Page")
        icon: "dcc_example"
        weight: 20
    }
}
```
### ExamplePage1.qml
```javascript
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0

DccObject {
    id: root
    property real calcValue
    property real calcType: 0
    DccObject {
        name: "calc"
        parentName: root.name
        displayName: qsTr("calc")
        icon: "dcc_example"
        weight: 10
        backgroundType: DccObject.Normal // 设置背景样式
        pageType: DccObject.Editor // Editor为page是右边的控件，左边显示displayName、icon等
        page: Button {
            text: dccObj.displayName
            onClicked: {
                calcValue = dccData.calc(calcValue, 2)
            }
        }
    }
    DccObject {
        name: "value"
        parentName: root.name
        displayName: qsTr("value")
        weight: 20
        pageType: DccObject.Editor
        backgroundType: DccObject.ClickStyle // ClickStyl表示有点击效果，点击时会发出active信号
        page: RowLayout {
            Text {
                text: calcValue
            }
            ComboBox {}
        }
        onActive: cmd => console.log(this, "onActive:", cmd)
    }
    DccObject {
        name: "group"
        parentName: root.name
        displayName: qsTr("group")
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {} // 组效果，其子项会放在一个组里
        DccObject {
            name: "item2" // name要求当前组内唯一
            parentName: root.name + "/group" // parentName要求可定位到对应项，可用多个DccObject的name组合
            displayName: qsTr("value")
            weight: 20
            pageType: DccObject.Item // Item的page将占整个区域
            page: Text {
                text: calcValue
            }
        }
        DccObject {
            name: "item1"
            parentName: root.name + "/group"
            displayName: qsTr("value")
            weight: 10
            pageType: DccObject.Editor
            page: Text {
                text: calcValue
            }
        }
        DccObject {
            id: calcTypeObj
            name: "calcType"
            parentName: root.name
            displayName: qsTr("calc type")
            description: qsTr("description")
            icon: "dcc_example"
            weight: 30
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Button {
                text: dccObj.displayName
                onClicked: {
                    calcType++
                    if (calcType >= 4) {
                        calcType = 0
                    }
                    dccData.setCalcType(calcType)
                }
            }
            Connections {
                target: dccData
                function onCalcTypeChanged(cType) {
                    calcTypeObj.displayName = cType
                }
            }
        }
    }

    DccObject {
        name: "group2"
        parentName: root.name
        displayName: qsTr("group2")
        weight: 40
        pageType: DccObject.Item
        page: DccGroupView {}
        DccRepeater { // DccRepeater配合model可实现多个DccObject
            model: 3
            delegate: DccObject {
                name: "item" + (index + 1)
                parentName: root.name + "/group2"
                displayName: qsTr("Item") + (index + 1)
                weight: 30 + index
                pageType: DccObject.Editor
                page: Switch {}
            }
        }
    }
}
```
### ExamplePage2.qml
```javascript
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

DccObject {
    id: root
    property real calcValue
    property real calcType: 0

    weight: 10
    page: DccSettingsView {} // 设置界面，此DccObject的pageType为Menu，page为DccSettingsView
    DccObject {
        name: "body" // DccSettingsView里的限制，其子项必须是两个DccObject，第一个为主界面，第二个为下方按钮区域
        parentName: root.name
        weight: 10
        pageType: DccObject.Item
        DccObject {
            name: "calc"
            parentName: root.name + "/body"
            displayName: qsTr("calc")
            icon: "dcc_example"
            weight: 10
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Button {
                text: dccObj.displayName
                onClicked: {
                    calcValue = dccData.calc(calcValue, 2)
                }
            }
        }
        DccObject {
            name: "value"
            parentName: root.name + "/body"
            displayName: qsTr("no Search")
            canSearch: false // 设置界面通常不搜索，可设置canSearch
            weight: 20
            pageType: DccObject.Item
            page: Text {
                text: calcValue
            }
        }
        DccObject {
            name: "menuEditor"
            parentName: root.name + "/body"
            displayName: qsTr("no Search")
            canSearch: false // 设置界面通常不搜索，可设置canSearch
            weight: 30
            pageType: DccObject.MenuEditor // 菜单加编辑控件,子项是一个菜单项
            page: Switch {
            }
            DccObject {
                name: "menu"
                parentName: root.name + "/body/menuEditor"
                weight: 10
                DccRepeater {
                    model: 8
                    delegate: DccObject {
                        name: "item" + (index + 1)
                        parentName: "menuEditor/menu"
                        displayName: qsTr("Item") + (index + 1)
                        weight: 30 + index
                        backgroundType: DccObject.Normal
                        pageType: DccObject.Editor
                        page: Switch {}
                    }
                }
            }
        }
        DccObject {
            name: "calcType" // 该DccObject会显示在example_2中
            parentName: "example_2/body" // DccObject位置只与parentName和weight有关，与其自身位置无关
            weight: 80
            displayName: qsTr("calc type")
            pageType: DccObject.Editor
            backgroundType: DccObject.Normal
            page: Text {
                text: calcType
            }
        }
        DccObject {
            name: "group2"
            parentName: root.name + "/body"
            displayName: qsTr("group2")
            weight: 100
            pageType: DccObject.Item
            page: DccGroupView {}

            DccObject {
                name: "item0"
                parentName: root.name + "/body/group2"
                displayName: qsTr("value")
                weight: 20
                pageType: DccObject.Item
                page: Rectangle {
                    implicitHeight: 50
                    Text {
                        anchors.centerIn: parent
                        text: calcValue
                    }
                }
            }
            // DccRepeater类可配合model实现多个DccObject
            DccRepeater {
                model: 23
                delegate: DccObject {
                    name: "item" + (index + 1)
                    parentName: root.name + "/body/group2"
                    displayName: qsTr("Item") + (index + 1)
                    weight: 30 + index
                    pageType: DccObject.Editor
                    page: Switch {}
                }
            }
        }
    }
    DccObject {
        name: "footer"
        parentName: root.name
        weight: 20
        pageType: DccObject.Item
        DccObject {
            name: "delete"
            parentName: root.name + "/footer"
            weight: 10
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Delete")
                onClicked: {
                    deleteDialog.createObject(this).show()
                }
            }
        }

        Component {
            id: deleteDialog
            D.DialogWindow {
                modality: Qt.ApplicationModal
                width: 380
                icon: "preferences-system"
                onClosing: destroy(10)
                ColumnLayout {
                    width: parent.width
                    Label {
                        Layout.fillWidth: true
                        Layout.leftMargin: 50
                        Layout.rightMargin: 50
                        text: qsTr("Are you sure you want to delete this configuration?")
                        font.bold: true
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                    RowLayout {
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                        Button {
                            Layout.fillWidth: true
                            text: qsTr("Cancel")
                            onClicked: close()
                        }
                        Rectangle {
                            implicitWidth: 2
                            Layout.fillHeight: true
                            color: this.palette.button
                        }

                        Button {
                            Layout.fillWidth: true
                            text: qsTr("Delete")
                            onClicked: {
                                close()
                            }
                        }
                    }
                }
            }
        }
        DccObject {
            // 按钮区域可加个空项处理右对齐问题
            name: "spacer"
            parentName: root.name + "/footer"
            weight: 20
            pageType: DccObject.Item
            page: Item {
                Layout.fillWidth: true
            }
        }
        DccObject {
            name: "cancel"
            parentName: root.name + "/footer"
            weight: 30
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Cancel")
                onClicked: {
                    DccApp.showPage(root.parentName)
                }
            }
        }
        DccObject {
            name: "save"
            parentName: root.name + "/footer"
            weight: 40
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Save")
                onClicked: {
                    calcValue = dccData.calc(calcValue, 3)
                }
            }
        }
    }
}
```
